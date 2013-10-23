/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  DataSource.hpp

                        DataSource.hpp -  description
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

#ifndef RTT_INTERNAL_DATASOURCE_HPP
#define RTT_INTERNAL_DATASOURCE_HPP

#include <map>
#include <boost/static_assert.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/add_const.hpp>

#include "../base/DataSourceBase.hpp"
#include "DataSourceTypeInfo.hpp"

namespace RTT
{ namespace internal {

  namespace details
  {
     template<typename X>
     struct HasConst
     { static const int value = 0; };
     template<typename X>
     struct HasConst<X const>
     { static const int value = 1; };

     template<typename X>
     struct GetConstRef
     { typedef typename boost::add_reference<typename boost::add_const<X>::type>::type type; };
     template<>
     struct GetConstRef<void>
     { typedef void type; };
  }

  /**
   * DataSource is a base class representing a generic way to read
   * data of type \a T.
   *
   * @see base::DataSourceBase for shared_ptr use.
   * @param T The type of data returned.
   */
  template<typename T>
  class DataSource
    : public base::DataSourceBase
  {
  protected:
      virtual ~DataSource();

  public:

      /**
       * The bare type of T is extracted into value_t.
       */
      typedef T value_t;
      typedef T result_t;
      typedef typename details::GetConstRef<T>::type const_reference_t;

      /**
       * If you get a compile error here, it means T has const or reference
       * qualifiers, which is not allowed for DataSource<T>
       */
      BOOST_STATIC_ASSERT( !details::HasConst<T>::value );

      typedef typename boost::intrusive_ptr<DataSource<T> > shared_ptr;

      typedef typename boost::intrusive_ptr<const DataSource<T> > const_ptr;

      /**
       * Return the data as type \a T.
       */
      virtual result_t get() const = 0;

      /**
       * Return the result of the last \a evaluate() function.
       * You must call evaluate() prior to calling this function in order to get
       * the most recent value of this attribute.
       */
      virtual result_t value() const = 0;

      /**
       * Get a const reference to the value of this DataSource.
       * You must call evaluate() prior to calling this function in order to get
       * the most recent value of this attribute.
       * @note Getting a reference to an internal data structure is not thread-safe.
       */
      virtual const_reference_t rvalue() const = 0;

      /** Generic implementation of getRawConstPointer() based on rvalue()
       */
      void const* getRawConstPointer() { return &rvalue(); }

      virtual bool evaluate() const;

      virtual DataSource<T>* clone() const = 0;

      virtual DataSource<T>* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const = 0;

      virtual std::string getType() const;

      /**
       * Return usefull type info in a human readable format.
       */
      static  std::string GetType();

      virtual const types::TypeInfo* getTypeInfo() const;

      /**
       * Return the Orocos type info.
       */
      static const types::TypeInfo* GetTypeInfo();

      /**
       * Return the Orocos type name, without const, pointer or reference
       * qualifiers.
       */
      static  std::string GetTypeName();

      virtual std::string getTypeName() const;

      /**
       * This method narrows a base::DataSourceBase to a typeded DataSource,
       * possibly returning a new object.
       */
      static DataSource<T>* narrow(base::DataSourceBase* db);

  };

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

}}

#endif

