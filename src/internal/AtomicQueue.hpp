/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  AtomicQueue.hpp

                        AtomicQueue.hpp -  description
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


#ifndef ORO_QUEUE_LOCK_FREE_HPP
#define ORO_QUEUE_LOCK_FREE_HPP

#include <vector>
#include "os/oro_atomic.h"
#include "os/CAS.hpp"
#include "../base/BufferPolicy.hpp"

namespace RTT
{
    /**
     * A lock-free queue implementation to \a enqueue or \a dequeue
     * a pointer of type \a T.
     * No memory allocation is done during read or write, but the maximum number
     * of threads which can access this object is defined by
     * MAX_THREADS.
     * @param T The pointer type to be stored in the queue.
     * Example : \begincode AtomicQueue<A*> \endcode is a queue which holds values of type A.
     * @param ReadPolicy The Policy to block (wait) on \a empty (during dequeue)
     * using \a BlockingPolicy, or to return \a false, using \a NonBlockingPolicy (Default).
     * This does not influence partial filled queue behaviour.
     * @param WritePolicy The Policy to block (wait) on \a full (during enqueue),
     * using \a BlockingPolicy, or to return \a false, using \a NonBlockingPolicy (Default).
     * This does not influence partial filled buffer behaviour.
     * @ingroup CoreLibBuffers
     */
    template< class T, class ReadPolicy = NonBlockingPolicy, class WritePolicy = NonBlockingPolicy>
    class AtomicQueue
    {
    public:
        /**
         * @brief The maximum number of threads.
         *
         * The number of threads which may concurrently access this buffer.
         */
        const unsigned int MAX_THREADS;

        typedef T value_t;
    private:
        typedef std::vector<value_t> BufferType;
        typedef typename BufferType::iterator Iterator;
        typedef typename BufferType::const_iterator CIterator;
        struct Item {
            Item()  {
                //ORO_ATOMIC_INIT(count);
                oro_atomic_set(&count,-1);
            }
            mutable oro_atomic_t count;  // refcount
            BufferType data;
        };

        struct StorageImpl
        {
            Item* items;
            StorageImpl(size_t alloc) : items( new Item[alloc] ) {
            }
            ~StorageImpl() {
                delete[] items;
            }
            Item& operator[](int i) {
                return items[i];
            }
        };

        /**
         * Our single storage
         */
        typedef StorageImpl* Storage;

        Storage newStorage(size_t alloc, size_t items, bool init = true)
        {
            Storage st( new StorageImpl(alloc) );
            for (unsigned int i=0; i < alloc; ++i) {
                (*st)[i].data.reserve( items ); // pre-allocate
            }
            // bootstrap the first queue :
            if (init) {
                active = &(*st)[0];
                oro_atomic_inc( &active->count );
            }

            return st;
        }

        Storage bufs;
        Item* volatile active;

        // each thread has one 'working' buffer, and one 'active' buffer
        // lock. Thus we require to allocate twice as much buffers as threads,
        // for all the locks to succeed in a worst case scenario.
        inline size_t BufNum() const {
            return MAX_THREADS * 2;
        }

        WritePolicy write_policy;
        ReadPolicy read_policy;

        oro_atomic_t counter;
        oro_atomic_t dcounter;
    public:
        typedef unsigned int size_type;

        /**
         * Create a lock-free queue wich can store \a lsize elements.
         * @param lsize the capacity of the queue.
         * @param threads the number of threads which may concurrently
         * read or write this buffer. Defaults to ORONUM_OS_MAX_THREADS, but you
         * may lower this number in case not all threads will read this buffer.
         * A lower number will consume less memory.
'        */
        AtomicQueue(unsigned int lsize, unsigned int threads = ORONUM_OS_MAX_THREADS )
            : MAX_THREADS( threads ), write_policy(lsize), read_policy(0)
        {
            const unsigned int BUF_NUM = BufNum();
            bufs = newStorage( BUF_NUM, lsize );
            oro_atomic_set(&counter,0);
            oro_atomic_set(&dcounter,0);
        }

        ~AtomicQueue() {
            delete bufs;
        }

        size_type capacity() const
        {
            size_type res;
            Item* orig = lockAndGetActive();
            res = orig->data.capacity();
            oro_atomic_dec( &orig->count ); // lockAndGetActive
            return res;
        }

        size_type size() const
        {
            size_type res;
            Item* orig = lockAndGetActive();
            res = orig->data.size();
            oro_atomic_dec( &orig->count ); // lockAndGetActive
            return res;
        }

        /**
         * Inspect if the Queue is empty.
         * @return true if empty, false otherwise.
         */
        bool isEmpty() const
        {
            bool res;
            Item* orig = lockAndGetActive();
            res = orig->data.empty();
            oro_atomic_dec( &orig->count ); // lockAndGetActive
            return res;
        }

        /**
         * Inspect if the Queue is full.
         * @return true if full, false otherwise.
         */
        bool isFull() const
        {
            bool res;
            Item* orig = lockAndGetActive();
            res = (orig->data.size() == orig->data.capacity());
            oro_atomic_dec( &orig->count ); // lockAndGetActive
            return res;
        }

