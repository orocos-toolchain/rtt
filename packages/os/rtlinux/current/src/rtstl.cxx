/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:40:03 CEST 2002  rtstl.cpp 

                       rtstl.cpp -  description
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
 *   Include our RealTime STL Header files
 */

#ifndef RTSTL_CPP
#define RTSTL_CPP

#include "corelib/rtstl.hpp"

// define the placement new as a weak global (not inline) symbol
#define WEAK(x) \
  x __attribute__ ((weak)); \
  x

#ifdef RTL_CUSTOM_STL


#elif defined RTL_SGI_STL


#ifdef USE_RTAI

extern "C"
{
    // XXX name mangled malloc and free
    void* malloc( size_t __size )
    {
        return rt_malloc( __size );
    }

    void free( void * p )
    {
        rt_free( p );
    }
}

#endif
#ifdef USE_RTLINUX

extern "C"
{
    // XXX name mangled malloc and free
    void* malloc( size_t __size )
    {
        return kmalloc( __size, 0 );
    }

    void free( void * p )
    {
        kfree( p );
    }
}

#endif

void __assert_fail ( __const char *__assertion, __const char *__file, unsigned int __line, __const char *__function )
{
    // You are expected not to return from this function
}


#elif defined RTL_STLPORT_STL


#endif

#endif
