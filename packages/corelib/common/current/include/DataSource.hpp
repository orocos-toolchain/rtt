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

#include <boost/intrusive_ptr.hpp>
#include <boost/type_traits.hpp>

#include <map>
#include <vector>
#include <string>
#include "DataSourceTypeInfo.hpp"

namespace
{
  // combines boost::remove_reference and boost::remove_const, is
  // basically a type manipulator that returns T for a type const T&,
  // and T for other types T...
  template<typename T>
  struct remove_cr
  {
    typedef typename boost::remove_const<
      typename boost::remove_reference<T>::type>::type type;
  };
}

namespace ORO_CoreLib
{
  /**
   * @brief The base class for all DataSource's
   *
   * The DataSource is an object containing Data of any type. It's
   * interface is designed for dynamic build-up and destruction of
   * these objects and allowing Commands, Properties etc to use them
   * as 'storage' devices which have the dual copy()/clone() semantics
   * (which is heavily used by the Orocos Task Infrastructure).
   *
   * DataSource's are reference counted.  Use
   * DataSourceBase::shared_ptr or DataSource<T>::shared_ptr to deal
   * with this automatically, or don't forget to call ref and deref..
   *
   * @see DataSource
   */
  class DataSourceBase
  {
      /**
         We keep the refcount ourselves.  We aren't using
         boost::shared_ptr, because boost::intrusive_ptr is better,
         exactly because it can be used with refcounts that are stored
         in the class itself.  Advantages are that the shared_ptr's for
         derived classes use the same refcount, which is of course very
         much desired, and that refcounting happens in an efficient way,
         which is also nice :)
      */
    int refcount;
  protected:
      /** the destructor is private.  You are not allowed to delete this
       * class yourself, use a shared pointer !
       */
    virtual ~DataSourceBase();
  public:
      /**
       * Use this type to store a pointer to a DataSourceBase.
       */
      typedef boost::intrusive_ptr<DataSourceBase> shared_ptr;

      DataSourceBase() : refcount( 0 ) {};
      /**
       * Increase the reference count by one.
       */
      void ref() { ++refcount; };
      /**
       * Decrease the reference count by one and delete this on zero.
       */
      void deref() { if ( --refcount <= 0 ) delete this; };

      /**
       * Reset the data to initial values.
       */
      virtual void reset();

      /**
       * Force an evaluation of the DataSourceBase.
       */
      virtual void evaluate() const = 0;
      /**
       * Create a deep copy of this DataSource, unless it is already cloned and place the association (parent, clone) in \a alreadyCloned.
       */
      virtual DataSourceBase* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) = 0;

      /**
       * Return usefull type info in a human readable format.
       */
      virtual std::string getType() const = 0;
  };

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
  public:
      typedef typename boost::intrusive_ptr<DataSource<T> > shared_ptr;

      virtual ~DataSource();
      /**
       * return the data you need to return..
       */
      virtual T get() const = 0;

      virtual void evaluate() const { this->get(); }
      /**
       * Clone Software Pattern.
       */
      virtual DataSource<T>* clone() const = 0;

      virtual DataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) = 0;

      virtual std::string getType() const;

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
          + detail::DataSourceTypeInfo< typename remove_cr<T>::type >::getType();
  }

  /**
   * A DataSource which has set() methods.
   */
  template<typename T>
  class AssignableDataSource
    : public DataSource<T>
  {
  public:
      typedef boost::intrusive_ptr<AssignableDataSource<T> > shared_ptr;

      virtual void set( T t ) = 0;

      virtual T& set() = 0;

      virtual AssignableDataSource<T>* clone() const = 0;

      virtual AssignableDataSource<T>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) = 0;
  };

  /**
   * A AssignableDataSource specialisation for const&.
   */
  template<typename _T>
  class AssignableDataSource<const _T&>
    : public DataSource<const _T&>
  {
  public:
      typedef const _T& T;
      typedef boost::intrusive_ptr<AssignableDataSource<T> > shared_ptr;

      virtual void set( T t ) = 0;

      virtual _T& set() = 0;

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

void intrusive_ptr_add_ref( ORO_CoreLib::DataSourceBase* p );
void intrusive_ptr_release( ORO_CoreLib::DataSourceBase* p );

#endif
