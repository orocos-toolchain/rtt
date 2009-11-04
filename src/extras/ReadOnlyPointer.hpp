/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  ReadWritePointer.hpp

                        ReadWritePointer.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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
                return (--readers);
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
        T* operator ->() const { return internal->value; }

        bool valid() const
        { return internal; }

        void reset(T* ptr)
        {
            internal = new Internal(ptr);
        }

        T* write_access()
        {
            boost::intrusive_ptr<Internal> safe = this->internal;
            if (!safe)
                return 0;

            T* value = 0;
            { os::MutexLock do_lock(safe->lock);
                if (safe->readers == 2)
                { // we're the only owner (don't forget the one just above ...).
                  // Just promote the current copy
                    std::swap(value, safe->value);
                }
                else
                { // there are other owners, do a copy
                    value = new T(*safe->value);
                }
            }

            internal.reset();
            return value;
        }
    };
}}

#endif

