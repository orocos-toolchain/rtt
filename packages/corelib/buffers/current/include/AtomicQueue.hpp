/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  AtomicQueue.hpp 

                        AtomicQueue.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#ifndef ORO_CORELIB_ATOMIC_QUEUE_HPP
#define ORO_CORELIB_ATOMIC_QUEUE_HPP

#include <os/CAS.hpp>
#include "BufferPolicy.hpp"
#include <utility>

namespace ORO_CoreLib 
{
    /**
     * Create an atomic, non-blocking single ended queue (FIFO) for storing
     * a pointer \a T by value. It is a 
     * Many Readers, Many Writers implementation
     * based on the atomic Compare And Swap instruction. Any number of threads
     * may access the queue concurrently.
     * @warning You can not store null pointers.
     * @param T The pointer type to be stored in the Queue.
     * Example : AtomicQueue< A* > is a queue of pointers to A.
     * @param ReadPolicy The Policy to block (wait) on \a empty (during dequeue)
     * using \a BlockingPolicy, or to return \a false, using \a NonBlockingPolicy (Default).
     * This does not influence partial filled queue behaviour.
     * @param WritePolicy The Policy to block (wait) on \a full (during enqueue), 
     * using \a BlockingPolicy, or to return \a false, using \a NonBlockingPolicy (Default).
     * This does not influence partial filled buffer behaviour.
     */
    template<class T, class ReadPolicy = NonBlockingPolicy, class WritePolicy = NonBlockingPolicy>
    class AtomicQueue
    {
        //typedef _T* T;
        const int _size;
        typedef std::pair<T, int>  C;
        typedef volatile C* volatile WriteType;
        typedef volatile C* volatile const ReadType;
        typedef volatile C* CachePtrType;
        typedef C* volatile CacheObjType;
        typedef C  ValueType;
        typedef C* PtrType;

        /**
         * The pointer to the buffer can be cached,
         * the contents are volatile.
         */
        CachePtrType  _buf;

        /**
         * A volatile pointer to a volatile cell.
         */
        WriteType _wptr;

        /**
         * This pointer is also writable because we
         * set it to zero to indicate it has been read.
         */
        WriteType _rptr;

        WritePolicy write_policy;
        ReadPolicy read_policy;

        /**
         * Atomic advance and wrap of the Write pointer.
         * Return the old position or zero if queue is full.
         */
        CachePtrType advance_w()
        {
            CachePtrType oldval, newval;
            do {
                oldval = _wptr;
                newval =  oldval+1;
                if ( newval >= _buf+_size )
                    newval -= _size;
                // check for full : 
                if ( newval->first != 0 || newval == _rptr || newval == _rptr + _size )
                    return 0;
                // if ptr is unchanged, replace it with newval.
            } while ( !ORO_OS::CAS( &_wptr, oldval, newval) );
            // frome here on :
            // oldval is 'unique', other preempting threads
            // will have a different value for oldval, as
            // _wptr advances.

            // return the old position to write to :
            return oldval;
        }
        /**
         * Atomic advance and wrap of the Read pointer.
         * Return the data position or zero if queue is empty.
         */
        CachePtrType advance_r()
        {
            CachePtrType oldval, newval;
            do {
                oldval = _rptr;
                newval =  oldval+1;
                if ( newval >= _buf+_size )
                    newval -= _size;
                // check for empty : 
                if ( oldval->first == 0 )
                    return 0;
                // if ptr is unchanged, replace it with newval.
            } while ( !ORO_OS::CAS( &_rptr, oldval, newval) );
            // frome here on :
            // oldval is 'unique', other preempting threads
            // will have a different value for oldval, as
            // _rptr advances.

            // return the old position to read from :
            return oldval;
        }

        // non-copyable !
        AtomicQueue( const AtomicQueue<T>& );
    public:
        /**
         * Create an AtomicQueue with queue size \a size.
         * @param size The size of the queue, should be 1 or greater.
         */
        AtomicQueue( unsigned int size )
            : _size(size+1), write_policy(size), read_policy(0)
        {
            _buf= new C[_size];
            this->clear();
        }

        ~AtomicQueue()
        {
            delete[] _buf;
        }

        /**
         * Inspect if the Queue is full.
         * @return true if full, false otherwise.
         */
        bool isFull()
        {
            // two cases where the queue is full : 
            // if wptr is one behind rptr or if wptr is at end
            // and rptr at beginning.
            return _wptr->first != 0 || _wptr == _rptr - 1 || _wptr == _rptr+_size - 1;
        }
        
        /**
         * Inspect if the Queue is empty.
         * @return true if empty, false otherwise.
         */
        bool isEmpty()
        {
            // empty if nothing to read.
            return _rptr->first == 0;
        }

        /**
         * Return the maximum number of items this queue can contain.
         */
        int capacity()
        {
            return _size -1;
        }

        /**
         * Enqueue an item.
         * @param value The value to enqueue.
         * @return false if queue is full, true if queued.
         */
        bool enqueue(const T& value)
        {
            if ( value == 0 )
                return false;
            write_policy.pop();
            CachePtrType loc = advance_w();
            if ( loc == 0 )
                return false;
            loc->first = value;
            read_policy.push();
            return true;
        }

        /**
         * Enqueue an item and return its 'ticket' number.
         * @param value The value to enqueue.
         * @return zero if the queue is full, the 'ticket' number otherwise.
         */
        int enqueueCounted(const T& value)
        {
            if ( value == 0 )
                return 0;
            write_policy.pop();
            CachePtrType loc = advance_w();
            if ( loc == 0 )
                return 0;
            loc->first = value;
            read_policy.push();
            return loc->second;
        }

        /**
         * Dequeue an item.
         * @param value The value dequeued.
         * @return false if queue is empty, true if dequeued.
         */
        bool dequeue( T& result )
        {
            read_policy.pop();
            CachePtrType loc = advance_r();
            if ( loc == 0 )
                return false;
            result = loc->first;
            loc->second += _size; // give the cell a new number.
            loc->first   = 0; // this releases the cell to write to.
            write_policy.push();
            return true;
        }

        /**
         * Dequeue an item and return the same 'ticket' number when it was queued.
         * @param value The value dequeued.
         * @return zero if the queue is empty, the 'ticket' number otherwise.
         */
        int dequeueCounted( T& result )
        {
            read_policy.pop();
            CachePtrType loc = advance_r();
            if ( loc == 0 )
                return false;
            result = loc->first;
            int nr = loc->second;
            loc->second += _size; // give the cell a new number.
            loc->first = 0; // this releases the cell to write to.
            write_policy.push();
            return nr;
        }

        /**
         * Clear all contents of the Queue and thus make it empty.
         */
        void clear()
        {
            for(int i = 0 ; i != _size; ++i) {
                if ( _buf[i].first != 0 ) {
                    _buf[i].first  = 0;
                }
                _buf[i].second = i+1; // init the counters
            }
            _rptr = _buf;
            _wptr = _buf;
            write_policy.reset( _size - 1 );
            read_policy.reset(0);
        }
            
    };

}

#endif
