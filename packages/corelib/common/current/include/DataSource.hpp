/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  DataSource.hpp

                        DataSource.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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

#ifndef CORELIB_DATASOURCE_HPP
#define CORELIB_DATASOURCE_HPP

#include <map>
#include <string>
#include <boost/call_traits.hpp>
#include <boost/type_traits.hpp>

#include "DataSourceTypeInfo.hpp"
#include "DataSourceBase.hpp"

namespace ORO_CoreLib
{

  /**
   * DataSource is a base class representing a generic way to read
   * data of type \a T.
   *
   * A VariableDataSource, which just keeps a value and returns it
   * every time in its get() method is implemented in ORO_Execution.
   *
   * @see DataSourceBase for shared_ptr use.
   * @param T The type of data returned by \a get(). It does not
   * necessarily say that the data is stored as a \a T, it only
   * specifies in which form the get() method returns the data.
   * Thus a DataSource<const std::string&> returns a const ref to
   * a string, but may store the string itself by value.
   */
  template<typename T>
  class DataSource
    : public DataSourceBase
  {
  protected:
      virtual ~DataSource();
  public:
      /**
       * The bare type of T is extracted into value_t.
       */
      typedef typename boost::remove_const<typename boost::remove_reference<T>::type>::type value_t;
      typedef T result_t;
      
      typedef typename boost::intrusive_ptr<DataSource<T> > shared_ptr;

      /**
       * Return the data as type \a T.
       */
      virtual result_t get() const = 0;

      virtual void evaluate() const { this->get(); }

      virtual DataSource<T>* clone() const = 0;

      virtual DataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) = 0;

      virtual std::string getType() const;

      /**
       * Return usefull type info in a human readable format.
       */
      static  std::string GetType();
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
  std::string DataSource<T>::GetType()
  {
      return detail::DataSourceTypeInfo< T >::getQualifier()
          + detail::DataSourceTypeInfo< T >::getType();
  }

  /**
   * A DataSource which has set() methods.
   * @param T See DataSource for the semantics of \a T.
   */
  template<typename T>
  class AssignableDataSource
    : public DataSource<T>
  {
  protected:
      ~AssignableDataSource();
  public:
      typedef typename DataSource<T>::value_t value_t;
      typedef typename boost::call_traits<value_t>::param_type param_t;
      typedef typename boost::call_traits<value_t>::reference reference_t;
      typedef typename boost::call_traits<value_t>::const_reference const_reference_t;
      /**
       * Use this type to store a pointer to an AssignableDataSource.
       */
      typedef boost::intrusive_ptr<AssignableDataSource<T> > shared_ptr;

      /**
       * Set this DataSource with a value.
       */
      virtual void set( param_t t ) = 0;

      /**
       * Get a reference (or null) to the value of this DataSource.
       * Getting a reference to an  internal data structure is not thread-safe.
       * DataSources which wish to protect their data from concurrent access may
       * return the null reference with this method. All calls to set() must first
       * check whether they do not return null.
       */
      virtual reference_t set() = 0;

      virtual AssignableDataSource<T>* clone() const = 0;

      virtual AssignableDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) = 0;
  };

  template<typename T>
  AssignableDataSource<T>::~AssignableDataSource()
  {}


}

#endif
