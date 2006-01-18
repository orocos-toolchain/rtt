/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  DataSourceBase.hpp 

                        DataSourceBase.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 

#ifndef CORELIB_DATASOURCE_BASE_HPP
#define CORELIB_DATASOURCE_BASE_HPP

#include <boost/intrusive_ptr.hpp>
#include <map>
#include <string>
#include <os/oro_atomic.h>

namespace ORO_CoreLib
{
  /**
   * @brief The base class for all DataSource's
   *
   * The DataSource is an object containing Data of any type. It's
   * interface is designed for dynamic build-up and destruction of
   * these objects and allowing Commands, Properties etc to use them
   * as 'storage' devices which have the dual \a copy() /\a clone() semantics
   * (which is heavily used by the Orocos Task Infrastructure).
   *
   * @important DataSource's are reference counted and must be allocated on the headp. Use
   * DataSourceBase::shared_ptr or DataSource<T>::shared_ptr to deal
   * with cleanup of allocated DataSources. You are not allowed to delete
   * a DataSource. If you must have the pointer of a DataSource, use
   * the \a .get() method of the \a shared_ptr class. 
   *
   * Once a newly created DataSource is assigned to a \a shared_ptr,
   * it will be deleted when that pointer goes out of scope and is not
   * shared by other \a shared_ptr objects.
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
      atomic_t refcount;
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

      DataSourceBase();
      /**
       * Increase the reference count by one.
       */
      void ref();
      /**
       * Decrease the reference count by one and delete this on zero.
       */
      void deref();

      /**
       * Reset the data to initial values.
       */
      virtual void reset();

      /**
       * Force an evaluation of the DataSourceBase.
       * @return true on successful evaluation.
       * If the DataSource itself contains a boolean, return that boolean.
       */
      virtual bool evaluate() const = 0;

      /**
       * Return a shallow clone of this DataSource. This method
       * returns a duplicate of this instance which re-uses the
       * DataSources this DataSource holds reference to. The
       * clone() function is thus a non-deep copy.
       */
      virtual DataSourceBase* clone() const = 0;

      /**
       * Create a deep copy of this DataSource, unless it is already
       * cloned. Places the association (parent, clone) in \a
       * alreadyCloned.  If the DataSource is non-copyable (for
       * example it represents the Property of a Task ), \a this may
       * be returned.
       */
      virtual DataSourceBase* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) = 0;

      /**
       * Return usefull type info in a human readable format.
       */
      virtual std::string getType() const = 0;
  };
}

void intrusive_ptr_add_ref( ORO_CoreLib::DataSourceBase* p );
void intrusive_ptr_release( ORO_CoreLib::DataSourceBase* p );

#endif
