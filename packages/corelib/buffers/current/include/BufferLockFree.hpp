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
     * No memory allocation is done during read or write, but the maximum number
     * of threads which can access this object is defined by
     * MAX_THREADS.
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
        /** 
         * @brief The maximum number of threads.
         *
         * The number of threads which may concurrently access this buffer.
         */
        const unsigned int MAX_THREADS;

        typedef typename ReadInterface<T>::reference_t reference_t;
        typedef typename WriteInterface<T>::param_t param_t;
        typedef typename BufferInterface<T>::size_t size_t;
        typedef T value_t;
    private:
        struct Item {
            Item()  {
                //ATOMIC_INIT(count);
                atomic_set(&count,-1);
            }
            atomic_t count;  // refcount
            std::vector<T> data;
        };

        Item* bufs;
        Item* volatile active;

        WritePolicy write_policy;
        ReadPolicy read_policy;
    public:
        /**
         * Create a lock-free buffer wich can store \a bufsize elements.
         * @param bufsize the capacity of the buffer.
         * @param threads the number of threads which may concurrently
         * read or write this buffer. Defaults to ORONUM_OS_MAX_THREADS, but you
         * may lower this number in case not all threads will read this buffer.
         * A lower number will consume less memory.
'         */
        BufferLockFree(unsigned int bufsize, unsigned int threads = ORONUM_OS_MAX_THREADS )
            : MAX_THREADS( threads ), write_policy( bufsize ), read_policy(0)
        {
            // each thread has one 'working' buffer, and one 'active' buffer
            // lock.
            const unsigned int BUF_NUM = MAX_THREADS * 2;
            bufs = new Item[ BUF_NUM ];
            for (unsigned int i=0; i < BUF_NUM; ++i) {
                bufs[i].data.reserve( bufsize ); // pre-allocate
            }
            // bootstrap the first buffer :
            active = bufs;
            atomic_inc( &active->count );
        }

        ~BufferLockFree() {
            delete[] bufs;
        }

        size_t capacity() const
        {
            return active->data.capacity();
        }

        size_t size() const
        {
            return active->data.size();
        }

        void clear()
        {
            Item* nextbuf = findEmptyBuf(); // find unused Item in bufs
            nextbuf->data.clear();
            Item* orig=0;
            int items = 0;
            do {
                if (orig)
                    atomic_dec(&orig->count);
                orig = lockAndGetActive();
                items = orig->data.size();
            } while ( CAS(&active, orig, nextbuf ) == false );
            atomic_dec( &orig->count ); // lockAndGetActive
            atomic_dec( &orig->count ); // ref count
            // update policies.
            read_policy.pop(items);
            write_policy.push(items);
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
            write_policy.pop();
            Item* usingbuf = findEmptyBuf(); // find unused Item in bufs
            Item* orig=0;
            do {
                if (orig)
                    atomic_dec(&orig->count);
                orig = lockAndGetActive();
                if ( orig->data.size() == orig->data.capacity() ) { // check for full
                    atomic_dec( &orig->count );
                    atomic_dec( &usingbuf->count );
                    write_policy.push();
                    return false;
                }
                usingbuf->data = orig->data;
                usingbuf->data.push_back( item );
            } while ( CAS(&active, orig, usingbuf ) ==false);
            atomic_dec( &orig->count ); // lockAndGetActive()
            atomic_dec( &orig->count ); // set buffer free
            read_policy.push();
            return true;
        }

        /**
         * Write a sequence of values to the buffer. 
         * Block if full if Policy is BlockingPolicy.
         * @param d the values to write
         * @return the number of values written (may be less than d.size())
         * @deprecated by Push(const std::vector<T>& item)
         */
        size_t write( const std::vector<T>& d) {
            return this->Push( d );
        }

        size_t Push(const std::vector<T>& items)
        {
            write_policy.pop();
            Item* usingbuf = findEmptyBuf(); // find unused Item in bufs
            Item* orig=0;
            int towrite  = items.size();
            do {
                if (orig)
                    atomic_dec(&orig->count);
                orig = lockAndGetActive();
                int maxwrite = orig->data.capacity() - orig->data.size();
                if ( maxwrite == 0 ) {
                    atomic_dec( &orig->count ); // lockAndGetActive()
                    atomic_dec( &usingbuf->count ); // findEmptyBuf()
                    return 0;
                }
                if ( towrite > maxwrite )
                    towrite = maxwrite;
                usingbuf->data = orig->data;
                usingbuf->data.insert( usingbuf->data.end(), items.begin(), items.begin() + towrite );
            } while ( CAS(&active, orig, usingbuf ) ==false );
            atomic_dec( &orig->count ); // lockAndGetActive()
            atomic_dec( &orig->count ); // set buffer free
            read_policy.push( towrite );
            return towrite;
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

        bool Pop( reference_t item )
        {
            read_policy.pop();
            Item* nextbuf = findEmptyBuf(); // find unused Item in bufs
            nextbuf->data.clear();
            Item* orig=0;
            do {
                if (orig)
                    atomic_dec(&orig->count);
                orig = lockAndGetActive();
                if ( orig->data.empty() ) {
                    atomic_dec( &orig->count );
                    atomic_dec( &nextbuf->count );
		    read_policy.push();
                    return false;
                }
                item = orig->data.front();
                nextbuf->data.insert( nextbuf->data.begin(), orig->data.begin()+1, orig->data.end() );
            } while ( CAS(&active, orig, nextbuf ) ==false );
            atomic_dec( &orig->count ); // lockAndGetActive
            atomic_dec( &orig->count ); // ref count
            write_policy.push();
            return true;
        }

        /**
         * Read the whole buffer. Block if empty if Policy is BlockingPolicy.
         * @param res is to be filled with all values in the buffer,
         * with res.begin() the oldest value.
         * @return the number of items read.
         * @deprecated by Pop( std::vector<T>& items )
         */
        size_t read(std::vector<T>& res)
        {
            return this->Pop( res );
        }

        size_t Pop(std::vector<T>& items )
        {
            read_policy.pop(); // read at least 1 item.
            Item* nextbuf = findEmptyBuf(); // find unused Item in bufs
            nextbuf->data.clear(); // an empty buffer will become active next.
            Item* orig=0;
            do {
                if (orig)
                    atomic_dec(&orig->count);
                orig = lockAndGetActive();
                items = orig->data;
            } while ( CAS(&active, orig, nextbuf ) == false );
            atomic_dec( &orig->count ); //lockAndGetActive
            atomic_dec( &orig->count ); //ref count
            read_policy.pop( items.size() ); // pop additional read data.
            write_policy.push( items.size() );
            return items.size();
        }

    private:
        // If MAX_THREADS is large enough, this will always succeed :
        Item* findEmptyBuf() {
            Item* start = bufs;
            while( true ) {
                if ( atomic_inc_and_test( &start->count ) )
                    break;
                atomic_dec( &start->count );
                ++start;
                if (start == bufs+MAX_THREADS*2 ) // BUF_NUM
                    start = bufs; // in case of races, rewind
            }
            return start; // unique pointer across all threads
        }

        // This is a kind-of smart-pointer implementation
        // We could move it into Item itself and overload operator=
        Item* lockAndGetActive() {
            Item* orig=0;
            do {
                if (orig)
                    atomic_dec( &orig->count );
                orig = active;
                atomic_inc( &orig->count );
            } while ( CAS(&active, orig, orig ) == false );
            return orig;
        }

    };

    extern template class BufferLockFree<double>;
}

#endif
