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
#include "PrimitiveTypeInfo.hpp"

#include "../rtt-config.h"

namespace RTT
{
    namespace types {

    /**
     * This template class allows user types to be used in all Orocos primitives.
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
        : public PrimitiveTypeInfo<T, use_ostream>
    {
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
            : PrimitiveTypeInfo<T,use_ostream>(name)
        {
        }

        virtual ~TemplateTypeInfo()
        {
        }

        virtual bool composeType( base::DataSourceBase::shared_ptr source, base::DataSourceBase::shared_ptr result) const {
            const internal::DataSource<PropertyBag>* pb = dynamic_cast< const internal::DataSource<PropertyBag>* > (source.get() );
            if ( !pb )
                return false;
            typename internal::AssignableDataSource<PropertyType>::shared_ptr ads = boost::dynamic_pointer_cast< internal::AssignableDataSource<PropertyType> >( result );
            if ( !ads )
                return false;

            // last fall-back: use user supplied function:
            if ( composeTypeImpl( pb->rvalue(), ads->set() ) )
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

        /**
         * User, implement this function in case you want to control reading the XML data format.
         */
        virtual bool composeTypeImpl(const PropertyBag& source,  typename internal::AssignableDataSource<T>::reference_t result) const {
            return false;
        }

        /**
         * User, implement this function in case you want to control writing the XML data format.
         * Add the structural elements of source to targetbag.
         */
        virtual bool decomposeTypeImpl( typename internal::AssignableDataSource<T>::const_reference_t source, PropertyBag& targetbag ) const {
            return false;
        }

        base::InputPortInterface*  inputPort(std::string const& name) const { return new InputPort<T>(name); }
        base::OutputPortInterface* outputPort(std::string const& name) const { return new OutputPort<T>(name); }

        base::ChannelElementBase::shared_ptr buildDataStorage(ConnPolicy const& policy) const {
            return internal::ConnFactory::buildDataStorage<T>(policy);
        }

        base::ChannelElementBase::shared_ptr buildChannelOutput(base::InputPortInterface& port) const
        {
            return internal::ConnFactory::buildChannelOutput(
                    static_cast<RTT::InputPort<T>&>(port), new internal::SimpleConnID());
        }

        base::ChannelElementBase::shared_ptr buildChannelInput(base::OutputPortInterface& port) const
        {
            return internal::ConnFactory::buildChannelInput(
                    static_cast<RTT::OutputPort<T>&>(port), new internal::SimpleConnID(), 0 );
        }
    };
}}

#endif
