/***************************************************************************
  tag: Peter Soetens  Sat May 7 12:56:52 CEST 2005  DataSourceTypeInfo.hpp 

                        DataSourceTypeInfo.hpp -  description
                           -------------------
    begin                : Sat May 07 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef DATASOURCE_TYPE_INFO_HPP
#define DATASOURCE_TYPE_INFO_HPP

#include <string>


namespace ORO_CoreLib
{
    template<unsigned int S, class T>
    class MultiVector;
}

namespace ORO_Geometry
{
    class Frame;
    class Vector;
    class Rotation;
    class Twist;
    class Wrench;
}


namespace ORO_CoreLib
{

    namespace detail {
        struct ValueType {};

        template< class T>
        struct DataSourceTypeInfo;

        template<>
        struct DataSourceTypeInfo<ValueType> {
            static const std::string type;
            static const std::string qual;
            static const std::string& getType() { return type; }
            static const std::string& getQualifier() { return qual; }
        };

        template< class T>
        struct DataSourceTypeInfo<const T&> {
            static const std::string qual;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return qual; }
        };

        template< class T >
        const std::string DataSourceTypeInfo<const T&>::qual("const& ");

        template< class T>
        struct DataSourceTypeInfo<T&> {
            static const std::string qual;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return qual; }
        };

        template< class T >
        const std::string DataSourceTypeInfo<T&>::qual("& ");

        template< class T>
        struct DataSourceTypeInfo<const T> {
            static const std::string qual;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return qual; }
        };

        template< class T >
        const std::string DataSourceTypeInfo<const T>::qual("const ");

        template< class T>
        struct DataSourceTypeInfo<T*> {
            static const std::string qual;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return qual; }
        };

        template< class T >
        const std::string DataSourceTypeInfo<T*>::qual("* ");

        template< class T>
        struct DataSourceTypeInfo<const T*> {
            static const std::string qual;
            static const std::string& getType()  { return DataSourceTypeInfo< T >::getType(); }
            static const std::string& getQualifier() { return qual; }
        };

        template< class T >
        const std::string DataSourceTypeInfo<const T*>::qual("const* ");

        template<>
        struct DataSourceTypeInfo<void> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<bool> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<int> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<unsigned int> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<double> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<char> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        // Forward decls allow us to define these, even if the geometry package is not used.
        template<>
        struct DataSourceTypeInfo<ORO_Geometry::Frame> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<ORO_Geometry::Vector> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<ORO_Geometry::Rotation> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<ORO_Geometry::Twist> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<ORO_Geometry::Wrench> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<std::string> {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo<ORO_CoreLib::MultiVector<6, double> > {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template<>
        struct DataSourceTypeInfo< std::vector<double> > {
            static const std::string type;
            static const std::string& getType()  { return type; }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

        template< class T>
        struct DataSourceTypeInfo {
            static const std::string& getType() { return DataSourceTypeInfo<ValueType>::getType(); }
            static const std::string& getQualifier() { return DataSourceTypeInfo<ValueType>::getQualifier(); }
        };

    }

}
#endif
