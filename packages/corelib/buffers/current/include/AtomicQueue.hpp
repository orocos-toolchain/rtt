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
#include <utility>

namespace ORO_CoreLib 
{
    /**
     * Create an atomic queue (FIFO) for storing
     * a pointer \a T by value. It is a 
     * Many Readers, Many Writers implementation
     * based on the atomic Compare And Swap instruction.
     * WARNING : You can not store null pointers.
     * @param T The pointer type to be stored in the Queue.
     * Example : AtomicQueue< A* > is a queue of pointers to A.
     */
    template<class T>
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
    public:
        /**
         * Create an AtomicQueue with queue size \a size.
         * @param size The size of the queue, should be 1 or greater.
         */
        AtomicQueue(int size)
            : _size(size+1)
        {
            _buf= new C[_size];
            this->clear();
        }

        ~AtomicQueue()
        {
            delete[] _buf;
        }

        bool isFull()
        {
            // two cases where the queue is full : 
            // if wptr is one behind rptr or if wptr is at end
            // and rptr at beginning.
            return _wptr->first != 0 || _wptr == _rptr - 1 || _wptr == _rptr+_size - 1;
        }
        
        bool isEmpty()
        {
            // empty if nothing to read.
            return _rptr->first == 0;
        }

        int capacity()
        {
            return _size -1;
        }

        bool enqueue(const T& value)
        {
            if ( value == 0 )
                return false;
            CachePtrType loc = advance_w();
            if ( loc == 0 )
                return false;
            loc->first = value;
            return true;
        }

        int enqueueCounted(const T& value)
        {
            if ( value == 0 )
                return 0;
            CachePtrType loc = advance_w();
            if ( loc == 0 )
                return 0;
            loc->first = value;
            return loc->second;
        }

        bool dequeue( T& result )
        {
            CachePtrType loc = advance_r();
            if ( loc == 0 )
                return false;
            result = loc->first;
            loc->first   = 0; // this releases the cell to write to.
            loc->second += _size; // give the cell a new number.
            return true;
        }

        int dequeueCounted( T& result )
        {
            CachePtrType loc = advance_r();
            if ( loc == 0 )
                return false;
            result = loc->first;
            int nr = loc->second;
            loc->first = 0; // this releases the cell to write to.
            loc->second += _size; // give the cell a new number.
            return nr;
        }

        void clear()
        {
            for(int i = 0 ; i != _size; ++i) {
                _buf[i].first  = 0;
                _buf[i].second = i+1; // init the counters
            }
            _rptr = _buf;
            _wptr = _buf;
        }
            
    };

}

#endif
