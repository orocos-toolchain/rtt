/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:40:06 CEST 2002  rtstl.hpp 

                       rtstl.hpp -  description
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

#ifndef RTSTL_HPP
#define RTSTL_HPP

#ifdef HAVE_CONFIG_H

#include <config.h>
#include <os/fosi.h>
#include <os/cpp.hpp>
#ifdef _IOSTREAM_H
#error please remove any includes of iostream \
before using rtstl.hpp, we include our own iostream
#endif

#else

#define USER_SGI_STL

#endif // HAVE_CONFIG_H

// current gets somewhere defined as get_current
#undef current 
/*************************************************************
 *                      CUSTOM STL
 ************************************************************/
#ifdef RTL_CUSTOM_STL

#   warning Incomplete vector implementation !
#   include "vector_cd.hpp"

/*************************************************************
 *                   REALTIME SGI STL
 ************************************************************/
#elif defined RTL_SGI_STL

// don't use the clib pthread types
#ifdef USE_RTAI
#   define _PTHREAD_H 1
//#   define _BITS_PTHREADTYPES_H // now done in fosi.h
#endif 
// LIAS needed this or error in stl_alloc.h
// because PTHREAD_MUTEX_INITIALIZER was not known at line 581
#ifdef USE_RTLINUX
#   define _NOTHREADS
#endif

// don't use the clib limits
#   define _LIMITS_H___ 1

// but we need this one XXX replace by kernel macro
#   define CHAR_BIT 8

// do not throw exceptions
#   undef __EXCEPTIONS

// use our own new (see cpp.h)
#   define __NEW__

#   define _MALOC_H
#   define _STDLIB_H


extern "C"
{
    // XXX the name mangled malloc and free
    void* malloc( size_t __size );
    void free( void * p );
}

// remove __THROW_BAD_ALLOC from inclusion
#   define __THROW_BAD_ALLOC

//#define _SYS_CDEFS_H    1 //defines the throw() thing that causes trouble
extern "C"
{
    // needed for memmove
#include "linux/string.h"
}

/**
 * memcpy and memset was #defined as '?' in some kernel header file which gives a nasty parsing error.
 * (prob. inline macros)
 */

// was not defined for GCC 3.2
#define _STRING_H   1
#define _CPP_CSTRING 1
#   include <new>
//#undef memcpy
//#undef memset
//#define memcmp
#define _CPP_CSTDLIB // exclude cstdlib file
#define _CPP_CSTDIO // exclude cstdio file
#define memcmp __builtin_memcmp
#include <stdio.h>
#   include <vector>
#   include <algorithm>
#   include <map>
#   include <queue>
#   include <string>
#   include <list>

// just checking...
#   ifdef __EXCEPTIONS
#      error __EXCEPTIONS not turned off !
#   endif

/************************************************************
*                      STL PORT
************************************************************/
#elif defined RTL_STLPORT_STL 
// XXX NOT TESTED XXX
#   define _BITS_PTHREADTYPES_H 1 
//#   define _LIBC_LIMITS_H_ 1
#   define _LIMITS_H___ 1
#   define CHAR_BIT 8 
//#   define _GCC_LIMITS_H_ 1
#   define STLP_NO_EXCEPTIONS
#   include <vector>
#   include <algorithm>


/*************************************************************
*                      USERSPACE SGI STL
************************************************************/
#elif defined USER_SGI_STL

#   include <string>
#   include <vector>
#   include <algorithm>
#   include <map>
#   include <queue>
#   include <list>
#   include <iostream>
#   include <sstream>
#endif

//using namespace std;
//using namespace ORO_OS::std;

#endif
