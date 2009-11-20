/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:06 CEST 2009  rt_list.hpp

                        rt_list.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


#ifndef ORO_OS_RT_LIST_HPP
#define ORO_OS_RT_LIST_HPP

#include <list>
#include "oro_allocator.hpp"

namespace RTT { namespace os {

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


}}

#endif
