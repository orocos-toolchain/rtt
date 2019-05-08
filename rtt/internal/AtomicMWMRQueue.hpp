/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  AtomicQueue.hpp

                        AtomicQueue.hpp -  description
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


#ifndef ORO_CORELIB_ATOMIC_MWMR_QUEUE_HPP
#define ORO_CORELIB_ATOMIC_MWMR_QUEUE_HPP

#include "AtomicQueue.hpp"
#include "../os/CAS.hpp"
#include <utility>

namespace RTT
{
    namespace internal {
    /**
     * Create an atomic, non-blocking single ended queue (FIFO) for storing
     * a \b pointer to \a T. It is a
     * Many Readers, Many Writers implementation
     * based on the atomic Compare And Swap instruction. Any number of threads
     * may access the queue concurrently.
     *
     * This queue tries to obey strict ordering, but under high contention
     * of reads interfering writes, one or more elements may be dequeued out of order.
     * For this reason, size() is expensive to accurately calculate the size.
     *
     * Due to the same limitations, it is possible that the full capacity of the
     * queue is not used (simulations show seldomly an off by one element if capacity==10) and
     * that isFull() returns true, while size() < capacity().
     *
     * @warning You can not store null pointers.
     * @param T The pointer type to be stored in the Queue.
     * Example : AtomicQueue< A* > is a queue of pointers to A.
     *
     * @ingroup CoreLibBuffers
     */
    template<class T>
    class AtomicMWMRQueue : public AtomicQueue<T>
    {
        const int _size;
        typedef T C;
        typedef volatile C* CachePtrType;
        typedef C* volatile CacheObjType;
        typedef C  ValueType;
        typedef C* PtrType;

        union SIndexes
        {
        	unsigned long _value;
        	unsigned short _index[2];
        };

        /**
         * The pointer to the buffer can be cached,
         * the contents are volatile.
         */
        CachePtrType  _buf;

        /**
         * The indexes are packed into one double word.
         * Therefore the read and write index can be read and written atomically.
         */
        volatile SIndexes _indxes;

        /**
         * The loose ordering may cause missed items in our
         * queue which are not pointed at by the read pointer.
         * This function recovers such items from _buf.
         * @return zero if nothing to recover, the location of
         * a lost item otherwise.
         */
        CachePtrType recover_r() const
        {
            // The implementation starts from the read pointer,
            // and wraps around until all fields were scanned.
            // As such, the out-of-order elements will at least
            // be returned in their relative order.
            SIndexes start;
            start._value = _indxes._value;
            unsigned short r = start._index[1];
            while( r != _size) {
                if (_buf[r])
                    return &_buf[r];
                ++r;
            }
            r = 0;
            while( r != start._index[1]) {
                if (_buf[r])
                    return &_buf[r];
                ++r;
            }
            return 0;
        }

        /**
         * Atomic advance and wrap of the Write pointer.
         * Return the old position or zero if queue is full.
         */
        CachePtrType propose_w()
        {
        	SIndexes oldval, newval;
            do {
            	oldval._value = _indxes._value; /*Points to a free writable pointer.*/
                newval._value = oldval._value; /*Points to the next writable pointer.*/
                // check for full on a *Copy* of oldval:
                if ( (newval._index[0] == newval._index[1] - 1) || (newval._index[0] == newval._index[1] + _size - 1) )
                {
                    // note: in case of high contention, there might be existing empty fields
                    // in _buf that aren't used.
                    return 0;
                }
                ++newval._index[0];
                if ( newval._index[0] == _size )
                	newval._index[0] = 0;
                // if ptr is unchanged, replace it with newval.
            } while ( !os::CAS( &_indxes._value, oldval._value, newval._value) );

            // the returned field may contain data, in that case, the caller needs to retry.
            return &_buf[ oldval._index[0] ];
        }
        /**
         * Atomic advance and wrap of the Read pointer.
         * Return the data position or zero if queue is empty.
         */
        CachePtrType propose_r()
        {
        	SIndexes oldval, newval;
            do {
            	oldval._value = _indxes._value;
            	newval._value = oldval._value;
                // check for empty on a *Copy* of oldval:
                if ( newval._index[0] == newval._index[1] )
                {
                    // seldom: R and W are indicating empty, but 'lost' fields
                    // are to be picked up. Return these
                    // that would have been read eventually after some writes.
                    return recover_r();
                }
                ++newval._index[1];
                if ( newval._index[1] == _size )
                	newval._index[1] = 0;

            } while ( !os::CAS( &_indxes._value, oldval._value, newval._value) );
            // the returned field may contain *no* data, in that case, the caller needs to retry.
            // as such r will advance until it hits a data sample or write pointer.
            return &_buf[oldval._index[1] ];
        }

        // non-copyable !
        AtomicMWMRQueue( const AtomicQueue<T>& );
    public:
        typedef typename AtomicQueue<T>::size_type size_type;

        /**
         * Create an AtomicQueue with queue size \a size.
         * @param size The size of the queue, should be 1 or greater.
         */
        AtomicMWMRQueue( unsigned int size )
            : _size(size+1)
        {
            _buf= new C[_size];
            this->clear();
        }

        ~AtomicMWMRQueue()
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
            return val._index[0] == val._index[1] && recover_r() == 0;
        }

        /**
         * Return the maximum number of items this queue can contain.
         */
        size_type capacity() const
        {
            return _size -1;
        }

        /**
         * Return the exact number of elements in the queue.
         * This is slow because it scans the whole
         * queue.
         */
        size_type size() const
        {
            int c = 0, ret = 0;
            while (c != _size ) {
                if (_buf[c++] )
                    ++ret;
            }
            return ret;
            //int c = (_indxes._index[0] - _indxes._index[1]);
            //return c >= 0 ? c : c + _size;
        }

        /**
         * Enqueue an item.
         * @param value The value to enqueue, not zero.
         * @return false if queue is full or value is zero, true if queued.
         */
        bool enqueue(const T& value)
        {
            if ( value == 0 )
                return false;
            CachePtrType loc;
            C null = 0;
            do {
                loc = propose_w();
                if ( loc == 0 )
                    return false; //full
                // if loc contains a zero, write it, otherwise, re-try.
            } while( !os::CAS(loc, null, value));
            return true;
        }

        /**
         * Dequeue an item.
         * @param value The value dequeued.
         * @return false if queue is empty, true if dequeued.
         */
        bool dequeue( T& result )
        {
            CachePtrType loc;
            C null = 0;
            do {
                loc = propose_r();
                if ( loc == 0 )
                    return false; // empty
                result = *loc;
                // if loc still contains result, clear it, otherwise, re-try.
            } while( result == 0 || !os::CAS(loc, result, null) );
            assert(result);
            return true;
        }

        /**
         * Return the next to be read value.
         */
        const T front() const
        {
            return _buf[_indxes._index[1] ];
        }

        /**
         * Clear all contents of the Queue and thus make it empty.
         */
        void clear()
        {
            for(int i = 0 ; i != _size; ++i) {
                _buf[i] = 0;
            }
            _indxes._value = 0;
        }

    };

}}

#endif
