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

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600   // use all Posix features.
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sched.h>
#include <assert.h> 

#ifdef __cplusplus
extern "C" {
#endif

	// Orocos Implementation (i386 specific)
#include "oro_atomic.h"
#include "oro_bitops.h"

#include "pkgconf/os.h"
#include "pkgconf/os_lxrt.h"
#if !defined(OROBLD_OS_AGNOSTIC) || defined(OROBLD_OS_LXRT_INTERNAL) // define the latter to include nevertheless the RTAI header files

// include custom redirect-like include
#if ORONUM_RTAI_VERSION == 3
#include <rtai_config.h>
#else
#include "rtai_config.h"
#endif


#if ORONUM_RTAI_VERSION == 3
#include <rtai_lxrt.h>
#include <rtai_posix.h>
#else
#define KEEP_STATIC_INLINE
#include <rtai_declare.h>
#include <rtai_usp_posix.h>
#include <rtai_lxrt_user.h> 

    /**
     * About KEEP_STATIC_INLINE:
     *  What is below is questionable, sometimes it crashes, sometimes not. I disassembled the C and C++ object code,
     *  they were the same for 3.x compilers. (ps)
     *  crashes on C++ stuff when using 2.95 compiler, thus you can not include rtai_lxrt.h or you get multiple
     * defined symbols. Missing defines from rtai_lxrt.h need to be redefined in other headerfile.
     */
//#include <rtai_fifos_lxrt_user.h> 
#endif

	// Finally, define the types we use :
	typedef RT_TASK RTOS_TASK;
	typedef SEM     RTOS_SEM;
	typedef CND     RTOS_CND;

#else // AGNOSTIC

	// For PeriodicTask.cxx :
	// we need to define the types without the headers,
	// this is RTAI version dependent.
#if ORONUM_RTAI_VERSION == 3
    // v3.x :
	typedef struct oro_lxrt_t {
		int opaque;
	} __LXRT_HANDLE_STRUCT;
    typedef __LXRT_HANDLE_STRUCT RTOS_TASK;
    typedef __LXRT_HANDLE_STRUCT RTOS_SEM;
    typedef __LXRT_HANDLE_STRUCT RTOS_CND; 
#else
    // v24.1.x :
    typedef void RTOS_TASK;
    typedef void RTOS_SEM;
    typedef void RTOS_CND; 
#endif
#endif // OROBLD_OS_AGNOSTIC // for RTAI header files.

	/**
	 * Typdefs
	 */
	int lock_all(int stk, int heap);

#define __LXRT_USERSPACE__


	typedef pthread_mutex_t rt_mutex_t;
	
	// Time Related
	
	typedef long long NANO_TIME;
	typedef long long TICK_TIME;
	typedef struct timespec TIME_SPEC;
	
#ifndef OROBLD_OS_AGNOSTIC
	
	// hrt is in ticks
TIME_SPEC ticks2timespec(TICK_TIME hrt)
{
	TIME_SPEC timevl;
	timevl.tv_sec = nano2count(hrt) / 1000000000LL;
	timevl.tv_nsec = nano2count(hrt) % 1000000000LL;
	return timevl;
}

// turn this on to have maximum detection of valid system calls.
#ifdef OROSEM_OS_LXRT_CHECK
#define CHK_LXRT_CALL() do { if(rt_buddy() == 0) { \
        printf("LXRT NOT INITIALISED IN THIS THREAD pid=%d,\n\
    BUT TRIES TO INVOKE LXRT FUNCTION >>%s<< ANYWAY\n", getpid(), __FUNCTION__ );\
        assert( rt_buddy() != 0 ); }\
        } while(0)
#define CHK_LXRT_PTR(ptr) do { if(ptr == 0) { \
        printf("TRIED TO PASS NULL POINTER TO LXRT IN THREAD pid=%d,\n\
    IN TRYING TO INVOKE LXRT FUNCTION >>%s<<\n", getpid(), __FUNCTION__ );\
        assert( ptr != 0 ); }\
        } while(0)
#else
#define CHK_LXRT_CALL()
#define CHK_LXRT_PTR()
#endif
    
inline NANO_TIME rtos_get_time_ns(void) { return rt_get_time_ns(); }

inline TICK_TIME systemTimeGet(void) { return rt_get_time(); }

inline TICK_TIME ticksPerSec(void) { return nano2count( 1000 * 1000 * 1000 ); }

	inline TICK_TIME nano2ticks(NANO_TIME t) { return nano2count(t); }
	inline NANO_TIME ticks2nano(TICK_TIME t) { return count2nano(t); }

