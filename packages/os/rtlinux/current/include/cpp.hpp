/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:44:30 CEST 2002  cpp.hpp 

                       cpp.hpp -  description
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


/*
 * rtlcpp.h - C++ support for RTL
 * Michael Barabanov, <baraban@fsmlabs.com>
 *
 * Ideas and code from David Olofson, Yunho Jeon and myself
 */

#ifndef __CPP_H__
#define __CPP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define new _new
#define virtual _virtual
#ifndef NULL
#define NULL 0
#endif
#include <linux/kernel.h>
#include <linux/module.h>
#include <rtl_sched.h>
#include <rtl_fifo.h>
#include <rtl_time.h>
#include <rtl_debug.h>
#include <pthread.h>

#undef new
#undef virtual
#ifdef __cplusplus
}

#endif

extern "C"
{

    void *kmalloc( unsigned size, int prio );
    void kfree( const void * p );
};

extern "C"
{
    void __do_global_ctors();
    void __do_global_dtors();
}

void *operator new ( unsigned ) ;

void *operator new[] ( unsigned ) ;

void operator delete ( void * ) ;

void operator delete[] ( void * ) ;

#define WEAK(x) \
  x __attribute__ ((weak)); \
  x

inline void* operator new( size_t __size, void* __p )
{
    return __p;
}

inline void* operator new[] ( size_t, void* __p )
{
    return __p;
}

extern "C"
{
    int init_module();
    void cleanup_module();
}

#endif
