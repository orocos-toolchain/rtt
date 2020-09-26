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
#include "rtt/types/carray.hpp"
#include "rtt/rtt-config.h"
#include "rtt/types/rtt-types-fwd.hpp"

namespace RTT
{

    namespace internal {

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

            static types::TypeInfo* TypeInfoObject;

            static const std::string noqual;
            static const std::string cqual;
            static const std::string refqual;
            static const std::string crefqual;
            static const std::string ptrqual;
            static const std::string cptrqual;
            static const std::string& getType();
            static const std::string& getTypeName();
            static const std::string& getQualifier();
            // we drop the const qualifier in this specialisation, since it is
            // not registered in the type repository (which returns a non const in type() )
            static types::TypeInfo* getTypeInfo();
        };

        /**
         * These specialisations only serve to find out
         * if a type T is a pointer, or reference, or...
         */
        template< class T>
        struct DataSourceTypeInfo<const T&> {
            typedef T value_type;
            typedef DataSourceTypeInfo<T> value_type_info;
            static std::string getType()  { return getTypeName() + getQualifier(); }
            static const std::string& getTypeName()  { return DataSourceTypeInfo< T >::getTypeName(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<UnknownType>::crefqual; }
            static const types::TypeInfo* getTypeInfo() { return DataSourceTypeInfo< T >::getTypeInfo(); }
        };

        template< class T>
        struct DataSourceTypeInfo<T&> {
            typedef T value_type;
            typedef DataSourceTypeInfo<T> value_type_info;
            static std::string getType()  { return getTypeName() + getQualifier(); }
            static const std::string& getTypeName()  { return DataSourceTypeInfo< T >::getTypeName(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<UnknownType>::refqual; }
            static const types::TypeInfo* getTypeInfo() { return DataSourceTypeInfo< T >::getTypeInfo(); }
        };

        template< class T>
        struct DataSourceTypeInfo<const T> {
            typedef T value_type;
            typedef DataSourceTypeInfo<T> value_type_info;
            static std::string getType()  { return getTypeName() + getQualifier(); }
            static const std::string& getTypeName()  { return DataSourceTypeInfo< T >::getTypeName(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<UnknownType>::cqual; }
            static const types::TypeInfo* getTypeInfo() { return DataSourceTypeInfo< T >::getTypeInfo(); }
        };

        template< class T>
        struct DataSourceTypeInfo<T*> {
            typedef T value_type;
            typedef DataSourceTypeInfo<T> value_type_info;
            static std::string getType()  { return getTypeName() + getQualifier(); }
            static const std::string& getTypeName()  { return DataSourceTypeInfo< T >::getTypeName(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<UnknownType>::ptrqual; }
            static const types::TypeInfo* getTypeInfo();
            static types::TypeInfo* TypeInfoObject;
        };

        template< class T>
        struct DataSourceTypeInfo<const T*> {
            typedef T value_type;
            typedef DataSourceTypeInfo<T> value_type_info;
            static std::string getType()  { return getTypeName() + getQualifier(); }
            static const std::string& getTypeName()  { return DataSourceTypeInfo< T >::getTypeName(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<UnknownType>::cptrqual; }
            static const types::TypeInfo* getTypeInfo() { return DataSourceTypeInfo< T >::getTypeInfo(); }
        };

        /**
         * Specialisation for a types::carray<T> type info object.
         * All RTT internal primitives should use carray references
         * to (boost) arrays such that run-time size checks can be done.
         * For example, Property<carray<double> > holds a
         * ValueDataSource<carray<double> >( carray<double>( data.c_array(), 6 ) )
         * where data is a boost::array<double,6> or equivalent
         */
        template< class T>
        struct DataSourceTypeInfo<types::carray<T> > {
            typedef types::carray<T> value_type;
            typedef DataSourceTypeInfo<value_type> value_type_info;
            static std::string getType()  { return getTypeName() + "[N]"; }
            static const std::string& getTypeName()  { return DataSourceTypeInfo< T >::getTypeInfo()->getTypeName(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<UnknownType>::noqual; }
            static const types::TypeInfo* getTypeInfo();
            static types::TypeInfo* TypeInfoObject;
        };

        /**
         * Every DataSource of type T has a type info class which
         * it can ask type information. The default is 'UnknownType', but
         * by adding a types::TemplateTypeInfo specialisation in the TypeRepository,
         * the type will get known to Orocos.
         */
        template< class T>
        struct DataSourceTypeInfo {
            typedef T value_type;
            typedef DataSourceTypeInfo<T> value_type_info;

            /**
             * Defaults to 0. If a types::TemplateTypeInfo object is created for T,
             * this pointer will be set to that type.
             */
            static types::TypeInfo* TypeInfoObject;

            /**
             * Return the qualified type.
             */
            static const std::string& getType();

            /**
             * Return the unqualified typename.
             */
            static const std::string& getTypeName();

            /**
             * Return the 'const', 'ref', 'pointer' qualifier for this type.
             */
            static const std::string& getQualifier();

            /**
             * Return the typeinfo object.
             */
            static const types::TypeInfo* getTypeInfo();
        };

        /**
         * (void) is a special case.
         */
        template<>
        struct RTT_API DataSourceTypeInfo<void> {
            static const std::string tname;
        public:
            typedef void value_type;
            typedef DataSourceTypeInfo<void> value_type_info;

            /**
             * Defaults to 0. If a types::TemplateTypeInfo object is created for T,
             * this pointer will be set to that type.
             */
            static types::TypeInfo* TypeInfoObject;

            /**
             * Return the qualified type.
             */
            static const std::string& getType();

            /**
             * Return the unqualified typename.
             */
            static const std::string& getTypeName();

            /**
             * Return the 'const', 'ref', 'pointer' qualifier for this type.
             */
            static const std::string& getQualifier();

            /**
             * Return the typeinfo object.
             */
            static const types::TypeInfo* getTypeInfo();
        };
    }

}

#endif

#include "rtt/internal/DataSourceTypeInfo.inl"
