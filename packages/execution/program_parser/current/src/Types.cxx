/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  Types.cxx 

                        Types.cxx -  description
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
#include "execution/Types.hpp"

#include <pkgconf/system.h>
// Include geometry support
#ifdef OROPKG_GEOMETRY
#include <geometry/frames.h>
#endif

#include <corelib/MultiVector.hpp>
#include "execution/TaskVariable.hpp"
#include "execution/mystd.hpp"
#include <functional>

namespace ORO_Execution
{
#ifdef OROPKG_GEOMETRY
  using ORO_Geometry::Frame;
  using ORO_Geometry::Vector;
  using ORO_Geometry::Rotation;
  using ORO_Geometry::Wrench;
  using ORO_Geometry::Twist;
#endif
  using ORO_CoreLib::Double6D;


    TaskAttributeBase* TypeInfo::buildVariable( int ) const {
        return this->buildVariable();
    }

    TaskAttributeBase* TypeInfo::buildConstant( int ) const {
        return this->buildConstant();
    }

  template<typename T>
  class TemplateTypeInfo
    : public TypeInfo
  {
  public:
      using TypeInfo::buildConstant;
      using TypeInfo::buildVariable;

    TaskAttributeBase* buildConstant() const
      {
        return new ParsedConstant<T>();
      }
    TaskAttributeBase* buildVariable() const
      {
        return new ParsedVariable<T>();
      }
    TaskAttributeBase* buildAlias( DataSourceBase* b ) const
      {
        DataSource<T>* ds( dynamic_cast<DataSource<T>*>( b ) );
        if ( ! ds )
          return 0;
        return new detail::ParsedAlias<T>( ds );
      }
  };

    // Identical to above, but the variable is of the TaskIndexVariable type.
  template<typename T, typename IndexType, typename SetType, typename Pred>
  class TemplateIndexTypeInfo
    : public TypeInfo
  {
  public:
      TemplateIndexTypeInfo() {}

      using TypeInfo::buildConstant;
      using TypeInfo::buildVariable;

    TaskAttributeBase* buildConstant() const
      {
        return new ParsedConstant<T>();
      }

    TaskAttributeBase* buildVariable() const
      {
        return new ParsedIndexVariable<T, IndexType, SetType, Pred>();
      }

    TaskAttributeBase* buildAlias( DataSourceBase* b ) const
      {
        DataSource<T>* ds( dynamic_cast<DataSource<T>*>( b ) );
        if ( ! ds )
          return 0;
        return new detail::ParsedAlias<T>( ds );
      }
  };

    // Identical to above, but the variable is of the TaskIndexVariable type
    // and the T is a container, which has a constructor which takes an int.
  template<typename T, typename IndexType, typename SetType, typename Pred>
  class TemplateIndexContainerTypeInfo
      : public TemplateIndexTypeInfo<T,IndexType,SetType,Pred>
  {
      typedef typename mystd::remove_cr<T>::type _T;
  public:
      using TypeInfo::buildConstant;
      using TypeInfo::buildVariable;

    TaskAttributeBase* buildVariable(int size) const
      {
          // if a sizehint is given, creat a TaskIndexContainerVariable instead,
          // which checks capacities.
          _T t_init(size, SetType());
          //t_init.reserve( size );
          return new ParsedIndexContainerVariable<T, IndexType, SetType, Pred>( t_init );
      }
  };


  TypeInfo::~TypeInfo()
  {
  }

  TypeInfoRepository& TypeInfoRepository::instance()
  {
    static TypeInfoRepository s;
    return s;
  }

  TypeInfo* TypeInfoRepository::type( const std::string& name ) const
  {
    map_t::const_iterator i = data.find( name );
    if ( i == data.end() )
      return 0;
    else return i->second;
  }

  TypeInfoRepository::~TypeInfoRepository()
  {
  }

  // check the validity of an index
    template< class T>
    struct ArrayIndexChecker
        : public std::binary_function< T, int, bool>
    {
        bool operator()(const T& v, int i ) const
        {
            return i > -1 && i < (int)(v.size());
        }
    };

#if 0
  // check the validity of an index
    template< class T>
    struct MultiVectorIndexChecker
        : public std::binary_function< T, int, bool>
    {
        bool operator()(const T& v, int i ) const
        {
            return i > -1 && i < T::size;
        }
    };
#endif

  // check the validity of a fixed range index
    template< class T, int Range>
    struct RangeIndexChecker
        : public std::binary_function< T, int, bool>
    {
        bool operator()(const T& v, int i ) const
        {
            return i > -1 && i < Range;
        }
    };

  TypeInfoRepository::TypeInfoRepository()
  {
#ifdef OROPKG_GEOMETRY
    data["frame"] = new TemplateTypeInfo<Frame>();
    data["rotation"] = new TemplateTypeInfo<Rotation>();
    data["wrench"] = new TemplateIndexTypeInfo<Wrench,int, double, RangeIndexChecker<Wrench,6> >;
    data["twist"] = new TemplateIndexTypeInfo<Twist,int, double, RangeIndexChecker<Twist,6> >;
    data["vector"] = new TemplateIndexTypeInfo<Vector,int, double, RangeIndexChecker<Vector,3> >;
#endif
    data["int"] = new TemplateTypeInfo<int>();
    data["char"] = new TemplateTypeInfo<char>();
    data["string"] = new TemplateIndexContainerTypeInfo<const std::string&, int, char, ArrayIndexChecker<std::string> >;
    data["double"] = new TemplateTypeInfo<double>();
    data["bool"] = new TemplateTypeInfo<bool>();
    data["double6d"] = new TemplateIndexTypeInfo<Double6D,int, double, RangeIndexChecker<Double6D,6> >;
    data["array"] = new TemplateIndexContainerTypeInfo<const std::vector<double>&, int, double, ArrayIndexChecker<std::vector<double> > >;
  }
}
