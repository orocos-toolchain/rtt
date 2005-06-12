#ifndef ORO_ALOCATOR_HPP
#define ORO_ALOCATOR_HPP

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <allocator>
#include <multimap>

namespace ORO_OS
{

    /**
     * A simple local allocator which
     * keeps a small pool which you can grow manually
     * with \a grow(). It delegates allocation to the
     * \a Alloc for each allocation request,
     * also if the pool is empty. If you copy this allocator,
     * a completely new instance is created with an empty pool.
     * The pool is deallocated upon destruction.
     *
     * @param T the type to allocate memory for
     * @param Alloc the allocator to delegate allocation to.
     */
    template <class T, class Alloc = std::allocator<T> >
    class local_allocator
    {
    public:
        typedef Alloc::value_type      value_type;
        typedef Alloc::pointer         pointer;
        typedef Alloc::const_pointer   const_pointer;
        typedef Alloc::reference       reference;
        typedef Alloc::const_reference const_reference;
        typedef Alloc::size_type       size_type;
        typedef Alloc::difference_type difference_type;
        //...
    public:
        pointer address(reference x) const { 
            return Alloc::address(x); 
        }
        
        const_pointer address(const_reference x) const { 
            return Alloc::address(x);
        }
    public:
        pointer allocate(size_type n, const_pointer hint = 0) {
            pointer ret = 0;
            if (n == 0)
                return ret;
            // if present in pool, return pool item
            std::pair<pool_it,pool_it> r = pool.equal_range( n );
            while ( r.first != r.second && r.first.second == 0  )
                ++r.first;
            // if found, return item :
            if ( r.first != r.second ) {
                ret = r.first.second;
                r.first.second = 0; // clear out
                return ret;
            }

            // fallback on allocator...
            // ret is still zero here
            pool.insert( pool_type::value_type(n,ret) );     // store free location.
            ret = this->_grow(n, hint);
            
            return ret;
        }

        void deallocate(pointer p, size_type n) {

            std::pair<pool_it,pool_it> r = pool.equal_range( n );
            while ( r.first != r.second && r.first.second != 0  )
                ++r.first;
            // if found, store item :
            if ( r.first != r.second ) {
                r.first.second = p;
                return;
            }
            // if not found, we did not allocate it !
            assert( false && "Attempt to deallocate pointer which was not allocated !");
        }

        size_type max_size() const { 
            return Alloc::max_size();
        }

        void construct(pointer p, const value_type& x) { 
            Alloc::construct(p, x);
        }

        void destroy(pointer p) { Alloc::destroy(p); }

        /**
         * Grow local pool with room for at least \a n additional items.
         */
        void grow(size_type n, const_pointer hint = 0) {
            pool.insert( pool_type::value_type( n, this->_grow(n, hint) ) );     // store mem location.
        }
    public:
        local_allocator() {}
        local_allocator(const local_allocator&) {}
        ~local_allocator() {
            // free all local memory.
            std::for_each( pool.begin(), pool.end(),
                           lambda::bind( &Alloc::deallocate, std::select1st(lambda::_1), std::select2nd(lambda::_1)));
        }
        template <class U, class A> 
        local_allocator(const local_allocator<U,A>&) {}

        template <class U>
        struct rebind { typedef local_allocator<U,Alloc> other; };
    private:
        /**
         * Allocate for at least \a n additional items.
         */
        pointer _grow(size_type n, const_pointer hint = 0) {
            if (hint == 0 && !pool.empty() )
                hint = pool.back().second; // use last element as hint.
            return Alloc::allocate( n, hint );
        }

        void operator=(const local_allocator&);

        // the pool stores block-size/pointer pairs. Also uses Alloc for allocation.
        typedef std::hash_multimap< size_t, pointer, std::hash<size_t>, std::equal_to<size_t>, Alloc::rebind<void>::other > pool_type;
        typedef pool_type::iterator       pool_it;
        typedef pool_type::const_iterator pool_cit;
        // stores blocks -> memory map for allocated memory.
        pool_type pool;
    };

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
    inline bool operator==(const local_allocator<T,A>& a1, 
                           const local_allocator<T,A>& a2) {
        return &a1 == &a2;
    }
    
    template <class T, class A>
    inline bool operator!=(const local_allocator<T,A>& a1, 
                           const local_allocator<T,A>& a2) {
        return &a1 != &a2;
    }

    template<class Alloc >
    class local_allocator<void, Alloc = std::allocator<void> >
    {
        typedef Alloc::value_type    value_type;
        typedef Alloc::pointer       pointer;
        typedef Alloc::const_pointer const_pointer;
        
        template <class U> 
        struct rebind { typedef local_allocator<U,Alloc> other; };
    }

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

}

#endif
