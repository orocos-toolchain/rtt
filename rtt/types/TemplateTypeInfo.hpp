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
#include "TemplateConnFactory.hpp"
#include "TemplateCompositionFactory.hpp"

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
        : public PrimitiveTypeInfo<T, use_ostream>,
          public TemplateConnFactory<T>,
          public TemplateCompositionFactory<T>
    {
    public:
        /**
         * The given \a T parameter is the type for reading DataSources.
         */
        typedef T UserType;

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


        virtual base::ChannelElementBase::shared_ptr buildRemoteChannelOutput(
                base::OutputPortInterface& output_port,
                types::TypeInfo const* type_info,
                base::InputPortInterface& input, const ConnPolicy& policy) {
            return base::ChannelElementBase::shared_ptr();
        }

        bool installTypeInfoObject(TypeInfo* ti) {
            // aquire a shared reference to the this object
            boost::shared_ptr< TemplateTypeInfo<T,use_ostream> > mthis = boost::dynamic_pointer_cast<TemplateTypeInfo<T,use_ostream> >( this->getSharedPtr() );
            assert(mthis);
            // Allow base to install first
            PrimitiveTypeInfo<T,use_ostream>::installTypeInfoObject(ti);
            // Install the factories for primitive types
            ti->setPortFactory( mthis );
            ti->setCompositionFactory( mthis );

            // Don't delete us, we're memory-managed.
            return false;
        }
    };
}}

#endif
