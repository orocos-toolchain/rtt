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

#include <boost/type_traits.hpp>
#include <boost/call_traits.hpp>

#include <map>
#include <vector>
#include <string>
#include "DataSourceTypeInfo.hpp"
#include "DataSourceBase.hpp"

namespace ORO_CoreLib
{

  /**
   * DataSource is a base class representing a generic way to get a
   * piece of data from somewhere.  A standard VariableDataSource,
   * which just keeps a value and returns it every time in its get()
   * method is included below ( @ref VariableDataSource ).
   *
   * NOTES/TODO: Condition is remarkably similar to DataSource<bool>,
   * and perhaps it would be useful to somehow merge the two..  Or
   * perhaps just providing adaptors in two directions will do ?
   * Currently, the second approach is taken, with adapters in
   * ConditionBoolDataSource.hpp and
   * DataSourceCondition.hpp.
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
       * Return the data.
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
   */
  template<typename T>
  class AssignableDataSource
    : public DataSource<T>
  {
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

#if 0
    namespace detail
    {
        struct NoDS {};
        template<class T>
        struct isNoDS { enum res { False = 0 }; typedef False Result; }
        template<>
        struct isNoDS<NoDS> { enum res { True = 1}; typedef True Result; }
    }

    /**
     * A function which returns a vector of DataSources.
     */
    template<class A1, class A2 = detail::NoDS, class A3=detail::NoDS>
    std::vector<DataSourceBase*> GenerateDataSource(A1 a1, A2 a2 = A2(), A3 a3 = A3())
    {
        std::vector<DataSourceBase*> res;
        res.push_back( new DataSource<A1>( a1 ));
        if ( !isNoDS<A2>::Result )
            res.push_back( new DataSource<A2>(a2));
        if ( !isNoDS<A3>::Result )
            res.push_back( new DataSource<A3>(a3));
        return res;
    }
    
#endif
}

#endif