inline int rtos_nanosleep(const TIME_SPEC *rqtp, TIME_SPEC *rmtp) 
{
    CHK_LXRT_CALL();
    nanosleep(rqtp,rmtp); // rtai 24.1.9
    return 0;
}

    static inline int rtos_mutex_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr)
    {
        CHK_LXRT_CALL();
        return pthread_mutex_init_rt(m, 0);
    }

    static inline int rtos_mutex_destroy(rt_mutex_t* m )
    {
        CHK_LXRT_CALL();
        return pthread_mutex_destroy_rt(m);
    }

    static inline int rtos_mutex_rec_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr)
    {
        CHK_LXRT_CALL();
        pthread_mutexattr_t ma_t;
        pthread_mutexattr_init(&ma_t);
		pthread_mutexattr_settype(&ma_t,PTHREAD_MUTEX_RECURSIVE_NP);
        return pthread_mutex_init_rt(m, 0);
    }

    static inline int rtos_mutex_rec_destroy(rt_mutex_t* m )
    {
        CHK_LXRT_CALL();
        return pthread_mutex_destroy_rt(m);
    }

    static inline int rtos_mutex_lock( rt_mutex_t* m)
    {
        CHK_LXRT_CALL();
        return pthread_mutex_lock_rt(m);
    }

    static inline int rtos_mutex_trylock( rt_mutex_t* m)
    {
        CHK_LXRT_CALL();
        return pthread_mutex_trylock_rt(m);
    }

    static inline int rtos_mutex_unlock( rt_mutex_t* m)
    {
        CHK_LXRT_CALL();
        return pthread_mutex_unlock_rt(m);
    }


    typedef pthread_cond_t rt_cond_t;
    static inline int rtos_cond_init(rt_cond_t *cond, pthread_condattr_t *cond_attr)
    {
        CHK_LXRT_CALL();
        return pthread_cond_init_rt(cond, cond_attr);
    }

    static inline int rtos_cond_destroy(rt_cond_t *cond)
    {
        CHK_LXRT_CALL();
        return pthread_cond_destroy_rt(cond);
    }

    static inline int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, const struct timespec *abstime)
    {
        CHK_LXRT_CALL();
        return pthread_cond_timedwait_rt(cond, mutex, abstime);
    }

    static inline int rtos_cond_broadcast(rt_cond_t *cond)
    {
        CHK_LXRT_CALL();
        return pthread_cond_broadcast_rt(cond);
    }

inline
int rtos_printf(const char *fmt, ...)
{
    va_list list;
    char printkbuf [2000];
    printkbuf[0] = '\0';
    va_start (list, fmt);
    vsprintf(printkbuf, fmt, list);
    va_end (list);
    // XXX revert to print to screen when debugging is over
    return rtai_print_to_screen(printkbuf);
    //return printf(printkbuf);
}

#else  // OSBLD_OS_AGNOSTIC

/**
 * Only function prototypes.
 */

TIME_SPEC ticks2timespec(TICK_TIME hrt);

NANO_TIME rtos_get_time_ns(void);

TICK_TIME systemTimeGet(void);

TICK_TIME ticksPerSec(void);

TICK_TIME nano2ticks(NANO_TIME t);

NANO_TIME ticks2nano(TICK_TIME t);

int rtos_nanosleep(const TIME_SPEC *rqtp, TIME_SPEC *rmtp) ;

int rtos_mutex_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr);

int rtos_mutex_destroy(rt_mutex_t* m );

int rtos_mutex_rec_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr);

int rtos_mutex_rec_destroy(rt_mutex_t* m );

int rtos_mutex_lock( rt_mutex_t* m);

int rtos_mutex_trylock( rt_mutex_t* m);

int rtos_mutex_unlock( rt_mutex_t* m);

typedef pthread_cond_t rt_cond_t;
int rtos_cond_init(rt_cond_t *cond, pthread_condattr_t *cond_attr);

int rtos_cond_destroy(rt_cond_t *cond);

int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, const struct timespec *abstime);

int rtos_cond_broadcast(rt_cond_t *cond);

int rtos_printf(const char *fmt, ...);

#endif // OSBLD_OS_AGNOSTIC



// will become redundant ?
void rtos_enable_fpu(pthread_attr_t *pa);

// RT FIFO

int rtosf_create(int fnr,size_t bytes);

int rtosf_destroy(int fnr);

int rtosf_put(int fnr, const char * text, size_t bytes);

int rtosf_get(int fnr, char * text, size_t  bytes);

int rtosf_resize(int fnr, size_t bytes) ;

void rtosf_remove_all_handlers(int fnr);


int rtosf_set_handler(int fnr, int (*handler)(unsigned int, int));


#ifdef __cplusplus
}
#endif

#endif
