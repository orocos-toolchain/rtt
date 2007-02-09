/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  MemoryPool.hpp 

                        MemoryPool.hpp -  description
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
 
 
#ifndef ORO_MEMORY_POOL_HPP
#define ORO_MEMORY_POOL_HPP

#include "AtomicQueue.hpp"
#include <vector>
#include <boost/shared_ptr.hpp>

namespace RTT
{

    /**
     * A memory pool in which allocate() and deallocate() are lock-free.
     * This implementation is for a variable amount of memory for \a value_types.
     *
     * Use reserve() or shrink() to reserve or remove the reservation of
     * memory for a type \a T. It returns in allocate objects of type \a T*.
     * Any number of concurrent threads may invoke allocate(), deallocate(),
     * lock() and unlock(). However, reserve() and shrink() may not be concurrently invoked.
     * 
     * @param T A class type for which memory will be allocated.
     * @see FixedSizeMemoryPool for a more memory efficient fixed size
     * memory pool.
     */
    template<class T>
    class MemoryPool
    {
    public:
        typedef T* pointer;

        typedef unsigned int size_type;

    protected:
        struct Item;
        /** The pool stores memory as void pointers */
        typedef AtomicQueue<void*> QueueType;
        typedef boost::shared_ptr<QueueType> QueueTypePtr;
        /** A vector of memory pools */
        typedef std::vector<std::pair<QueueTypePtr,Item*> > PoolType;

        unsigned int used_cap, alloc_cnt;
        PoolType mpool;

        T minit;

        /**
         * Adds a reference count.
         */
        struct Item {
            Item( const T& initial_value )
                : content(initial_value) {
                oro_atomic_set(&rc, 0);
            }
            Item()
                : content() {
                oro_atomic_set(&rc, 0);
            }
            // the order is important !
            T content;
            oro_atomic_t rc;
        };

        /**
         * For each additional pool, also store the location of the Item pointer.
         */
        void make_pool( size_type growsize )
        {
            Item* newit = new Item[growsize];
            mpool.push_back( std::make_pair(QueueTypePtr(new QueueType( growsize )), newit) );
            for( unsigned int i = 0; i < growsize; ++i ) {
                newit[i].content = minit;
                mpool.back().first->enqueue( static_cast<void*>(&newit[i]) );
            }
            assert( mpool.back().first->size() == growsize );
        }
    public:
        /**
         * Initialise the memory pool and already allocate some
         * memory.
         * @param startsize The initial size of the pool. The amount of
         * memory allocated will be doubled when growing. For example,
         * if the startsize is 4 and 5 items or more are needed, 8 new additional
         * will be allocated, totalling in 12 available items. If later on
         * 13 items or more are needed, 16 new additional items are allocated, 
         * totalling in 12+16=28 items and so on. Must be at least 1.
         * @param initial_value The initial value for all the
         * data returned the \em first time by allocate().
         * This parameter is usefull if T requires a first time initialisation,
         * for example, when storing an std::vector. The data stored in
         * this pool is only destructed when the MemoryPool is destructed,
         * so no destructor is invoked in deallocate(). When the same data is
         * returned in allocate() it will still contain the last value.
         */
        MemoryPool(unsigned int startsize = 4, const T& initial_value = T() )
            :used_cap(0), alloc_cnt(startsize == 0 ? 1 : startsize), minit( initial_value )
        {
            this->make_pool( alloc_cnt );
        }

        /**
         * Release all memory from the pool.
         */
        ~MemoryPool()
        {
            // iterate over the whole pool and release all memory.
            for ( typename PoolType::iterator it = mpool.begin(); it != mpool.end(); ++it ) {
                delete[] it->second;
            }
            
        }

        /**
         * Returns the number of elements currently available.
         */
        size_type size() const {
            size_type res(0);
            // return the sum of all queued items.
            for ( typename PoolType::const_iterator it = mpool.begin(); it != mpool.end(); ++it )
                res += it->first->size();
            return res;
        }

        /**
         * Returns the maximum number of elements.
         */
        size_type capacity() const {
            return alloc_cnt;
        }

        /**
         * Reserve one additional element in the pool, allocating
         * extra memory if needed.
         */
        void reserve()
        {
            // instead of allocating individual elements, we 
            // allocate increasingly growing lock-free 'queues' filled with
            // memory blocks. used_cap and alloc_cnt track the actually used
            // and memory available in the pool.
            if ( ++used_cap > alloc_cnt ) {
                unsigned int growsize = mpool.back().first->capacity() * 2;
                alloc_cnt += growsize;

                this->make_pool( growsize );
            }
        }

        /**
         * Unreserve one additional element in the pool. The memory
         * is not physically freed, that only happens in the destructor.
         */
        void shrink()
        {
            --used_cap;
        }

