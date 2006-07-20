/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  rt_list.hpp 

                        rt_list.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#ifndef OS_RT_LIST_HPP
#define OS_RT_LIST_HPP

#include <list>
#include "oro_allocator.hpp"

namespace OS
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
