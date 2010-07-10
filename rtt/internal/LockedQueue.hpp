/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  LockedQueue.hpp

                        LockedQueue.hpp -  description
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


#ifndef ORO_LOCKED_QUEUE_HPP
#define ORO_LOCKED_QUEUE_HPP

#include <deque>
#include "../os/Mutex.hpp"
#include "../os/MutexLock.hpp"

namespace RTT
{ namespace internal {

    /**
     * A lock-based queue implementation to \a enqueue or \a dequeue
     * a pointer of type \a T.
     * No memory allocation is done during read or write.
     * @param T The pointer type to be stored in the queue.
     * Example : \begincode LockedQueue<A*> \endcode is a queue which holds values of type A.
     * @ingroup CoreLibBuffers
     */
    template< class T>
    class LockedQueue
    {
    public:
        typedef T value_t;
    private:
        typedef std::deque<value_t> BufferType;
        typedef typename BufferType::iterator Iterator;
        typedef typename BufferType::const_iterator CIterator;
        mutable os::Mutex lock;
        BufferType data;

        int cap;
        int counter;
        int dcounter;
    public:
        typedef unsigned int size_type;

        /**
         * Create a lock-based queue wich can store \a lsize elements.
         * @param lsize the capacity of the queue.
'        */
        LockedQueue(unsigned int lsize)
            : cap(lsize),
              counter(0), dcounter(0)
        {
            data.resize(lsize);
            data.resize(0);
        }

        ~LockedQueue() {
        }

        size_type capacity() const
        {
            return cap;
        }

        size_type size() const
        {
            os::MutexLock locker(lock);
            return data.size();
        }

        /**
         * Inspect if the Queue is empty.
         * @return true if empty, false otherwise.
         */
        bool isEmpty() const
        {
            os::MutexLock locker(lock);
            return data.empty();
        }

        /**
         * Inspect if the Queue is full.
         * @return true if full, false otherwise.
         */
        bool isFull() const
        {
            os::MutexLock locker(lock);
            return data.size() ==  cap;
        }

        void clear()
        {
            os::MutexLock locker(lock);
            data.clear();
        }

        /**
         * Enqueue an item.
         * @param value The value to enqueue.
         * @return false if queue is full, true if queued.
         */
        bool enqueue(const T& value)
        {
            {
                os::MutexLock locker(lock);
                if (cap == data.size() )
                    return false;
                data.push_back(value);
            }
            return true;
        }

        /**
         * Enqueue an item and return its 'ticket' number.
         * @param value The value to enqueue.
         * @return zero if the queue is full, the 'ticket' number otherwise.
         * @deprecated <b> Do not use this function </b>
         */
        int enqueueCounted(const T& value)
        {
            int ret;
            {
                os::MutexLock locker(lock);
                if (cap == data.size() )
                    return 0;
                data.push_back(value);
                ret = ++counter;
            }
            return ret;
        }

        /**
         * Dequeue an item.
         * @param result The value dequeued.
         * @return false if queue is empty, true if dequeued.
         */
        bool dequeue( T& result )
        {
            {
                os::MutexLock locker(lock);
                if ( data.empty() )
                    return false;
                result = data.front();
            }
            return true;
        }

        /**
         * Dequeue an item and return the same 'ticket' number when it was queued.
         * @param value The value dequeued.
         * @return zero if the queue is empty, the 'ticket' number otherwise.
         * @deprecated <b> Do not use this function </b>
         */
        int dequeueCounted( T& result )
        {
            int ret;
            {
                os::MutexLock locker(lock);
                if ( data.empty() )
                    return 0;
                result = data.front();
                ret = ++dcounter;
            }
            return ret;
        }

        /**
         * Returns the first element of the queue.
         */
        value_t front() const
        {
            os::MutexLock locker(lock);
            value_t item = value_t();
            if ( !data.empty() )
                item = data.front();
            return item;
        }

        /**
         * Returns the last element of the queue.
         */
        value_t back() const
        {
            os::MutexLock locker(lock);
            value_t item = value_t();
            if ( !data.empty() )
                item = data.back();
            return item;
        }

    };

}}

#endif
