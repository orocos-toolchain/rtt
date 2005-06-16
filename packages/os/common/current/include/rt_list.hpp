#ifndef ORO_OS_RT_LIST_HPP
#define ORO_OS_RT_LIST_HPP

#include <list>
#include "oro_allocator.hpp"

namespace ORO_OS
{
	
    /**
     * Implements a list with real-time insertion/removal
     * of elements. All std::list<T> functions remain
     * available, but use rt_ functions to grow or shrink
     * a memory pool of list items.
     * @param T The value stored in the list.
     * @param Alloc The allocator which effectivily ( in the end ) allocates the memory.
     */
	template<class T, class Alloc = std::allocator<T> >
	struct rt_list
        : public std::list<T, local_allocator<T,Alloc> >
    {
    protected:
        typedef typename std::list<T, local_allocator<T,Alloc> >::_Node ItemType;
    public:
        ~rt_list() {
            rt_shrink(1); // cleanup if possible.
            // this prevents leaking the node allocated
            // by list<>.
        }
        /**
         * Grow memory pool for list elements. 
         * @throw bad_alloc if no more memory available.
         */
        void rt_grow(std::size_t items)
        {
            local_allocator<ItemType> la;
            la.grow( items );
        }

        /**
         * Shrink memory pool.
         */
        void rt_shrink(std::size_t items)
        {
            local_allocator<ItemType> la;
            la.shrink( items );
        }
    };


}

#endif
