/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:44:34 CEST 2002  fosi.h 

                       fosi.h -  description
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
 * This file translates the FOSI (Framework Operating System Interface) from
 * orocos calls to native RTOS calls  
 *
 * TODO : split in multiple files
 */


#ifndef __FOSI_H
#define __FOSI_H

#ifdef __cplusplus

extern "C"
{
#endif

extern void __do_global_ctors_aux( void );
extern void __do_global_dtors_aux( void );

#define DO_GLOBAL_CTORS() __do_global_ctors_aux()
#define DO_GLOBAL_DTORS() __do_global_dtors_aux()

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/modversions.h>

#define new _new
#define virtual _virtual

#include <rtl_posixio.h>

#undef new
#undef virtual

    // rt_task compatibility etc....
#include <pthread.h>
#include <rtl_compat.h>
#include <linux/string.h>

#include <semaphore.h> 
    //int rtos_task_init(a,b,c,d,e,f,g) { rt_task_init(a,b,c,d,e); rtos_enable_fpu(*a,1); }
    typedef pthread_t RTOS_TASK;


    // Time Related
#include <rtl_time.h>
    typedef hrtime_t NANO_TIME;
    typedef hrtime_t COUNTS;

    typedef struct timespec TIME_SPEC;

    inline NANO_TIME rtos_get_time_ns( void )
    {
        return gethrtime();
    }

    inline int rtos_nanosleep( const TIME_SPEC * rqtp, TIME_SPEC * rmtp )
    {
        return nanosleep( rqtp, rmtp );
    }

    inline COUNTS nano2count( NANO_TIME nanos )
    {
        return nanos;
    }

    inline NANO_TIME count2nano( COUNTS count )
    {
        return count;
    }

    // Mutex functions

    typedef pthread_mutex_t rt_mutex_t;

    static inline int rtos_mutex_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr)
    {
        return pthread_mutex_init(m, 0);
    }

    static inline int rtos_mutex_destroy(rt_mutex_t* m )
    {
        return pthread_mutex_destroy(m);
    }

    static inline int rtos_mutex_lock( rt_mutex_t* m)
    {
        return pthread_mutex_lock(m);
    }

    static inline int rtos_mutex_trylock( rt_mutex_t* m)
    {
        return pthread_mutex_trylock(m);
    }

    static inline int rtos_mutex_unlock( rt_mutex_t* m)
    {
        return pthread_mutex_unlock(m);
    }



    /**
     * Returns the realtime system time
     */
    inline
    NANO_TIME systemTimeGet( void )
    {
        return gethrtime();
    }

#ifdef __KERNEL__
#include "vsprintf.h"
#endif

    // Console printing
#include <stdarg.h>
    static int rtos_printf( const char * fmt, ... )
    {
        va_list list;
        //    int ret;
        char printkbuf [ 201 ];
        va_start ( list, fmt );
        vsnprintf( printkbuf, 200, fmt, list );
        va_end ( list );
        return rtl_printf( printkbuf );
    }

    inline void rtos_enable_fpu( pthread_attr_t * pt )
    {
        pthread_attr_setfp_np( pt, 1 );
    }

    inline void rtos_task_exit( void )
    {
        pthread_exit( 0 );
    }

    // RT FIFO
#include <rtl_fifo.h>
#define rtosf_create(fnr,bytes) rtf_create(fnr,bytes)
#define rtosf_destroy(fnr) rtf_destroy(fnr)

    inline int rtosf_put( int fnr, const char * text, size_t bytes )
    {
        return rtf_put( fnr, ( char* ) text, bytes );
    };

    inline int rtosf_get( int fnr, char * text, size_t bytes )
    {
        return rtf_get( fnr, text, bytes );
    };

    inline int rtosf_set_handler( int fnr, int ( *handler ) ( unsigned int, int ) )
    {
        return rtf_create_handler( fnr, ( ( int ( * ) ( unsigned int ) ) ( handler ) ) );
    };


#ifdef __cplusplus
}

#endif

#endif
