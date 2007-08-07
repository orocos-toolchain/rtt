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

#ifndef CORELIB_DATASOURCE_HPP
#define CORELIB_DATASOURCE_HPP

#include <map>
#include <string>
#include "rtt-config.h"
#include <boost/call_traits.hpp>
#include <boost/type_traits.hpp>

#include "DataSourceBase.hpp"

namespace RTT
{

#ifndef ORO_EMBEDDED
    /**
     * This exception is thrown if the target and source type
     * of an assignment of a DataSource with a DataSourceBase
     * differ.
     */
    struct RTT_EXPORT bad_assignment
    {
    };
#endif


  /**
   * DataSource is a base class representing a generic way to read
   * data of type \a T.
   *
   * @see DataSourceBase for shared_ptr use.
   * @param T The type of data returned by \a get(). It does not
   * necessarily say that the data is stored as a \a T, it only
   * specifies in which form the get() method returns the data.
   * Thus a DataSource<const std::string&> returns a const ref to
   * a string, but may store the string itself by value.
   */
  template<typename T>
  class RTT_EXPORT DataSource
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

      typedef typename boost::intrusive_ptr<const DataSource<T> > const_ptr;

      /**
       * Return the data as type \a T.
       */
      virtual result_t get() const = 0;

      /**
       * Return the result of the last \a evaluate() function.
       */
      virtual result_t value() const = 0;

      virtual bool evaluate() const;

      virtual DataSource<T>* clone() const = 0;

      virtual DataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const = 0;

      virtual std::string getType() const;

      /**
       * Return usefull type info in a human readable format.
       */
      static  std::string GetType();

      virtual const TypeInfo* getTypeInfo() const;

      /**
       * Return the Orocos type info.
       */
      static const TypeInfo* GetTypeInfo();

      /**
       * Return the Orocos type name, without const, pointer or reference
       * qualifiers.
       */
      static  std::string GetTypeName();

      virtual std::string getTypeName() const;

      /**
       * This method narrows a DataSourceBase to a typeded DataSource,
       * possibly returning a new object.
       */
      static DataSource<T>* narrow(DataSourceBase* db);

  };

  /**
   * A DataSource which has set() methods.
   * @param T See DataSource for the semantics of \a T.
   */
  template<typename T>
  class RTT_EXPORT AssignableDataSource
    : public DataSource<T>
  {
  protected:
      ~AssignableDataSource();
  public:
      typedef typename DataSource<T>::value_t value_t;
      typedef typename boost::call_traits<value_t>::param_type param_t;
      typedef typename boost::call_traits<value_t>::reference reference_t;
      typedef typename boost::call_traits<value_t>::const_reference const_reference_t;

      // For assignment from another datasource, we use the call_traits convention but
      // remove the 'const' for the 'small' types. This to avoid requiring a DataSourceAdaptor.
      // Big types (classes) are still passed by const&.
      typedef typename boost::remove_const<typename boost::call_traits<value_t>::param_type>::type copy_t;

      /**
       * Use this type to store a pointer to an AssignableDataSource.
       */
      typedef boost::intrusive_ptr<AssignableDataSource<T> > shared_ptr;
      typedef typename boost::intrusive_ptr<const AssignableDataSource<T> > const_ptr;

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

      /**
       * Get a const reference (or null) to the value of this DataSource.
       * Getting a reference to an  internal data structure is not thread-safe.
       * DataSources which wish to protect their data from concurrent access may
       * return the null reference with this method. All calls to rvalue() must first
       * check whether they do not return null.
       */
      virtual const_reference_t rvalue() const = 0;

      virtual bool update( DataSourceBase* other );

      virtual CommandInterface* updateCommand( DataSourceBase* other);

      virtual AssignableDataSource<T>* clone() const = 0;

      virtual AssignableDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const = 0;

      virtual bool updateBlob(int protocol, const void* data);

      virtual void* server(int protocol, void* data );

      /**
       * This method narrows a DataSourceBase to a typeded AssignableDataSource,
       * possibly returning a new object.
       */
      static AssignableDataSource<T>* narrow(DataSourceBase* db);

  };
}

// workaround inclusion dependencies.
#ifndef ORO_CORELIB_DATASOURCES_HPP
#include "DataSource.inl"
#endif
#endif