        /**
         * Acquire and lock() a pointer to previously reserve()'d memory of type \a T.
         */
        pointer allocate()
        {
            void* result;
            // iterate over the whole pool and try to get a free slot.
            for ( typename PoolType::iterator it = mpool.begin(); it != mpool.end(); ++it ) {
                if ( it->first->dequeue( result ) ) {
                    oro_atomic_inc( &static_cast<Item*>(result)->rc);
                    return static_cast<pointer>( result );
                }
            }
            return 0;
        }

        /**
         * Increase the reference count of a piece of memory.
         * Returns false if already released.
         */
        bool lock(pointer m) {
            Item* it = reinterpret_cast<Item*>(m);
            if ( oro_atomic_read(&it->rc) == 0 )
                return false;
            oro_atomic_inc(&(it->rc) );
            return true;
        }

        /**
         * Decrease the reference count of a piece of memory.
         * Returns false if already released.
         */
        bool unlock( pointer m ) {
            return this->deallocate(m);
        }

        /**
         * Deallocate and unlock() a piece of memory.
         * Returns false if already released.
         */
        bool deallocate( pointer m )
        {
            Item* item = reinterpret_cast<Item*>(m);
            if ( oro_atomic_read(&item->rc) == 0 )
                return false;
            if( oro_atomic_dec_and_test( &(item->rc) ) ) {
                for ( typename PoolType::iterator it = mpool.begin(); it != mpool.end(); ++it ) {
                    if ( it->first->enqueue( static_cast<void*>(m) ) ) {
                        return true;
                    }
                }
                assert(false && "Deallocating more elements than allocated !");
            }
            return true;
        }

        /**
         * Returns how many times a piece of memory
         * is used.
         */
        size_type useCount( pointer m ) {
            return oro_atomic_read( &static_cast< Item* >(m)->rc );
        }
    };

    /**
     * A fixed size, lock-free Memory Pool with reference counted memory.
     *
     * @param T A class type for which memory will be allocated.
     * @see MemoryPool for a flexible size memory pool.
     */
    template<class T>
    class FixedSizeMemoryPool
    {
    public:
        typedef T* pointer;

        typedef unsigned int size_type;

    protected:
        /**
         * Adds a reference count.
         */
        struct Item {
            Item( const T& initial_value )
                : content(initial_value) {
                oro_atomic_set(&rc, 0);
            }
            Item()
                : content() {
                oro_atomic_set(&rc, 0);
            }
            // the order is important !
            T content;
            oro_atomic_t rc;
        };

        /**
         * For each additional pool, also store the location of the Item pointer.
         */
        void make_pool( size_type growsize )
        {
            mpit = new Item[growsize];
            for( unsigned int i = 0; i < growsize; ++i ) {
                mpit[i].content = minit;
                mpool.enqueue( static_cast<void*>(&mpit[i]) );
            }
        }
        
        /** The pool stores memory as void pointers */
        typedef AtomicQueue<void*> QueueType;

        QueueType mpool;
        T minit;
        Item* mpit;

    public:
        /**
         * Initialise the memory pool with \a fixed_size elements.
         * @param fixed_size the number of elements, must be at least 1.
         */
        FixedSizeMemoryPool(size_type fixed_size, const T& initial_value = T() )
            : mpool(fixed_size == 0 ? 1 : fixed_size), minit( initial_value ), mpit(0)
        {
            this->make_pool(fixed_size);
        }

        /**
         * Release all memory from the pool.
         */
        ~FixedSizeMemoryPool()
        {
            delete[] mpit;
        }

        /**
         * Returns the number of elements currently available.
         */
        size_type size() const {
            return mpool.size();
        }

        /**
         * Returns the maximum number of elements.
         */
        size_type capacity() const {
            return mpool.capacity();
        }

        /**
         * Acquire a pointer to memory of type \a T.
         */
        pointer allocate()
        {
            void* result;
            // iterate over the whole pool and try to get a free slot.
            if ( mpool.dequeue( result ) ) {
                Item* it = static_cast<Item*>(result);
                oro_atomic_inc( &(it->rc) );
                return (&it->content);
            }
            return 0;
        }

        /**
         * Increase the reference count of a piece of memory.
         * Returns false if already released.
         */
        bool lock(pointer m) {
            Item* it = reinterpret_cast<Item*>(m);
            if ( oro_atomic_read(&it->rc) == 0 )
                return false;
            oro_atomic_inc(&(it->rc) );
            return true;
        }

        /**
         * Decrease the reference count of a piece of memory.
         * Returns false if already released.
         */
        bool unlock( pointer m ) {
            return this->deallocate(m);
        }

        /**
         * Decrease the reference count of a piece of memory \a m, 
         * which becomes available for allocation.
         */
        bool deallocate( pointer m )
        {
            Item* it = reinterpret_cast<Item*>(m);
            if ( oro_atomic_read(&it->rc) == 0 )
                return false;
            if( oro_atomic_dec_and_test( &(it->rc) ) )
                if ( mpool.enqueue( static_cast<void*>(m) ) == false )
                    assert(false && "Deallocating more elements than allocated !");
            return true;
        }
    };

}

#endif
