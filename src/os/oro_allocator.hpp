/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  oro_allocator.hpp

                        oro_allocator.hpp -  description
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


#ifndef ORO_ALOCATOR_HPP
#define ORO_ALOCATOR_HPP

#include <memory>
#include <algorithm>
#include <utility>
#include <functional>
#include <iostream>
#include <typeinfo>

//#define ORO_USE_SGI_EXT
#ifdef ORO_USE_SGI_EXT
#include <hash_map>
#else
#include <map>
#endif

#include "MutexLock.hpp"

namespace RTT { namespace os {
    /**
     * A simple local allocator which
     * keeps a small pool which you can grow manually
     * with \a grow(). It delegates allocation to the
     * \a Alloc for each allocation request,
     * also if the pool is empty. The pool works on a per-type
     * basis globally (static !). The pool is thus _not_ per-object.
     * A per-object allocator is allowed by the standard.
     * The pool is deallocated upon global destruction.
     *
     * @param T the type to allocate memory for
     * @param Alloc the allocator to delegate allocation to.
     */
    template <class T, class Alloc = std::allocator<T> >
    class local_allocator
    {
    public:
        typedef typename Alloc::value_type      value_type;
        typedef typename Alloc::pointer         pointer;
        typedef typename Alloc::const_pointer   const_pointer;
        typedef typename Alloc::reference       reference;
        typedef typename Alloc::const_reference const_reference;
        typedef typename Alloc::size_type       size_type;
        typedef typename Alloc::difference_type difference_type;
        //...
    public:
        pointer address(reference x) const {
            return Alloc().address(x);
        }

        const_pointer address(const_reference x) const {
            return Alloc().address(x);
        }
    public:
        pointer allocate(size_type n, const_pointer hint = 0) {
            pointer ret = 0;
            if (n == 0)
                return ret;
            MutexLock lock( pool_lock );
            // if present in pool, return pool item
            std::pair<pool_it,pool_it> r = pool.equal_range( n );
            while ( r.first != r.second && r.first->second == 0  )
                ++r.first;
            // if found, return item :
            if ( r.first != r.second ) {
                ret = r.first->second;
                r.first->second = 0; // clear out
                //std::cerr << "Using grown: "<< ret<<" of size "<<n<<" in "<<typeid(ret).name()<<std::endl;
                return ret;
            }

            // fallback on allocator...
            // ret is still zero here
            pool.insert( typename pool_type::value_type(n,ret) );     // store free location.
            ret = this->_grow(n, hint);
            //std::cerr << "Allocated ungrown: "<< ret<<" of size "<<n<<" in "<<typeid(ret).name()<<std::endl;

            return ret;
        }

        void deallocate(pointer p, size_type n) {
            MutexLock lock( pool_lock );
            std::pair<pool_it,pool_it> r = pool.equal_range( n );
//             if ( find( r.first, r.second, typename pool_type::value_type(n,p) ) != r.second )
//                 assert(false && "Do not deallocate twice !");
            while ( r.first != r.second && r.first->second != 0  )
                ++r.first;
            // if found, store item :
            if ( r.first != r.second ) {
                r.first->second = p;
                //std::cerr << "Storing grown: "<< p <<" of size "<<n<<" in "<<typeid(p).name()<<std::endl;
                return;
            }
            // if not found, we did not allocate it !
            //std::cerr << "Deallocating ungrown: "<<p << " of size "<<n<<" in "<<typeid(p).name()<<std::endl;
            this->_shrink(p,n);
        }

        size_type max_size() const {
            return Alloc().max_size();
        }

        void construct(pointer p, const value_type& x) {
            Alloc().construct(p, x);
        }

        void destroy(pointer p) { Alloc().destroy(p); }

        /**
         * Grow local pool with room for at least \a n additional items.
         */
        void grow(size_type n, const_pointer hint = 0) {
            MutexLock lock( pool_lock );
            pointer ret = this->_grow(n, hint);
            pool.insert( typename pool_type::value_type( n,ret ) );     // store mem location.
            //std::cerr << "Added   : "<< ret<<" of size "<<n<<" in "<<typeid(ret).name()<<std::endl;
        }

        /**
         * Shrink local pool with \a n items.
         */
        void shrink(size_type n) {
            if (n == 0)
                return;
            MutexLock lock( pool_lock );
            std::pair<pool_it,pool_it> r = pool.equal_range( n );
            while ( r.first != r.second && r.first->second == 0  )
                ++r.first;
            if ( r.first != r.second ) {
                pointer t = r.first->second;
                pool.erase( r.first ); // erase unused slot.
                //std::cerr << "Removed: "<<t << " of size "<<n<<" in "<<typeid(t).name()<<std::endl;
                _shrink(t,n);
                return;
            }
            //std::cerr << "Unable to remove "<<n<<std::endl;
        }
    public:
        local_allocator() {}
        local_allocator(const local_allocator&) {}
        ~local_allocator() {}
        template <class U, class A>
        local_allocator(const local_allocator<U,A>&) {}

