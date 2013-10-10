/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ReadOnlyPointer.hpp

                        ReadOnlyPointer.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Sylvain Joyeux
    email                : Sylvain Joyeux <sylvain.joyeux@m4x.org>

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


#ifndef RTT_READ_WRITE_POINTER_HPP
#define RTT_READ_WRITE_POINTER_HPP

#include <boost/intrusive_ptr.hpp>
#include <boost/call_traits.hpp>

#include "../os/Mutex.hpp"
#include "../os/MutexLock.hpp"

namespace RTT
{ namespace extras {

        template<typename T>
        struct ROPtrInternal
        {
            os::Mutex  lock;
            T*     value;
            size_t readers;

            ROPtrInternal(T* value)
                : value(value), readers(0) {}
            ~ROPtrInternal() { delete value; }

            void ref()
            { os::MutexLock do_lock(lock);
                ++readers;
            }
            bool deref()
            { os::MutexLock do_lock(lock);
                return (--readers) != 0;
            }
        };

        template<typename T>
        void intrusive_ptr_add_ref(ROPtrInternal<T>* data)
        {
            data->ref();
        }
        template<typename T>
        void intrusive_ptr_release(ROPtrInternal<T>* data)
        {
            if (!data->deref())
                delete data;
        }

    /** Smart pointer that allows safe sharing of data between multiple threads
     *
     * This smart pointer registers a memory area as being read-only. It can
     * therefore be shared safely between threads without adding means of
     * synchronization between the threads. Indeed: the value will *not* be
     * changed between the threads.
     *
     * If a thread wants to modify the value in-place (i.e. not do any copying),
     * it can get ownership of the object referenced by the ReadOnlyPointer by
     * calling ReadOnlyPointer::write_access. If this smart pointer if the only
     * owner of the memory zone, then no copy will be done. Otherwise, one copy
     * is done to satisfy the caller.
     */
    template<typename T>
    class ReadOnlyPointer
    {
        typedef ROPtrInternal<T> Internal;
        boost::intrusive_ptr<Internal> internal;
        typedef boost::call_traits<T> traits;

    public:
        ReadOnlyPointer(T* ptr = 0)
            : internal(new Internal(ptr)) {}

        typename traits::const_reference operator *() const { return *(internal->value); }
        T const* operator ->() const { return internal->value; }

        /** True if this refers to a non-NULL pointer */
        bool valid() const
        { return internal->value != 0; }

        T const* get() const
        {
            return internal->value;
        }

        /** Modifies the value referenced by this smart pointer
         *
         * After this call, \c ptr is owned by the smart pointer, and the caller
         * should not modify the value referenced by \c ptr (i.e. it should be
         * treated as read-only by the caller).
         *
         * This does not change the other instances of ReadOnlyPointer that were
         * referencing the same memory zone as \c this. I.e. after the following
         * snippet, ptr2 refers to value2 and ptr1 to value1.
         *
         * <code>
         * T* value1 = new T;
         * T* value2 = new T;
         * ReadOnlyPointer<T> ptr1(value1);
         * ReadOnlyPointer<T> ptr2(ptr1);
         * ptr2->reset(value2);
         * </code>
         *
         * If \c this is the only owner of the object it refers to, this object
         * will be deleted.
         */
        void reset(T* ptr)
        {
            boost::intrusive_ptr<Internal> safe = this->internal;
            if (!safe)
            {
                internal = new Internal(ptr);
                return;
            }

            if (safe->value == ptr)
                return;

            { os::MutexLock do_lock(safe->lock);
                if (safe->readers == 2) // we are sole owner
                {
                    delete safe->value;
                    safe->value = ptr;
                    return;
                }
            }

            // We must *not* change 'internal' while safe->lock is taken. The
            // above block returns in case we don't need to reallocate a new
            // Internal structure.
            //
            // In other words, if we are here, it is because we *need* to
            // reallocate.
            internal = new Internal(ptr);
        }

        /** Gets write access to the pointed-to object if it does not require
         * any copying
         *
         * This method is like write_access, except that it will return NULL if
         * a copy is needed
         *
         * If non-NULL, it is the responsibility of the caller to delete the
         * returned value.
         */
        T* try_write_access()
        {
            boost::intrusive_ptr<Internal> safe = this->internal;
            if (!safe)
                return 0;

            { os::MutexLock do_lock(safe->lock);
                if (safe->readers == 2)
                { // we're the only owner (don't forget +safe+ above).
                  // Just promote the current copy
                    T* value = 0;
                    std::swap(value, safe->value);
                    return value;
                }
                else
                { // there are other owners
                    return NULL;
                }
            }
        }

        /** Gets write access to the pointed-to object.
         *
         * If \c this is the only owner of that object, then no copy will be
         * done *and* the pointer will be invalidated. Otherwise, the method
         * returns a copy of the pointed-to object.
         *
         * If the copy might be a problem, one can use try_write_access to get
         * the object only when a copy is not needed.
         *
         * It is the responsibility of the caller to delete the returned value.
         */
        T* write_access()
        {
            boost::intrusive_ptr<Internal> safe = this->internal;
            if (!safe)
                return 0;

            { os::MutexLock do_lock(safe->lock);
                if (safe->readers == 2)
                { // we're the only owner (don't forget +safe+ above).
                  // Just promote the current copy
                    T* value = 0;
                    std::swap(value, safe->value);
                    return value;
                }
                else
                { // there are other owners, do a copy
                    return new T(*safe->value);
                }
            }
        }
    };
}}

#endif

