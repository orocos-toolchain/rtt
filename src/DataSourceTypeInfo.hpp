/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  DataSourceTypeInfo.hpp

                        DataSourceTypeInfo.hpp -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
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


#ifndef DATASOURCE_TYPE_INFO_HPP
#define DATASOURCE_TYPE_INFO_HPP

#include <string>
#include <typeinfo>
#include "rtt-config.h"

namespace RTT
{
    class TypeInfo;

    namespace detail {

        struct RTT_API UnknownType {};

        template< class T>
        struct DataSourceTypeInfo;

        /**
         * This class offers the default implementation
         * of type information for an unknown type.
         * @see DataSource.cpp
         */
        template<>
        struct RTT_API DataSourceTypeInfo<UnknownType> {
            typedef UnknownType value_type;
            typedef DataSourceTypeInfo<UnknownType> value_type_info;

            static TypeInfo* TypeInfoObject;

            static const std::string noqual;
            static const std::string cqual;
            static const std::string refqual;
            static const std::string crefqual;
            static const std::string ptrqual;
            static const std::string cptrqual;
            static const std::string& getType();
            static const std::string& getQualifier();
            // we drop the const qualifier in this specialisation, since it is
            // not registered in the type repository (which returns a non const in type() )
            static TypeInfo* getTypeInfo();
        };

        /**
         * These specialisations only serve to find out
         * if a type T is a pointer, or reference, or...
         */
        template< class T>
        struct RTT_EXPORT DataSourceTypeInfo<const T&> {
            typedef T value_type;
            typedef DataSourceTypeInfo<T> value_type_info;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<UnknownType>::crefqual; }
            static const TypeInfo* getTypeInfo() { return DataSourceTypeInfo< T >::getTypeInfo(); }
        };

        template< class T>
        struct RTT_EXPORT DataSourceTypeInfo<T&> {
            typedef T value_type;
            typedef DataSourceTypeInfo<T> value_type_info;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<UnknownType>::refqual; }
            static const TypeInfo* getTypeInfo() { return DataSourceTypeInfo< T >::getTypeInfo(); }
        };

        template< class T>
        struct RTT_EXPORT DataSourceTypeInfo<const T> {
            typedef T value_type;
            typedef DataSourceTypeInfo<T> value_type_info;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<UnknownType>::cqual; }
            static const TypeInfo* getTypeInfo() { return DataSourceTypeInfo< T >::getTypeInfo(); }
        };

        template< class T>
        struct RTT_EXPORT DataSourceTypeInfo<T*> {
            typedef T value_type;
            typedef DataSourceTypeInfo<T> value_type_info;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<UnknownType>::ptrqual; }
            static const TypeInfo* getTypeInfo() { return DataSourceTypeInfo< T >::getTypeInfo(); }
        };

        template< class T>
        struct RTT_EXPORT DataSourceTypeInfo<const T*> {
            typedef T value_type;
            typedef DataSourceTypeInfo<T> value_type_info;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<UnknownType>::cptrqual; }
            static const TypeInfo* getTypeInfo() { return DataSourceTypeInfo< T >::getTypeInfo(); }
        };

        /**
         * Every DataSource of type T has a type info class which
         * it can ask type information. The default is 'UnknownType', but
         * by adding a TemplateTypeInfo specialisation in the TypeRepository,
         * the type will get known to Orocos.
         */
        template< class T>
        struct RTT_EXPORT DataSourceTypeInfo {
            typedef T value_type;
            typedef DataSourceTypeInfo<T> value_type_info;

            /**
             * Defaults to 0. If a TemplateTypeInfo object is created for T,
             * this pointer will be set to that type.
             */
            static TypeInfo* TypeInfoObject;

            /**
             * Return the unqualified typename.
             */
            static const std::string& getType();

            /**
             * Return the 'const', 'ref', 'pointer' qualifier for this type.
             */
            static const std::string& getQualifier();

            /**
             * Return the typeinfo object.
             */
            static const TypeInfo* getTypeInfo();
        };

        /**
         * (void) is a special case.
         */
        template<>
        class RTT_API DataSourceTypeInfo<void> {
            static const std::string tname;
        public:
            typedef void value_type;
            typedef DataSourceTypeInfo<void> value_type_info;

            /**
             * Defaults to 0. If a TemplateTypeInfo object is created for T,
             * this pointer will be set to that type.
             */
            static TypeInfo* TypeInfoObject;

            /**
             * Return the unqualified typename.
             */
            static const std::string& getType();

            /**
             * Return the 'const', 'ref', 'pointer' qualifier for this type.
             */
            static const std::string& getQualifier();

            /**
             * Return the typeinfo object.
             */
            static const TypeInfo* getTypeInfo();
        };
    }

}

#endif

#include "DataSourceTypeInfo.inl"
