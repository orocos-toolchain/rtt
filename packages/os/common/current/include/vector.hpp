/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:40:11 CEST 2002  vector.hpp 

                       vector.hpp -  description
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


// Real-time vector implementation
// by Christophe.Devriese <Christophe.Devriese@student.kuleuven.ac.be>
//
// you give the maximum amount of members at initialization and thou shallt NOT go over it


template <class _Container>

class iterator
{
        typedef _Container& _Reference;
        typedef _Container* pointer;
        typedef iterator _Self;

    protected:
        _Container* current;

    public:
        iterator()
        {}

        ;
        iterator( _Container* c )
        {
            current = c;
        }

        pointer base()
        {
            return current;
        }

        pointer operator&()
        {
            return current;
        }

        _Reference operator*()
        {
            return * current;
        }

        pointer operator->() const
        {
            return & ( operator*() );
        }

        _Self& operator++()
        {
            current++;
            return *this;
        }

        _Self operator++( int )
        {
            _Self __tmp = *this;
            current++;
            return __tmp;
        }

        _Self& operator--()
        {
            current--;
            return *this;
        }

        _Self operator--( int )
        {
            _Self __tmp = *this;
            current++;
            return __tmp;
        }

        bool operator==( const iterator<_Container>& __x )
        {
            return __x.base() == base();
        }

        //_Self& operator=(_Container c) { current = &c; return *this; }
};


template <class T>

class vector
{

    public:
        typedef iterator<T> Iterator;
        vector( int max_elem )
        {
            data = new T[ max_elem ];
            ptr = data;
            max = data + max_elem;
        }

        void push_back( T t )
        {
            if ( ptr < max )
                * ptr = t;

            ptr++;
        }

        void erase( iterator<T> een, iterator<T> twee )
        {
            iterator<T> tus = twee; //non deep copy

            while ( een != tus )
            {
                ( *een ) ++ = ( *twee ) ++;
            }

            ptr = &een;
        }

        iterator<T> find( T t )
        {
            iterator<T> tus = begin();

            while ( tus != ptr )
                if ( tus++ == t )
                    return ( --tus );
        }

        iterator<T> begin()
        {
            iterator<T> itl( data );
            return itl;
        }

        iterator<T> end()
        {
            iterator<T> itl( ptr ) ;
            return itl;
        }

        /**
         * Pointer to first vector position
         */
        T* data;
        /**
         * Pointer to last element position
         */
        T* ptr;
        /**
         * Pointer to last vector position
         */
        T* max;
};


