/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:44:27 CEST 2002  cpp.cpp 

                       cpp.cpp -  description
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




#include "corelib/cpp.hpp"

#define WEAK(x) \
  x __attribute__ ((weak)); \
  x 
/**
 * The placement new operator, creates an object on location
 * pointed at by p
 * Must be weak and not inline
 */
/*
WEAK(void* operator new(size_t sz, void* p ))
{
   return p;
}*/

/**
 * Override the builtin new and delete operators
 */

void *operator new ( unsigned __size )
{
    void* ptr = kmalloc( __size, 0 );

    if ( ptr == 0 )
    {
        rtl_printf( "\nNULL POINTER FROM KMALLOC : EXPECT CRASH\n" );
    }

    return ptr;
}

void *operator new[] ( unsigned __size )
{
    return kmalloc( __size, 0 );
}

void operator delete ( void *__p )
{
    if ( __p == 0 )
    {
        rtl_printf( "\n TRIED TO DELETE NULL POINTER\n" );
        return ;
    }

    kfree( __p );
}

void operator delete[] ( void *__p )
{
    kfree( __p );
}


extern "C" void __pure_virtual()
{
}

extern "C" void __do_global_ctors_aux();
extern "C" void __do_global_dtors_aux();

extern "C" void __do_global_dtors()
{
	__do_global_dtors_aux();
}

extern "C" void __do_global_ctors()
{
	__do_global_ctors_aux();
}
