/***************************************************************************
  tag: Peter Soetens  Thu Jan 13 10:24:51 CET 2005  BufferLockFree.hpp 

                        BufferLockFree.hpp -  description
                           -------------------
    begin                : Thu January 13 2005
    copyright            : (C) 2005 Peter Soetens
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

#ifndef ORO_BUFFER_LOCK_FREE_HPP
#define ORO_BUFFER_LOCK_FREE_HPP
 
#include "os/oro_atomic.h"
#include "os/CAS.hpp"
#include "BufferPolicy.hpp"
#include "BufferInterface.hpp"
#include "AtomicQueue.hpp"
#include "MemoryPool.hpp"
#include <vector>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace ORO_CoreLib
{

    using ORO_OS::CAS;

    /**
     * A Lock-free buffer implementation to read and write
     * data of type \a T in a FIFO way. Optionally block (synchronize) on an
     * \a empty of \a full buffer (See \a Policy below ). The
     * default Policy is thus completely Asynchronous (non-blocking).
     * No memory allocation is done during read or write, any number of threads
     * may access this buffer concurrently.
     * @param T The value type to be stored in the Buffer.
     * Example : BufferLockFree<A> is a buffer which holds values of type A.
     * @param ReadPolicy The Policy to block (wait) on \a empty (during read),
     * using \a BlockingPolicy, or to return \a false, using \a NonBlockingPolicy (Default).
     * This does not influence partial filled buffer behaviour.
     * @param WritePolicy The Policy to block (wait) on \a full (during write), 
     * using \a BlockingPolicy, or to return \a false, using \a NonBlockingPolicy (Default).
     * This does not influence partial filled buffer behaviour.
     */
    template< class T, class ReadPolicy = NonBlockingPolicy, class WritePolicy = NonBlockingPolicy >
    class BufferLockFree
        : public BufferInterface<T>
    {
    public:
        typedef typename ReadInterface<T>::reference_t reference_t;
        typedef typename WriteInterface<T>::param_t param_t;
        typedef typename BufferInterface<T>::size_type size_type;
        typedef T value_t;
    private:
        typedef T Item;
        AtomicQueue<Item*,ReadPolicy,WritePolicy> bufs;
        // is mutable because of reference counting.
        mutable FixedSizeMemoryPool<Item> mpool;
    public:
        /**
         * Create a lock-free buffer wich can store \a bufsize elements.
         * @param bufsize the capacity of the buffer.
'         */
        BufferLockFree( unsigned int bufsize, const T& initial_value = T() )
            : bufs( bufsize ), mpool(bufsize, initial_value)
        {
        }

        size_type capacity() const
        {
            return bufs.capacity();
        }

        size_type size() const
        {
            return bufs.size();
        }

        bool empty() const
        {
            return bufs.isEmpty();
        }

        bool full() const
        {
            return bufs.isFull();
        }

        void clear()
        {
            Item* item;
            while ( bufs.dequeue(item) )
                mpool.deallocate( item );
        }

        /**
         * Write a single value to the buffer.
         * @param d the value to write
         * @return false if the buffer is full.
         * @deprecated by Push( param_t item )
         */
        bool write( param_t d ) {
            return this->Push( d );
        }

        bool Push( param_t item) 
        {
            Item* mitem = mpool.allocate();
            if ( mitem == 0 ) // queue full.
                return false;
            // copy over.
            *mitem = item;
            if (bufs.enqueue( mitem ) == false ) { // can this ever happen ?
                //mpool.deallocate( mitem );
                //return false;
                assert(false && "Race detected in Push()");
            }
            return true;
        }

        /**
         * Write a sequence of values to the buffer. 
         * Block if full if Policy is BlockingPolicy.
         * @param d the values to write
         * @return the number of values written (may be less than d.size())
         * @deprecated by Push(const std::vector<T>& item)
         */
        size_type write( const std::vector<T>& d) {
            return this->Push( d );
        }

        size_type Push(const std::vector<T>& items)
        {
            int towrite  = items.size();
            typename std::vector<T>::const_iterator it;
            for(  it = items.begin(); it != items.end(); ++it)
                if ( this->Push( *it ) == false )
                    break;
            return towrite - (items.end() - it);
        }


        /**
         * Read the oldest value from the buffer.
         * Block if empty if Policy is BlockingPolicy.
         * @param res is to be filled with a value from the buffer.
         * @return true if something was read.
         * @deprecated by Pop( reference_t item )
         */
        bool read(T& res) {
            return this->Pop( res );
        }

        value_t front() const {
            Item* orig;
            orig = bufs.lockfront(mpool);
            // if orig == 0, then queue is empty
            if (orig == 0)
                return value_t();
            
            // ok, copy, unlock and return front.
            value_t ret = *orig;
            mpool.unlock( orig );
            return ret;
        }

        bool Pop( reference_t item )
        {
            Item* ipop;
            if (bufs.dequeue( ipop ) == false )
                return false;
            item = *ipop;
            if (mpool.deallocate( ipop ) == false )
                assert(false);
            return true;
        }

        /**
         * Read the whole buffer. Block if empty if Policy is BlockingPolicy.
         * @param res is to be filled with all values in the buffer,
         * with res.begin() the oldest value.
         * @return the number of items read.
         * @deprecated by Pop( std::vector<T>& items )
         */
        size_type read(std::vector<T>& res)
        {
            return this->Pop( res );
        }

        size_type Pop(std::vector<T>& items )
        {
            Item* ipop;
            items.clear();
            while( bufs.dequeue(ipop) ) {
                items.push_back( *ipop );
                if (mpool.deallocate(ipop) == false)
                    assert(false);
            }
            return items.size();
        }

    };

    extern template class BufferLockFree<double>;
}

#endif
