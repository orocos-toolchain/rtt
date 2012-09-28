/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  TypeInfoName.hpp

                        TypeInfoName.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_TYPEINFO_NAME_HPP
#define ORO_TYPEINFO_NAME_HPP

#include "Types.hpp"
#include "TypeInfoGenerator.hpp"
#include "../internal/DataSourceTypeInfo.hpp"
#include "../Logger.hpp"
#include "../rtt-config.h"

namespace RTT
{ namespace types {

    /**
     * Empty implementation of TypeInfo interface.
     */
    class RTT_API EmptyTypeInfo
        : public TypeInfoGenerator
    {
    protected:
        const std::string tname;
    public:
        EmptyTypeInfo(std::string name)
            : tname(name)
        {
        }

        bool installTypeInfoObject(TypeInfo* ti) {
            return true;
        }

        virtual const std::string& getTypeName() const { return tname; }
    };

    /**
     * This helper class allows only type names to be added to Orocos.
     * @warning OR use this class OR use TemplateTypeInfo to describe
     * your type, not both.
     * @see TemplateTypeInfo for adding full type info to Orocos.
     */
    template<typename T>
    struct TypeInfoName
        : public EmptyTypeInfo
    {
        /**
         * Setup Type Name Information for type \a name.
         * This causes a switch from 'unknown' type to basic
         * type information for type T.
         * @param name the 'Orocos' type name.
         *
         */
        TypeInfoName(std::string name)
            : EmptyTypeInfo(name)
        {
        }

        bool installTypeInfoObject(TypeInfo* ti ) {
            internal::DataSourceTypeInfo<T>::value_type_info::TypeInfoObject = ti;
            ti->setTypeId( &typeid(T) );
            return true;
        }
        TypeInfo* getTypeInfoObject() const {
            return TypeInfoRepository::Instance()->getTypeInfo<T>();
        }
    };

}}

#endif
