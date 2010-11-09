/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  Types.hpp

                        Types.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef ORO_TEMPLATE_TYPEINFO_HPP
#define ORO_TEMPLATE_TYPEINFO_HPP

#include "Types.hpp"
#include "../Property.hpp"
#include "../Attribute.hpp"
#include "../Logger.hpp"
#include "../InputPort.hpp"
#include "../OutputPort.hpp"
#include <ostream>
#include "../internal/FusedFunctorDataSource.hpp"
#include "../internal/CreateSequence.hpp"
#include "PropertyDecomposition.hpp"

#include <boost/type_traits/function_traits.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include "../rtt-config.h"

namespace RTT
{
    namespace types {
        template<typename T, bool b_value>
        struct TypeStreamSelector;
        template<typename T>
        struct TypeStreamSelector<T,true>
        {
            static std::ostream& write(std::ostream& os, T t)
            {
#ifdef OS_HAVE_STREAMS
                os << t;
#endif
                return os;
            }
            static std::istream& read(std::istream& os, T& t)
            {
#ifdef OS_HAVE_STREAMS
                os >> t;
#endif
                return os;
            }
        };
        template<typename T>
        struct TypeStreamSelector<T,false>
        {
            static std::ostream& write(std::ostream& os, T)
            {
                return os;
            }
            static std::istream& read(std::istream& os, T& )
            {
                return os;
            }
        };

    /**
     * This template class allows user types to be added to Orocos.
     * It provides 'default' implementations for virtual functions of TypeInfo.
     * For user defined types, this is very likely not satisfactory and
     * the user needs to override the methods of this class in a subclass
     * or provide specialised global functions.
     *
     * @param T The user class type.
     * @param use_ostream When set to \a true, the class will use operator<<(std::ostream&, T)
     * to write out the type to a stream. When set to \a false, the class will use this function
     * and write '( \a type \a name )' to a stream instead. Defaults to \a false. Set to \a true
     * if your class \a T has the above mentioned function.
     * @see TypeInfoRepository.
     * @see operator<<
     * @see StructTypeInfo, SequenceTypeInfo
     */
    template<typename T, bool use_ostream = false>
    class TemplateTypeInfo
        : public TypeInfo
    {
        const std::string tname;
    public:
        using TypeInfo::buildConstant;
        using TypeInfo::buildVariable;

        /**
         * The given \a T parameter is the type for reading DataSources.
         */
        typedef T UserType;
        /**
         * When Properties of \a T are constructed, they are non-const, non-reference.
         */
        typedef typename Property<T>::DataSourceType PropertyType;

        /**
         * Setup Type Information for type \a name.
         * This causes a switch from 'unknown' type to basic
         * type information for type T.
         * @param name the 'Orocos' type name.
         *
         */
        TemplateTypeInfo(std::string name)
            : tname(name)
        {
            // Install the type info object for T.
            if ( internal::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject != 0) {
                Logger::log() << Logger::Warning << "Overriding TypeInfo for '"
                              << internal::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject->getTypeName()
                              << "'." << Logger::endl;
            }
            internal::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject = this;

        }

        base::AttributeBase* buildConstant(std::string name, base::DataSourceBase::shared_ptr dsb) const
        {
            typename internal::DataSource<PropertyType>::shared_ptr res =
                boost::dynamic_pointer_cast< internal::DataSource<PropertyType> >( internal::DataSourceTypeInfo<PropertyType>::getTypeInfo()->convert(dsb));
            if ( res ) {
                res->get();
                Logger::log() << Logger::Info << "Building "<<tname<<" Constant '"<<name<<"' with value "<< dsb->getTypeInfo()->toString(dsb) <<Logger::endl;
                return new Constant<PropertyType>( name, res->value() );
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
            typename internal::AssignableDataSource<PropertyType>::shared_ptr ds;
            if ( !in )
                ds = new internal::ValueDataSource<PropertyType>();
            else
                ds = internal::AssignableDataSource<PropertyType>::narrow( in.get() );
            if (!ds)
                return 0;
            // A variable starts its life as unbounded.
            Logger::log() << Logger::Debug << "Building Attribute '"<< name <<"' of type " << tname <<Logger::endl;
            return new Attribute<PropertyType>( name, ds.get() );
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
               typename internal::AssignableDataSource<PropertyType>::shared_ptr ad
                    = boost::dynamic_pointer_cast< internal::AssignableDataSource<PropertyType> >( source );
                if (ad)
                    return new Property<PropertyType>(name, desc, ad );
                else {
                    log(Error) <<"Failed to build 'Property<"<< this->tname <<"> "<<name<<"' from given DataSourceBase. Returning default."<<endlog();
                }
            }
            return new Property<PropertyType>(name, desc, PropertyType());
        }

        virtual base::DataSourceBase::shared_ptr buildValue() const {
            return new internal::ValueDataSource<PropertyType>();
        }
        virtual base::DataSourceBase::shared_ptr buildReference(void* ptr) const {
            return new internal::ReferenceDataSource<PropertyType>(*static_cast<PropertyType*>(ptr));
        }

        virtual std::ostream& write( std::ostream& os, base::DataSourceBase::shared_ptr in ) const {
            typename internal::DataSource<T>::shared_ptr d = boost::dynamic_pointer_cast< internal::DataSource<T> >( in );
            if ( d && use_ostream )
                detail::TypeStreamSelector<T, use_ostream>::write( os, d->value() );
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
                detail::TypeStreamSelector<T, use_ostream>::read( os, d->set() );
                d->updated(); // because use of set().
            }
            return os;
        }

        virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr result) const {
            // First, try a plain update.
            if ( result->update( source.get() ) )
                return true;

            // Did not work, maybe the type needs to be composed.
            const internal::DataSource<PropertyBag>* pb = dynamic_cast< const internal::DataSource<PropertyBag>* > (source.get() );
            if ( !pb )
                return false;
            typename internal::AssignableDataSource<PropertyType>::shared_ptr ads = boost::dynamic_pointer_cast< internal::AssignableDataSource<PropertyType> >( result );
            if ( !ads )
                return false;

            // last fall-back: use user supplied function:
            if ( composeTypeImpl( pb->value(), ads->set() ) )
                ads->updated();
            else {
                Logger::log() <<Logger::Debug<<"Failed to compose from "<< source->getTypeName() <<Logger::endl;
                return false;
            }
            Logger::log() <<Logger::Debug<<"Successfuly composed type from "<< source->getTypeName() <<Logger::endl;
            return true;
        }

        /**
         * This default implementation sets up a PropertyBag which is passed
         * to decomposeTypeImpl(). It is advised to implement that function and
         * to leave this function as-is, unless you don't want to return a
         * PropertyBag, but another data type.
         */
        virtual base::DataSourceBase::shared_ptr decomposeType(base::DataSourceBase::shared_ptr source) const
        {
            // Extract typed values
            typename internal::DataSource<PropertyType>::shared_ptr ds = boost::dynamic_pointer_cast< internal::DataSource<PropertyType> >( source );
            if ( !ds )
                return base::DataSourceBase::shared_ptr(); // happens in the case of 'unknown type'
            Property<PropertyBag> targetbag_p("targetbag");
            if (decomposeTypeImpl( ds->rvalue(), targetbag_p.value() ))
                return targetbag_p.getDataSource();
            return base::DataSourceBase::shared_ptr();
        }

        virtual bool decomposeType( base::DataSourceBase::shared_ptr source, PropertyBag& targetbag ) const {
            // Extract typed values
            typename internal::DataSource<PropertyType>::shared_ptr ds = boost::dynamic_pointer_cast< internal::DataSource<PropertyType> >( source );
            if ( !ds )
                return false; // happens in the case of 'unknown type'
            // Try user's function.
            return decomposeTypeImpl( ds->rvalue(), targetbag );
        }

        /**
         * User, implement this function in case you want to control reading the XML data format.
         * TemplateTypeInfo provides a default, good for most types implementation in case getMember()
         * is implemented.
         */
        virtual bool composeTypeImpl(const PropertyBag& source,  typename internal::AssignableDataSource<T>::reference_t result) const {
            // The default implementation decomposes result and refreshes it with source.
            internal::ReferenceDataSource<T> rds(result);
            rds.ref(); // prevent dealloc.
            PropertyBag decomp;
            // only try refreshProperties if decomp's type is equal to source type.
            return typeDecomposition( &rds, decomp) && ( decomp.getType() == source.getType() ) && refreshProperties(decomp, source);
        }

        /**
         * User, implement this function in case you want to control writing the XML data format.
         * Add the structural elements of source to targetbag.
         */
        virtual bool decomposeTypeImpl( typename internal::AssignableDataSource<T>::const_reference_t source, PropertyBag& targetbag ) const {
            return false;
        }

		std::string getTypeIdName() const { return typeid(T).name(); }


        base::InputPortInterface*  inputPort(std::string const& name) const { return new InputPort<T>(name); }
        base::OutputPortInterface* outputPort(std::string const& name) const { return new OutputPort<T>(name); }

        base::ChannelElementBase* buildDataStorage(ConnPolicy const& policy) const {
            return internal::ConnFactory::buildDataStorage<T>(policy);
        }

        base::ChannelElementBase* buildChannelOutput(base::InputPortInterface& port) const
        {
            return internal::ConnFactory::buildChannelOutput(
                    static_cast<RTT::InputPort<T>&>(port), new internal::SimpleConnID());
        }

        base::ChannelElementBase* buildChannelInput(base::OutputPortInterface& port) const
        {
            return internal::ConnFactory::buildChannelInput(
                    static_cast<RTT::OutputPort<T>&>(port), new internal::SimpleConnID(), 0 );
        }
    };
}}

#endif
