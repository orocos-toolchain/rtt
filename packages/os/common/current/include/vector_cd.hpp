/***************************************************************************
 tag: Christophe Devriese  Mon Jun 10 14:40:51 CEST 2002  vector_cd.hpp 

                       vector_cd.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Christophe Devriese
   email                : christophe.devriese@student.kuleuven.ac.be

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

// The new and delete are not overloaded here. If needed for the realtime kernel, do so
// in the cpp file that includes this one.  This file must stay maximal portable
//
#ifndef VECTOR_H
#define VECTOR_H

#include <os/fosi.h>

template <class T>

class vector
{

    public:
        typedef T* iterator;
        vector()
        {
            data = new T[ 32 ]; // arbitrary
            ptr = data;
            max = data + 32; // one beyond our array
            rtos_printf( "Vector Constructed, size 32; address: %x\n", data );
        }

        ~vector()
        {
            rtos_printf( "Vector Destructed, size %d; contents: %di, address %x\n", capacity(), size(), data );
            delete[] data;
        }

        void push_back( T t )
        {
            if ( ptr < max )
                * ptr = t;

            ++ptr;

            rtos_printf( "Vector push_back,           contents: %d\n", size() );
        }

        void erase( iterator een, iterator twee )
        {
            //        iterator tus = ptr;
            //        copy the empty endpoint too ? NO!

            if ( een != twee )
            {
                while ( twee != ptr )
                {
                    *( een++ ) = *( twee++ );
                }

                ptr = een;
            }
        }

        void erase( iterator een )
        {
            erase( een, een + 1 );
        }

        /**
         * This will destroy your current vector
         */
        void reserve( size_t max_elem )
        {
            rtos_printf( "resizing %x\n", data );
            delete[] data;
            data = new T[ max_elem ];
            ptr = data;
            max = data + max_elem;
            rtos_printf( "Vector reserve,size %d; address: %x\n", capacity(), data );
        }

        void insert( iterator start, T t )
        {
            if ( ptr < max )
            {
                iterator tus = ptr;

                while ( tus != start )
                    * ( tus-- ) = *( tus - 1 );

                *( start ) = t;

                ptr++;
            }
        }

        size_t size()
        {
            return ptr -data;
        }

        size_t capacity()
        {
            return max -data;
        }

        /**
         * Returns the first position of the vector
         */
        iterator begin()
        {
            return data;
        }

        /**
         * Returns the first empty position of the vector
         */
        iterator end()
        {
            return ptr;
        }

        T &operator[] ( int i )
        {
            return data[ i ];
        }

        /**
         * Pointer to first vector position
         */
        T* data;
        /**
         * Pointer to last element position
         */
        iterator ptr;
        /**
         * Pointer to last vector position
         */
        iterator max;
};

/**
 * Returns an iterator to element t if it was found between
 * een and twee.  Otherwise, returns end()
 */
template <class T>
vector<T>::iterator find( vector<T>::iterator een, vector<T>::iterator twee, const T &t )
{
    vector<T>::iterator tus = een;

    while ( tus != twee )
        if ( *( tus++ ) == t )
            return ( --tus );

    return twee;
}

#endif
