/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:43:39 CEST 2002  fosi.h 

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

extern void __do_global_ctors( void );
extern void __do_global_dtors( void );

#define DO_GLOBAL_CTORS() __do_global_ctors()
#define DO_GLOBAL_DTORS() __do_global_dtors()

#ifndef NULL
#define NULL 0
#endif

#define new _new
#define virtual _virtual

#include <linux/kernel.h>
#include <linux/version.h>
  // module.h conflicts with rtai_pqueue_wrapper.h
  //#include <linux/module.h>

//#if !defined( _STRUCT_TIMESPEC ) ||  !defined( __timespec_defined )
//#define _STRUCT_TIMESPEC
//#define __timespec_defined 1
//# define _TIME_H    1
//    struct timespec
//    {
//        time_t tv_sec;         /* seconds */
//        long tv_nsec;        /* nanoseconds */
//    };
//#endif /* _STRUCT_TIMESPEC */

#ifndef __cplusplus

#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>
#include <rtai_pthread.h>
#include <rtai_types.h>

#else

#define _XOPEN_SOURCE 600
#include <features.h>

// only exclude linux/time.h for gcc 3.2 and higher.
#if __GNUC__ <  3
#else
#  define _LINUX_TIME_H
#endif

#include <rtai_fifos.h>
#include <rtai_types.h>

#include <rtai_cpp/linux_wrapper.h>
#include <rtai_cpp/rtai_wrapper.h>
#include <rtai_cpp/rtai_pthread_wrapper.h>
// include local version until rtai is fixed
#include "rtai_asm_wrapper.h"
#include <rtai_cpp/rtai_pqueue_wrapper.h>

// use kernel types
#define _BITS_PTHREADTYPES_H 1
typedef unsigned int pthread_key_t;
typedef int pthread_once_t;

#include <features.h>
#include "pthread_adds.h"
#include <stdio.h>

#endif //__cplusplus

  //#include <rtai_pthread_int.h>
  // secure that user space threads are not used
#define _BITS_PTHREADTYPES_H    1
  // use kernel limits.
#define _GCC_LIMITS_H_          1

#include <stdarg.h>
#include <linux/string.h>

#undef new
#undef virtual

static inline char *getenv ( const char *c) { return 0; }

typedef pthread_t RTOS_TASK;
typedef SEM sem_t;

// Time Related

typedef RTIME NANO_TIME;

typedef struct timespec TIME_SPEC;

/**
 * Returns the amount of ticks in one second
 */
inline RTIME ticksPerSec( void );

/**
 * Returns the realtime system time
 */
inline RTIME systemTimeGet( void );

// searching for better implementation
// hrt is in nanoseconds
#define hrt2ts(hrt) ((const TIME_SPEC *) ({ TIME_SPEC timevl; \
            __count2timespec(nano2count(hrt),&timevl);\
            &timevl; }))

inline NANO_TIME rtos_get_time_ns( void );

inline int rtos_nanosleep( const TIME_SPEC *rqtp, TIME_SPEC *rmtp );

inline int sem_post( sem_t *s );

inline int sem_wait( sem_t *s );

inline void sem_init( sem_t *s, int dummy, int value );

inline int sem_destroy( sem_t *s );


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

    typedef pthread_cond_t rt_cond_t;
    static inline int rtos_cond_init(rt_cond_t *cond, pthread_condattr_t *cond_attr)
    {
        return pthread_cond_init(cond, cond_attr);
    }

    static inline int rtos_cond_destroy(rt_cond_t *cond)
    {
        return pthread_cond_destroy(cond);
    }

    static inline int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, const struct timespec *abstime)
    {
        return pthread_cond_timedwait(cond, mutex, abstime);
    }

    static inline int rtos_cond_broadcast(rt_cond_t *cond)
    {
        return pthread_cond_broadcast(cond);
    }



// printf
//#define rtos_printf(x) rt_printk(x)
inline int rtos_printf( const char *fmt, ... );

// will become redundant ?
inline void rtos_enable_fpu( pthread_attr_t *pa );

// RT FIFO
// niet meer nodig, nu gedefined in configure.ac
//#define HAVE_FIFO_RT

inline int rtosf_create( int fnr, size_t bytes );

inline int rtosf_destroy( int fnr );

inline int rtosf_put( int fnr, const char * text, size_t bytes );

inline int rtosf_get( int fnr, char * text, size_t bytes );

inline int rtosf_resize( int fnr, size_t bytes );

inline void rtosf_remove_all_handlers( int fnr );

inline int rtosf_set_handler( int fnr, int ( *handler ) ( unsigned int, int ) );

#ifdef __cplusplus
}
#endif


/**
 * waits for a semaphore in init_module
 */
#ifdef __cplusplus
extern "C"
#endif
    void waitInModule( sem_t* t );

#endif