        void clear()
        {
            Item* orig(0);
            Item* nextbuf(0);
            int items = 0;
            do {
                if (orig) {
                    oro_atomic_dec(&orig->count);
                    oro_atomic_dec(&nextbuf->count);
                }
                orig = lockAndGetActive();
                items = orig->data.size();
                nextbuf = findEmptyBuf(); // find unused Item in bufs
            } while ( OS::CAS(&active, orig, nextbuf ) == false );
            oro_atomic_dec( &orig->count ); // lockAndGetActive
            oro_atomic_dec( &orig->count ); // ref count
            oro_atomic_set(&counter,0);
            oro_atomic_set(&dcounter,0);
        }

        /**
         * Enqueue an item.
         * @param value The value to enqueue.
         * @return false if queue is full, true if queued.
         */
        bool enqueue(const T& value)
        {
            Item* orig=0;
            Item* usingbuf(0);
            write_policy.pop();
            do {
                if (orig) {
                    oro_atomic_dec(&orig->count);
                    oro_atomic_dec(&usingbuf->count);
                }
                orig = lockAndGetActive();
                if ( orig->data.size() == orig->data.capacity() ) { // check for full
                    oro_atomic_dec( &orig->count );
                    write_policy.push(); // return our token.
                    return false;
                }
                usingbuf = findEmptyBuf(); // find unused Item in bufs
                usingbuf->data = orig->data;
                usingbuf->data.push_back( value );
            } while ( OS::CAS(&active, orig, usingbuf ) ==false);
            oro_atomic_dec( &orig->count ); // lockAndGetActive()
            oro_atomic_dec( &orig->count ); // set queue free
            read_policy.push();
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
            if ( enqueue( value ) ) {
                oro_atomic_inc(&counter);
                return oro_atomic_read(&counter);
            }
            return 0;
        }

        /**
         * Dequeue an item.
         * @param result The value dequeued.
         * @return false if queue is empty, true if dequeued.
         */
        bool dequeue( T& result )
        {
            Item* orig=0;
            Item* usingbuf(0);
            read_policy.pop();
            do {
                if (orig) {
                    oro_atomic_dec(&orig->count);
                    oro_atomic_dec(&usingbuf->count);
                }
                orig = lockAndGetActive();
                if ( orig->data.empty() ) { // check for empty
                    oro_atomic_dec( &orig->count );
                    read_policy.push();
                    return false;
                }
                usingbuf = findEmptyBuf(); // find unused Item in bufs
                result = orig->data.front();
                CIterator it = ++(orig->data.begin());
                for ( ;  it != orig->data.end(); ++it )
                    usingbuf->data.push_back(*it);
                //usingbuf->data.insert( usingbuf->data.end(), it, orig->data.end() ); // ALTERNATIVE. (does it allocate??)
            } while ( OS::CAS(&active, orig, usingbuf ) ==false);
            oro_atomic_dec( &orig->count ); // lockAndGetActive()
            oro_atomic_dec( &orig->count ); // set queue free
            write_policy.push();
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
            if (dequeue(result) ) {
                oro_atomic_inc(&dcounter);
                return oro_atomic_read(&dcounter);
            }
            return 0;
        }

        /**
         * Return the next to be read value and lock
         * it in a MemoryPool, such that it is not freed.
         * The returned pointer must be unlock()'ed by the
         * user's code.
         */
        template<class MPoolType>
        T lockfront(MPoolType& mp) const
        {
            bool was_locked = false;
            Item* orig=0;
            T result;
            do {
                if (orig) {
                    mp.unlock( orig->data.front() );
                    oro_atomic_dec(&orig->count);
                }
                orig = lockAndGetActive();
                if ( orig->data.empty() ) { // check for empty
                    oro_atomic_dec( &orig->count ); //lockAndGetActive
                    return 0;
                }

                was_locked = mp.lock( orig->data.front() );
                // retry if lock failed or read moved.
            } while( !was_locked );
            result = orig->data.front();
            oro_atomic_dec( &orig->count ); // lockAndGetActive()
            return result;
        }

        /**
         * Returns the first element of the queue.
         */
        value_t front() const
        {
            Item* orig = lockAndGetActive();
            value_t ret(orig->data.front());
            oro_atomic_dec( &orig->count ); //lockAndGetActive
            return ret;
        }

        /**
         * Returns the last element of the queue.
         */
        value_t back() const
        {
            Item* orig = lockAndGetActive();
            value_t ret(orig->data.back());
            oro_atomic_dec( &orig->count ); //lockAndGetActive
            return ret;
        }

    private:
        /**
         * Item returned is guaranteed empty
         */
        Item* findEmptyBuf() {
            // These two functions are copy/pasted from BufferLockFree.
            // If MAX_THREADS is large enough, this will always succeed :
            Item* start = &(*bufs)[0];
            while( true ) {
                if ( oro_atomic_inc_and_test( &start->count ) )
                    break;
                oro_atomic_dec( &start->count );
                ++start;
                if (start == &(*bufs)[0] + BufNum() )
                    start = &(*bufs)[0]; // in case of races, rewind
            }
            start->data.clear();
            return start; // unique pointer across all threads
        }

        /**
         * Only to be used by reserve() !
         * Caller must guarantee that active points to valid memory.
         */
        Item* lockAndGetActive() const {
            // only operates on active's refcount.
            Item* orig=0;
            do {
                if (orig)
                    oro_atomic_dec( &orig->count );
                orig = active;
                oro_atomic_inc( &orig->count );
                // this synchronisation point is 'aggressive' (a _sufficient_ condition)
                // if active is still equal to orig, the increase of orig->count is
                // surely valid, since no contention (change of active) occured.
            } while ( active != orig );
            return orig;
        }
    };

}

#endif
