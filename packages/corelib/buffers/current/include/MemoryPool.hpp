#ifndef ORO_MEMORY_POOL_HPP
#define ORO_MEMORY_POOL_HPP

#include "AtomicQueue.hpp"
#include <vector>
#include <boost/shared_ptr.hpp>

namespace ORO_CoreLib
{

    /**
     * A memory pool in which allocate() and deallocate() are lock-free.
     * Use reserve() or shrink() to reserve or remove the reservation of
     * memory for a type \a T. It returns in allocate objects of type \a T*.
     * Any number of concurrent threads may invoke allocate() and deallocate(),
     * however, reserve() and shrink() may not be concurrently invoked.
     * @param T A class type for which memory will be allocated.
     * @note Do not use this implementation, there is far better memory
     * management built-in into BufferLockFree and ListLockFree.
     */
    template<class T>
    class MemoryPool
    {
        /** The pool stores memory as void pointers */
        typedef AtomicQueue<void*> QueueType;
        typedef boost::shared_ptr<QueueType> QueueTypePtr;
        typedef std::vector<QueueTypePtr> PoolType;

        unsigned int used_cap, alloc_cnt;
        PoolType mpool;
    public:
        typedef T* pointer;

        /**
         * Initialise the memory pool and already allocate some
         * memory.
         */
        MemoryPool(unsigned int startsize = 4)
            :used_cap(0), alloc_cnt(startsize)
        {
            mpool.push_back( QueueTypePtr(new QueueType( startsize )) );
            for( unsigned int i = 0; i < startsize; ++i )
                mpool.back()->enqueue( malloc( sizeof(T) ) );
        }

        /**
         * Release all memory from the pool.
         */
        ~MemoryPool()
        {
            void* result;
            // iterate over the whole pool and release all memory.
            for ( typename PoolType::iterator it = mpool.begin(); it != mpool.end(); ++it ) {
                while ( (*it)->dequeue( result ) ) {
                    free( result );
                }
            }
            
        }

        /**
         * Reserve one additional element in the pool, allocating extra
         * memory if needed.
         */
        void reserve()
        {
            // instead of allocating individual elements, we 
            // allocate increasingly growing lock-free 'queues' filled with
            // memory blocks. used_cap and alloc_cnt track the actually used
            // and memory available in the pool.
            if ( ++used_cap > alloc_cnt ) {
                unsigned int growsize = mpool.back()->capacity() * 2;
                alloc_cnt += growsize;
                mpool.push_back( QueueTypePtr(new QueueType( growsize )) );
                for( unsigned int i = 0; i < growsize; ++i )
                    mpool.back()->enqueue( malloc( sizeof(T) ) );
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
         * Acquire a pointer to previously reserve()'d memory of type \a T.
         */
        pointer allocate()
        {
            void* result;
            // iterate over the whole pool and try to get a free slot.
            for ( typename PoolType::iterator it = mpool.begin(); it != mpool.end(); ++it ) {
                if ( (*it)->dequeue( result ) ) {
                    return static_cast<pointer>(result);
                }
            }
            return 0;
        }

        /**
         * Give back a piece of memory \a m, which becomes available for allocation again.
         */
        void deallocate( pointer m )
        {
            for ( typename PoolType::iterator it = mpool.begin(); it != mpool.end(); ++it ) {
                if ( (*it)->enqueue( static_cast<void*>(m) ) ) {
                    return;
                }
            }
            assert(false && "Deallocating more elements than allocated !");
        }
    };


}

#endif
