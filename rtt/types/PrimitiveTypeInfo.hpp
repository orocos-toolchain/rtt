#ifndef ORO_PRIMITIVE_TYPEINFO_HPP
#define ORO_PRIMITIVE_TYPEINFO_HPP

#include "Types.hpp"
#include "../Property.hpp"
#include "../Attribute.hpp"
#include "../Logger.hpp"
#include "TypeStreamSelector.hpp"

#include "../rtt-config.h"

namespace RTT
{
    namespace types {

    /**
     * This template class allows primitive types, which are not sent over ports, to be added to Orocos.
     * The type is known to the type system, but you can not use it in flow ports.
     *
     * @param T The user class type.
     * @param use_ostream When set to \a true, the class will use operator<<(std::ostream&, T)
     * to write out the type to a stream. When set to \a false, the class will use this function
     * and write '( \a type \a name )' to a stream instead. Defaults to \a false. Set to \a true
     * if your class \a T has the above mentioned function.
     * @see TypeInfoRepository.
     * @see operator<<
     * @see StructTypeInfo, SequenceTypeInfo, CArrayTypeInfo, BoostArrayTypeInfo
     */
    template<typename T, bool use_ostream = false>
    class PrimitiveTypeInfo
        : public TypeInfo
    {
    protected:
        const std::string tname;
    public:
        using TypeInfo::buildConstant;
        using TypeInfo::buildVariable;

        /**
         * The given \a T parameter is the type of the DataSources.
         */
        typedef T DataType;

        /**
         * Setup Type Information for type \a name.
         * This causes a switch from 'unknown' type to basic
         * type information for type T.
         * @param name the 'Orocos' type name.
         *
         */
        PrimitiveTypeInfo(std::string name)
            : tname(name)
        {
        }

        virtual ~PrimitiveTypeInfo()
        {
            if ( internal::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject == this)
                internal::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject = 0;
        }

        bool installTypeInfoObject() {
            // Install the type info object for T.
            TypeInfo* orig = internal::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject;
            if ( orig != 0) {
                std::string oname = orig->getTypeName();
                if ( oname != tname ) {
                    log(Info) << "TypeInfo for type '" << tname << "' already exists as '"
                              << oname
                              << "': I'll alias the original and install the new instance." << endlog();
                    this->migrateProtocols( orig );
                    Types()->aliasType( oname, this); // deletes orig !
                }
            } else {
                // check for type name conflict (ie "string" for "std::string" and "Foo::Bar"
                if ( Types()->type(tname) ) {
                    log(Error) << "You attemted to register type name "<< tname << " which is already "
                               << "in use for a different C++ type." <<endlog();
                    return false;
                }
            }
            // finally install it:
            internal::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject = this;
            return true;
        }

        base::AttributeBase* buildConstant(std::string name, base::DataSourceBase::shared_ptr dsb) const
        {
            typename internal::DataSource<DataType>::shared_ptr res =
                boost::dynamic_pointer_cast< internal::DataSource<DataType> >( internal::DataSourceTypeInfo<DataType>::getTypeInfo()->convert(dsb));
            if ( res ) {
                res->get();
                Logger::log() << Logger::Info << "Building "<<tname<<" Constant '"<<name<<"' with value "<< dsb->getTypeInfo()->toString(dsb) <<Logger::endl;
                return new Constant<DataType>( name, res->rvalue() );
            }
            else
                return 0;
        }

        base::AttributeBase* buildVariable(std::string name) const
        {
            // A variable starts its life as unbounded.
            Logger::log() << Logger::Debug << "Building variable '"<<name <<"' of type " << tname <<Logger::endl;
            return new Attribute<T>( name, new internal::UnboundDataSource<internal::ValueDataSource<T> >() );
        }

        base::AttributeBase* buildAttribute( std::string name, base::DataSourceBase::shared_ptr in) const
        {
            typename internal::AssignableDataSource<DataType>::shared_ptr ds;
            if ( !in )
                ds = new internal::ValueDataSource<DataType>();
            else
                ds = internal::AssignableDataSource<DataType>::narrow( in.get() );
            if (!ds)
                return 0;
            // An attribute is always bounded.
            Logger::log() << Logger::Debug << "Building Attribute '"<< name <<"' of type " << tname <<Logger::endl;
            return new Attribute<DataType>( name, ds.get() );
        }

        base::AttributeBase* buildAlias(std::string name, base::DataSourceBase::shared_ptr in ) const
        {
            typename internal::DataSource<T>::shared_ptr ds = boost::dynamic_pointer_cast< internal::DataSource<T> >( internal::DataSourceTypeInfo<T>::getTypeInfo()->convert(in) );
            if ( ! ds )
                return 0;
            return new Alias( name, ds );
        }

        base::DataSourceBase::shared_ptr buildActionAlias(base::ActionInterface* action, base::DataSourceBase::shared_ptr in) const
        {
            typename internal::AssignableDataSource<T>::shared_ptr ads = boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >( in ); // no type conversion is done.
            if ( ads )
                return new internal::ActionAliasAssignableDataSource<T>(action, ads.get());

            typename internal::DataSource<T>::shared_ptr ds = boost::dynamic_pointer_cast< internal::DataSource<T> >( in ); // no type conversion is done.
            if ( ! ds )
                return 0;
            return new internal::ActionAliasDataSource<T>(action, ds.get());
        }

        virtual const std::string& getTypeName() const { return tname; }

        virtual base::PropertyBase* buildProperty(const std::string& name, const std::string& desc, base::DataSourceBase::shared_ptr source = 0) const {
            if (source) {
               typename internal::AssignableDataSource<DataType>::shared_ptr ad
                    = boost::dynamic_pointer_cast< internal::AssignableDataSource<DataType> >( source );
                if (ad)
                    return new Property<DataType>(name, desc, ad );
                else {
                    log(Error) <<"Failed to build 'Property<"<< this->tname <<"> "<<name<<"' from given DataSourceBase. Returning default."<<endlog();
                }
            }
            return new Property<DataType>(name, desc, DataType());
        }

        virtual base::DataSourceBase::shared_ptr buildValue() const {
            return new internal::ValueDataSource<DataType>();
        }
        virtual base::DataSourceBase::shared_ptr buildReference(void* ptr) const {
            return new internal::ReferenceDataSource<DataType>(*static_cast<DataType*>(ptr));
        }

        virtual std::ostream& write( std::ostream& os, base::DataSourceBase::shared_ptr in ) const {
            typename internal::DataSource<T>::shared_ptr d = boost::dynamic_pointer_cast< internal::DataSource<T> >( in );
            if ( d && use_ostream )
                types::TypeStreamSelector<T, use_ostream>::write( os, d->rvalue() );
            else {
#ifdef OS_HAVE_STREAMS
                std::string output = std::string("(")+ in->getTypeName() +")";
                os << output;
#endif
            }
            return os;
            //return os << "("<< tname <<")"
        }

        virtual std::istream& read( std::istream& os, base::DataSourceBase::shared_ptr out ) const {
            typename internal::AssignableDataSource<T>::shared_ptr d = boost::dynamic_pointer_cast< internal::AssignableDataSource<T> >( out );
            if ( d && use_ostream ) {
                types::TypeStreamSelector<T, use_ostream>::read( os, d->set() );
                d->updated(); // because use of set().
            }
            return os;
        }

        virtual bool isStreamable() const {
            return use_ostream;
        }

        virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr result) const {
            return false;
        }

        /**
         * A primitive type is decomposed into itself.
         */
        virtual base::DataSourceBase::shared_ptr decomposeType(base::DataSourceBase::shared_ptr source) const
        {
            return source;
        }

        virtual bool decomposeType( base::DataSourceBase::shared_ptr source, PropertyBag& targetbag ) const {
            return false;
        }

		std::string getTypeIdName() const { return typeid(T).name(); }


        base::InputPortInterface*  inputPort(std::string const& name) const { return 0; }
        base::OutputPortInterface* outputPort(std::string const& name) const { return 0; }

        base::ChannelElementBase::shared_ptr buildDataStorage(ConnPolicy const& policy) const {
            return base::ChannelElementBase::shared_ptr();
        }

        base::ChannelElementBase::shared_ptr buildChannelOutput(base::InputPortInterface& port) const
        {
            return base::ChannelElementBase::shared_ptr();
        }

        base::ChannelElementBase::shared_ptr buildChannelInput(base::OutputPortInterface& port) const
        {
            return base::ChannelElementBase::shared_ptr();
        }
    };
}}

#endif
