/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  DataSourceBase.hpp

                        DataSourceBase.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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



#ifndef CORELIB_DATASOURCE_BASE_HPP
#define CORELIB_DATASOURCE_BASE_HPP

#include <boost/intrusive_ptr.hpp>
#include <map>
#include <string>
#include "../os/Atomic.hpp"
#include "../rtt-config.h"
#include "ActionInterface.hpp"
#include "../rtt-fwd.hpp"

namespace RTT
{ namespace base {

  /**
   * @brief The base class for all internal::DataSource's
   *
   * The internal::DataSource is an object containing Data of any type. It's
   * interface is designed for dynamic build-up and destruction of
   * these objects and allowing Commands, Properties etc to use them
   * as 'storage' devices which have the dual \a copy() /\a clone() semantics
   * (which is heavily used by the Orocos Task Infrastructure).
   *
   * @important internal::DataSource's are reference counted and must be allocated on the headp. Use
   * DataSourceBase::shared_ptr or internal::DataSource<T>::shared_ptr to deal
   * with cleanup of allocated DataSources. You are not allowed to delete
   * a DataSource. If you must have the pointer of a internal::DataSource, use
   * the \a .get() method of the \a shared_ptr class.
   *
   * Once a newly created internal::DataSource is assigned to a \a shared_ptr,
   * it will be deleted when that pointer goes out of scope and is not
   * shared by other \a shared_ptr objects.
   *
   * @see internal::DataSource
   */
  class RTT_API DataSourceBase
  {
  protected:
      /**
         We keep the refcount ourselves.  We aren't using
         boost::shared_ptr, because boost::intrusive_ptr is better,
         exactly because it can be used with refcounts that are stored
         in the class itself.  Advantages are that the shared_ptr's for
         derived classes use the same refcount, which is of course very
         much desired, and that refcounting happens in an efficient way,
         which is also nice :)
      */
      mutable os::AtomicInt refcount;

      /** the destructor is private.  You are not allowed to delete this
       * class RTT_API yourself, use a shared pointer !
       */
      virtual ~DataSourceBase();

  public:
      /**
       * Use this type to store a pointer to a DataSourceBase.
       */
      typedef boost::intrusive_ptr<DataSourceBase> shared_ptr;

      /**
       * Use this type to store a const pointer to a DataSourceBase.
       */
      typedef boost::intrusive_ptr<const DataSourceBase> const_ptr;

      /**
       * Returns a shared ptr to a DataSourceBase living on the stack.
       * Make sure that the shared_ptr does not outlive the stacked
       * element.
       */
      static shared_ptr stack_shared_ptr(DataSourceBase* dsb);

      /**
       * Returns a const shared_ptr to a DataSourceBase living on the stack.
       * Make sure that the const_ptr does not outlive the stacked
       * element.
       */
      static const_ptr stack_const_ptr(DataSourceBase* dsb);

      DataSourceBase();
      /**
       * Increase the reference count by one.
       */
      void ref() const;
      /**
       * Decrease the reference count by one and delete this on zero.
       */
      void deref() const;

      /**
       * Reset the data to initial values.
       */
      virtual void reset();

      /**
       * Force an evaluation of the DataSourceBase.
       * @return true on successful evaluation.
       * If the internal::DataSource itself contains a boolean, return that boolean.
       */
      virtual bool evaluate() const = 0;

      /**
       * In case the internal::DataSource returns a 'reference' type,
       * call this method to notify it that the data was updated
       * in the course of an invocation of get().
       */
      virtual void updated();

      /**
       * Update the value of this internal::DataSource with the value of an \a other DataSource.
       * Update does a full update of the value, adding extra
       * information if necessary.
       * @return false if the DataSources are of different type OR if the
       * contents of this internal::DataSource can not be updated.
       */
      virtual bool update( DataSourceBase* other );

      /**
       * Generate a ActionInterface object which will update this internal::DataSource
       * with the value of another internal::DataSource when execute()'ed.
       * @return zero if the internal::DataSource types do not match OR if the
       * contents of this internal::DataSource can not be updated.
       */
      virtual ActionInterface* updateCommand( DataSourceBase* other);

      /**
       * Update \a part of the value of this internal::DataSource with the value of an \a other DataSource.
       * Update does a partial update of the value, according to \a part, which is
       * most likely an index or hash value of some type.
       * @return false if the DataSources are of different type OR if the
       * contents of this internal::DataSource can not be partially updated.
       */
      virtual bool updatePart( DataSourceBase* part, DataSourceBase* other );

      /**
       * Generate a ActionInterface object which will partially update this internal::DataSource
       * with the value of another internal::DataSource when execute()'ed. \a part is an index or
       * hash value of some type.
       * @return zero if the internal::DataSource types do not match OR if the
       * contents of this internal::DataSource can not be partially updated.
       */
      virtual ActionInterface* updatePartCommand( DataSourceBase* part, DataSourceBase* other);

      /**
       * Return a shallow clone of this DataSource. This method
       * returns a duplicate of this instance which re-uses the
       * DataSources this internal::DataSource holds reference to. The
       * clone() function is thus a non-deep copy.
       */
      virtual DataSourceBase* clone() const = 0;

      /**
       * Create a deep copy of this internal::DataSource, unless it is already
       * cloned. Places the association (parent, clone) in \a
       * alreadyCloned.  If the internal::DataSource is non-copyable (for
       * example it represents the Property of a Task ), \a this may
       * be returned.
       */
      virtual DataSourceBase* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const = 0;

      /**
       * Return useful type info in a human readable format.
       */
      virtual std::string getType() const = 0;

      /**
       * Return the Orocos type info object.
       */
      virtual const types::TypeInfo* getTypeInfo() const = 0;

      /**
       * Return the Orocos type name, without const, pointer or reference
       * qualifiers.
       */
      virtual std::string getTypeName() const = 0;

      /**
       * Stream the contents of this object.
       * @see types::TypeInfo
       */
      std::ostream& write(std::ostream& os);

      /**
       * Get the contents of this object as a string.
       * @see types::TypeInfo
       */
      std::string toString();

      /**
       * Decompose the contents of this object into properties.
       * @see types::TypeInfo
       */
      bool decomposeType( PropertyBag& targetbag );

      /**
       * Compose the contents of this object from another datasource.
       * @see types::TypeInfo
       */
      bool composeType( DataSourceBase::shared_ptr source);

      /**
       * Inspect if this internal::DataSource is a proxy for a remote server object.
       * @return 0 if it is a local internal::DataSource, or the protocol id if it
       * is a proxy for a remove server.
       */
      virtual int serverProtocol() const;

      /**
       * Create an object server which 'mirrors' this DataSource.
       * @return The existing server if serverProtocol() == \a protocol, or a
       * \a new server object reference otherwise.
       * @see Operations.idl
       * @deprecated This function is nowhere used or can be replaced by
       * a call to a type transporter.
       */
      virtual void* server( int protocol, void* arg );

      /**
       * Create an object server which 'mirrors' this DataSource.
       * @return The existing server if serverProtocol() == \a protocol, or a
       * \a new method object reference otherwise.
       * @see Operations.idl
       * @deprecated This function is nowhere used or can be replaced by
       * a call to a type transporter.
       */
      virtual void* method( int protocol, internal::MethodC* orig, void* arg );
  };

    /**
     * Stream the contents of this object.
     * @see types::TypeInfo
     */
    RTT_API std::ostream& operator<<(std::ostream& os, DataSourceBase::shared_ptr dsb );

}}

RTT_API void intrusive_ptr_add_ref(const RTT::base::DataSourceBase* p );
RTT_API void intrusive_ptr_release(const RTT::base::DataSourceBase* p );

#endif
