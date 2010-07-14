/*
 * TsPool.hpp
 *
 *  Created on: Jul 10, 2010
 *      Author: Butch Slayer
 */

#ifndef RTT_TSPOOL_HPP_
#define RTT_TSPOOL_HPP_

#include "../os/CAS.hpp"
#include <assert.h>

namespace RTT
{
    namespace internal
    {

        /**
         * A multi-reader multi-writer MemoryPool implementation.
         * It can hold max 65535 elements of type T.
         */
        template<typename T>
        class TsPool
        {
        public:
            typedef T value_t;
        private:
            union Pointer_t
            {
                unsigned int value;
                struct _ptr_type
                {
                    unsigned short tag;
                    unsigned short index;
                } ptr;
            };

            /**
             * The implementation assumes that value
             * is the first element of this struct
             * and that there are no virtual functions in this class.
             */
            struct Item
            {
                value_t value;
                volatile Pointer_t next;

                Item() :
                    value(value_t())
                {
                    next.value = 0;
                }
            };

            Item* pool;
            Item head;

            unsigned int pool_size, pool_capacity;
        public:

            typedef unsigned int size_type;
            /**
             * Creates a fixed size memory pool holding \a ssize
             * blocks of memory that can hold an object of class \a T.
             */
            TsPool(unsigned int ssize, const T& sample = T()) :
                pool_size(0), pool_capacity(ssize)
            {
                pool = new Item[ssize];
                data_sample( sample );
            }

            ~TsPool()
            {
#ifndef NDEBUG
                /*Check pool consistency.*/
                unsigned int i = 0, endseen = 0;
                for (; i < pool_capacity; i++)
                {
                    if (pool[i].next.ptr.index == (unsigned short) -1)
                    {
                        ++endseen;
                    }
                }
                assert( endseen == 1);
                assert( size() == pool_capacity && "TsPool: not all pieces were deallocated !" );
#endif
                delete[] pool;
            }

            /**
             * Clears all internal management data of this Memory Pool.
             * All data blobs are considered to be owned by the pool
             * again.
             * @nts
             * @rt
             */
            void clear()
            {
                for (unsigned int i = 0; i < pool_capacity; i++)
                {
                    pool[i].next.ptr.index = i + 1;
                }
                pool[pool_capacity - 1].next.ptr.index = (unsigned short) -1;
                head.next.ptr.index = 0;
            }

            /**
             * Initializes every element of the pool with the given sample
             * and clears the pool.
             * @nts
             * @nrt
             */
            void data_sample(const T& sample) {
                for (unsigned int i = 0; i < pool_capacity; i++)
                    pool[i].value = sample;
                clear();
            }

            value_t* allocate()
            {
                volatile Pointer_t oldval;
                volatile Pointer_t newval;
                Item* item;
                do
                {
                    oldval.value = head.next.value;
                    //List empty?
                    if (oldval.ptr.index == (unsigned short) -1)
                    {
                        return 0;
                    }
                    item = &pool[oldval.ptr.index];
                    newval.ptr.index = item->next.ptr.index;
                    newval.ptr.tag = oldval.ptr.tag + 1;
                } while (!os::CAS(&head.next.value, oldval.value, newval.value));
                return &item->value;
            }

            bool deallocate(T* Value)
            {
                if (Value == 0)
                {
                    return false;
                }
                assert(Value >= (T*) &pool[0] && Value <= (T*) &pool[pool_capacity]);
                volatile Pointer_t oldval;
                Pointer_t head_next;
                Item* item = reinterpret_cast<Item*> (Value);
                do
                {
                    oldval.value = head.next.value;
                    item->next.value = oldval.value;
                    head_next.ptr.index = (item - pool);
                    head_next.ptr.tag = oldval.ptr.tag + 1;
                } while (!os::CAS(&head.next.value, oldval.value, head_next.value));
                return true;
            }

            /**
             * Return the number of elements that are available to be allocated.
             * This function is not thread-safe and should not be used when concurrent
             * allocate()/deallocate() functions are running.
             * @return the number of elements left to allocate.
             */
            unsigned int size()
            {
                unsigned int ret = 0;
                volatile Item* oldval;
                oldval = &head;
                while ( oldval->next.ptr.index != (unsigned short) -1) {
                    ++ret;
                    oldval = &pool[oldval->next.ptr.index];
                    assert(ret <= pool_capacity); // abort on corruption due to concurrency.
                }
                return ret;
            }

            /**
             * The maximum number of elements available for allocation.
             * @return The maximum size.
             */
            unsigned int capacity()
            {
                return pool_capacity;
            }

        private:

        };
    }
}

#endif /* RTT_TSPOOL_HPP_ */
