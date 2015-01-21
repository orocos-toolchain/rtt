#ifndef ORO_PRIMITIVE_TYPEINFO_HPP
#define ORO_PRIMITIVE_TYPEINFO_HPP

#include "Types.hpp"
#include "../Property.hpp"
#include "../Attribute.hpp"
#include "../Logger.hpp"
#include "TypeStreamSelector.hpp"
#include "TypeInfoGenerator.hpp"
#include "StreamFactory.hpp"
#include "TemplateValueFactory.hpp"
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
    class PrimitiveTypeInfo :
            public TypeInfoGenerator,
            public TemplateValueFactory<T>,
            public StreamFactory
    {
    protected:
        const std::string tname;
        boost::shared_ptr<PrimitiveTypeInfo<T, use_ostream> > mshared;
    public:
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
        }

        boost::shared_ptr<PrimitiveTypeInfo<T, use_ostream> > getSharedPtr() {
            if (!mshared)
                mshared.reset(this);
            return mshared;
        }

        bool installTypeInfoObject(TypeInfo* ti) {
            // Install the factories for primitive types
            ti->setValueFactory( this->getSharedPtr() );
	    if (use_ostream)
	      ti->setStreamFactory( this->getSharedPtr() );

            // Install the type info object for T
            internal::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject = ti;
            ti->setTypeId( &typeid(T) );

            // Clean up reference to ourselves.
            mshared.reset();
            // Don't delete us, we're memory-managed.
            return false;
        }

        TypeInfo* getTypeInfoObject() const {
            return TypeInfoRepository::Instance()->getTypeInfo<T>();
        }

        virtual const std::string& getTypeName() const { return tname; }

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
    };
}}

#endif
