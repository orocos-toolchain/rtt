/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:39:59 CEST 2002  allocator.hpp 

                       allocator.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


/**
 * An allocator written for embedded systems will allow us to use STL and boost
 * in the kernel and realtime environments.
 * 
 * See "The C++ Programming Language" p 567 by B. Stroustrup
 * 
 * TODO : *Memory segment allignment
 *
 * @memo We only have one Pool per allocator<T> (static)
 */


template <class T>

class embeddedAllocator
{
        static embeddedPool* mem;

    public:
        typedef T value_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef T* pointer;
        typedef const T* const_pointer;

        typedef T& reference;
        typedef const T& const_reference;

        pointer address( reference r ) const
        {
            return & r;
        }

        const_pointer address( const_reference r ) const
        {
            return & r;
        }

        embeddedAllocator() THROW
    :
        mem( embeddedPool::Instance() )
        {}

        embeddedAllocator( size_type allocHint ) THROW
    :
        mem( embeddedPool::Instance() )
        {}

        template <class U>
        allocator( const allocator<U>& ) THROW
        {}


        ~allocator() THROW;

        pointer allocate( size_type n, allocator<void>::const_pointer hint = 0 );

        void deallocate( pointer p, size_type n );

        void construct( pointer p, const T& val )
        {
            new( p ) T( val );
        }

        void destroy( pointer p )
        {
            p->~T();
        }

        size_type max_size() const THROW;

        template <class U>

        struct rebind
        {
            typedef embeddedAllocator<U> other;
        };
};

template <class T>
bool operator==( const allocator<T>&, const allocator<T>& ) THROW;

template <class T>
bool operator!=( const allocator<T>&, const allocator<T>& ) THROW;
