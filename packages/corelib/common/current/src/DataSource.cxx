/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  DataSource.cxx 

                        DataSource.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
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
#include "corelib/DataSource.hpp"


namespace ORO_CoreLib
{
    DataSourceBase::~DataSourceBase()
    {
    }
    
    void DataSourceBase::reset()
    {
    }

    namespace detail {
        const std::string DataSourceTypeInfo<ValueType>::type("unknown type");
        const std::string DataSourceTypeInfo<ValueType>::qual("");
        const std::string DataSourceTypeInfo<void>::type("void");
        const std::string DataSourceTypeInfo<bool>::type("bool");
        const std::string DataSourceTypeInfo<int>::type("int");
        const std::string DataSourceTypeInfo<unsigned int>::type("unsigned int");
        const std::string DataSourceTypeInfo<double>::type("double");
        const std::string DataSourceTypeInfo<float>::type("float");
        const std::string DataSourceTypeInfo<char>::type("char");
        const std::string DataSourceTypeInfo<PropertyBag>::type("PropertyBag");
        const std::string DataSourceTypeInfo<ORO_CoreLib::MultiVector<6,double> >::type("Double6D");
        const std::string DataSourceTypeInfo<std::string>::type("std::string");
        const std::string DataSourceTypeInfo<std::vector<double> >::type("std::vector<double> (array)");

        // Forward decls allow us to define these, even if the geometry package is not used.
        const std::string DataSourceTypeInfo<ORO_Geometry::Frame>::type("Frame");
        const std::string DataSourceTypeInfo<ORO_Geometry::Vector>::type("Vector");
        const std::string DataSourceTypeInfo<ORO_Geometry::Rotation>::type("Rotation");
        const std::string DataSourceTypeInfo<ORO_Geometry::Twist>::type("Twist");
        const std::string DataSourceTypeInfo<ORO_Geometry::Wrench>::type("Wrench");
    }
}

void intrusive_ptr_add_ref( ORO_CoreLib::DataSourceBase* p )
{
  p->ref();
};

void intrusive_ptr_release( ORO_CoreLib::DataSourceBase* p )
{
  p->deref();
};