        template <class U>
        struct rebind { typedef local_allocator<U, typename Alloc::template rebind<U>::other > other; };
    private:
        Mutex pool_lock;
        /**
         * Allocate for at least \a n additional items.
         */
        pointer _grow(size_type n, const_pointer hint = 0) {
            return Alloc().allocate( n, hint );
        }

        void _shrink( pointer t, size_type n) {
            Alloc().deallocate( t, n);
        }

        void operator=(const local_allocator&);

        // the pool stores block-size/pointer pairs. Also uses Alloc for allocation.
#ifdef ORO_USE_SGI_EXT
        // hash_multimap is non-standard C++. use std::multimap instead...
        typedef std::hash_multimap< size_t, pointer> pool_type;
#else
        typedef std::multimap< size_t, pointer> pool_type;
#endif
        typedef typename pool_type::iterator       pool_it;
        typedef typename pool_type::const_iterator pool_cit;
        // stores blocks -> memory map for allocated memory.
        struct pool_wrapper_type : public pool_type {
            ~pool_wrapper_type() {
                // free all memory.
                typename pool_type::iterator it = this->begin();
                for (; it != this->end(); ++it ) {
                    Alloc().deallocate(it->second, it->first );
                }
            }
        };
        static pool_wrapper_type pool;

    };

    template< class T, class A>
    typename local_allocator<T,A>::pool_wrapper_type local_allocator<T,A>::pool;

//     template< class T, class A>
//     Mutex local_allocator<T,A>::pool_lock;

    template <class T, class A, class A2>
    inline bool operator==(const local_allocator<T,A>& ,
                           const local_allocator<T,A2>& ) {
        return false;
    }

    template <class T, class A, class A2>
    inline bool operator!=(const local_allocator<T,A>& ,
                           const local_allocator<T,A2>& ) {
        return true;
    }

    template <class T, class A>
    inline bool operator==(const local_allocator<T,A>& ,
                           const local_allocator<T,A>& ) {
        return true;;
    }

    template <class T, class A>
    inline bool operator!=(const local_allocator<T,A>&,
                           const local_allocator<T,A>&) {
        return false;
    }

    template<>
    class local_allocator<void>
    {
        typedef void    value_type;
        typedef void*       pointer;
        typedef const void* const_pointer;

        template <class U>
        struct rebind { typedef local_allocator<U> other; };
    };

#if 0
    // use the std::malloc_alloc class !

    /**
     * A simple malloc based allocator which allocates
     * every block with malloc() and deallocates with free().
     */
    template <class T> class malloc_allocator
    {
    public:
        typedef T                 value_type;
        typedef value_type*       pointer;
        typedef const value_type* const_pointer;
        typedef value_type&       reference;
        typedef const value_type& const_reference;
        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;
        //...
    public:
        pointer address(reference x) const {
            return &x;
        }

        const_pointer address(const_reference x) const {
            return &x;
        }
    public:
        pointer allocate(size_type n, const_pointer = 0) {
            void* p = std::malloc(n * sizeof(T));
            if (!p)
                throw std::bad_alloc();
            return static_cast<pointer>(p);
        }

        void deallocate(pointer p, size_type) {
            std::free(p);
        }

        size_type max_size() const {
            return static_cast<size_type>(-1) / sizeof(value_type);
        }

        void construct(pointer p, const value_type& x) {
            new(p) value_type(x);
        }

        void destroy(pointer p) { p->~value_type(); }

    public:
        malloc_allocator() {}
        malloc_allocator(const malloc_allocator&) {}
        ~malloc_allocator() {}
        template <class U>
        malloc_allocator(const malloc_allocator<U>&) {}

        template <class U>
        struct rebind { typedef malloc_allocator<U> other; };
    private:
        void operator=(const malloc_allocator&);
    };

    template <class T>
    inline bool operator==(const malloc_allocator<T>&,
                           const malloc_allocator<T>&) {
        return true;
    }

    template <class T>
    inline bool operator!=(const malloc_allocator<T>&,
                           const malloc_allocator<T>&) {
        return false;
    }

    template<> class malloc_allocator<void>
    {
        typedef void        value_type;
        typedef void*       pointer;
        typedef const void* const_pointer;

        template <class U>
        struct rebind { typedef malloc_allocator<U> other; };
    }
#endif

}}

#endif
