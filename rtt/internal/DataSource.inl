/***************************************************************************
  tag: psoetens Thu May 4 14:08:47 2006 +0000 DataSource.inl

                        DataSource.inl -  description
                           -------------------
    begin                : Thu May 4 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter@thesourceworks.com

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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef CORELIB_DATASOURCE_INL
#define CORELIB_DATASOURCE_INL

#include "rtt/internal/AssignCommand.hpp"
#include "rtt/internal/DataSourceTypeInfo.hpp"
#include "rtt/types/TypeTransporter.hpp"

#include "rtt/rtt-config.h"

namespace RTT
{ namespace internal {


  template<typename T>
  DataSource<T>::~DataSource()
  {
  }

  template< typename T>
  std::string DataSource<T>::getType() const
  {
      return DataSource<T>::GetType();
  }

  template< typename T>
  std::string DataSource<T>::getTypeName() const
  {
      return DataSource<T>::GetTypeName();
  }

  template< typename T>
  std::string DataSource<T>::GetType()
  {
      return DataSourceTypeInfo< T >::getTypeName() + DataSourceTypeInfo< T >::getQualifier();

  }

  template< typename T>
  std::string DataSource<T>::GetTypeName()
  {
      return DataSourceTypeInfo< T >::getTypeName();

  }

  template< typename T>
  const types::TypeInfo* DataSource<T>::getTypeInfo() const { return GetTypeInfo(); }

  template< typename T>
  const types::TypeInfo* DataSource<T>::GetTypeInfo() { return DataSourceTypeInfo<T>::getTypeInfo(); }

  template< typename T>
  bool DataSource<T>::evaluate() const
  {
      this->get();
      return true;
  }

  template<>
  inline void const* DataSource<void>::getRawConstPointer() { return 0; }

    template<typename T>
    AssignableDataSource<T>::~AssignableDataSource()
    {}

    template<class T>
    DataSource<T>* DataSource<T>::narrow(base::DataSourceBase* dsb) {
        DataSource<T>* ret = dynamic_cast< DataSource<T>* >( dsb );
	return ret;
    }

    template<class T>
    AssignableDataSource<T>* AssignableDataSource<T>::narrow(base::DataSourceBase* dsb) {
        AssignableDataSource<T>* ret = dynamic_cast< AssignableDataSource<T>* >( dsb );
	return ret;
    }

    template<class T>
    bool AssignableDataSource<T>::update( base::DataSourceBase* other ) {
        if (!other) return false;

        base::DataSourceBase::shared_ptr r( other );
        typename DataSource<T>::shared_ptr o = boost::dynamic_pointer_cast< DataSource<T> >( DataSourceTypeInfo<T>::getTypeInfo()->convert(r) );
        if (o) {
            if ( o->evaluate() ) {
                this->set( o->value() );
                return true;
            }
            return false;
        }
        return false;
    }

    template<class T>
    base::ActionInterface* AssignableDataSource<T>::updateAction( base::DataSourceBase* other) {
#ifndef ORO_EMBEDDED
        if (!other) throw bad_assignment();
#else
        if (!other) return 0;
#endif

        // Use the same rules of parameter passing as C++, but no const for 'int',...
        base::DataSourceBase::shared_ptr r( other );
        typename DataSource<value_t>::shared_ptr t = boost::dynamic_pointer_cast< DataSource<value_t> >( DataSourceTypeInfo<T>::getTypeInfo()->convert(r) );
        if ( t )
            return new AssignCommand<value_t>( this, t );

#ifndef ORO_EMBEDDED
        throw bad_assignment();
#endif

        return 0;
    }
}}


#endif
