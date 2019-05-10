/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  AtomicMWSRQueue.hpp

                        AtomicMWSRQueue.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef ORO_CORELIB_ATOMIC_MWSR_QUEUE_HPP
#define ORO_CORELIB_ATOMIC_MWSR_QUEUE_HPP

#include "AtomicQueue.hpp"
#include "../os/CAS.hpp"
#include <utility>

namespace RTT
{
    namespace internal
    {
        /**
         * Create an atomic, non-blocking Multi-Writer Single-Reader FIFO for storing
         * a pointer \a T by value. Any number of writer threads
         * may access the queue concurrently, but only one thread may read it.
         * @warning You can not store null pointers.
         * @param T The pointer type to be stored in the Queue.
         * Example : AtomicMWSRQueue< A* > is a queue of pointers to A.
         * @ingroup CoreLibBuffers
         */
        template<class T>
        class AtomicMWSRQueue : public AtomicQueue<T>
        {
            //typedef _T* T;
            const int _size;
            typedef T C;
            typedef volatile C* CachePtrType;
            typedef C* volatile CacheObjType;
            typedef C  ValueType;
            typedef C* PtrType;

            /**
             * Both read and write pointer are in a union.
             * This means a read's cas may fail because a
             * write happened (preemption). An implementation
             * with 2 distinct read/write pointers would not
             * suffer from this.
             */
            union SIndexes
            {
                unsigned long _value;
                unsigned short _index[2];
            };

            /**
             * The pointer to the buffer can be cached,
             * the contents are volatile.
             */
            CachePtrType _buf;

            /**
             * The indexes are packed into one double word.
             * Therefore the read and write index can be read and written atomically.
             */
            volatile SIndexes _indxes;

            /**
             * Atomic advance and wrap of the Write pointer.
             * Return the old position or zero if queue is full.
             */
            CachePtrType advance_w()
            {
                SIndexes oldval, newval;
                do
                {
                    oldval._value = _indxes._value; /*Points to a free writable pointer.*/
                    newval._value = oldval._value; /*Points to the next writable pointer.*/
                    // check for full :
                    if ((newval._index[0] == newval._index[1] - 1) || (newval._index[0] == newval._index[1] + _size - 1))
                    {
                        return 0;
                    }
                    newval._index[0]++;
                    if (newval._index[0] >= _size)
                        newval._index[0] = 0;
                    // if ptr is unchanged, replace it with newval.
                } while (!os::CAS(&_indxes._value, oldval._value, newval._value));
                // frome here on :
                // oldval is 'unique', other preempting threads
                // will have a different value for oldval, as
                // _wptr advances. As long as oldval has not been written,
                // rptr will not advance and wptr will remain stuck behind it.
                // return the old position to write to :
                return &_buf[oldval._index[0]];
            }

            /**
             * Advance and wrap of the Read pointer.
             * Only one thread may call this.
             */
            bool advance_r(T& result)
            {
                SIndexes oldval, newval;
                // read it:
                oldval._value = _indxes._value;
                result = _buf[oldval._index[1]];
                // return it if not yet written:
                if ( !result )
                    return false;
                // got it, clear field.
                _buf[oldval._index[1]] = 0;

                // move pointer:
                do
                {
                    // re-read indxes, since we are the only reader,
                    // _index[1] will not have changed since entry of this function
                    oldval._value = _indxes._value;
                    newval._value = oldval._value;
                    ++newval._index[1];
                    if (newval._index[1] >= _size)
                        newval._index[1] = 0;

                    // we need to CAS since the write pointer may have moved.
                    // this moves read pointer only:
                } while (!os::CAS(&_indxes._value, oldval._value, newval._value));

                return true;
            }

            // non-copyable !
            AtomicMWSRQueue(const AtomicMWSRQueue<T>&);

        public:
            typedef typename AtomicQueue<T>::size_type size_type;

            /**
             * Create an AtomicMWSRQueue with queue size \a size.
             * @param size The size of the queue, should be 1 or greater.
             */
            AtomicMWSRQueue(unsigned int size) :
                _size(size + 1)
            {
                _buf = new C[_size];
                this->clear();
            }

            ~AtomicMWSRQueue()
            {
                delete[] _buf;
            }

            /**
             * Inspect if the Queue is full.
             * @return true if full, false otherwise.
             */
            bool isFull() const
            {
                // two cases where the queue is full :
                // if wptr is one behind rptr or if wptr is at end
                // and rptr at beginning.
                SIndexes val;
                val._value = _indxes._value;
                return val._index[0] == val._index[1] - 1 || val._index[0] == val._index[1] + _size - 1;
            }

            /**
             * Inspect if the Queue is empty.
             * @return true if empty, false otherwise.
             */
            bool isEmpty() const
            {
                // empty if nothing to read.
                SIndexes val;
                val._value = _indxes._value;
                return val._index[0] == val._index[1];
            }

            /**
             * Return the maximum number of items this queue can contain.
             */
            size_type capacity() const
            {
                return _size - 1;
            }

            /**
             * Return the number of elements in the queue.
             */
            size_type size() const
            {
                SIndexes val;
                val._value = _indxes._value;
                int c = (val._index[0] - val._index[1]);
                return c >= 0 ? c : c + _size;
            }

            /**
             * Enqueue an item.
             * @param value The value to enqueue.
             * @return false if queue is full, true if queued.
             */
            bool enqueue(const T& value)
            {
                if (value == 0)
                    return false;
                CachePtrType loc = advance_w();
                if (loc == 0)
                    return false;
                *loc = value;
                return true;
            }

            /**
             * Dequeue an item.
             * @param value Stores the dequeued value. It is unchanged when
             * dequeue returns false and contains the dequeued value
             * when it returns true.
             * @return false if queue is empty, true if result was written.
             */
            bool dequeue(T& result)
            {
                T tmpresult;
                if (advance_r(tmpresult) ) {
                    result = tmpresult;
                    return true;
                }
                return false;
            }

            /**
             * Return the next to be read value.
             */
            const T front() const
            {
                return _buf[_indxes._index[1]];
            }

            /**
             * Clear all contents of the Queue and thus make it empty.
             */
            void clear()
            {
                for (int i = 0; i != _size; ++i)
                {
                    _buf[i] = 0;
                }
                _indxes._value = 0;
            }

        };

    }
}
#endif
