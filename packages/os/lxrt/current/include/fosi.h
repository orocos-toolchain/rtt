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

#define _XOPEN_SOURCE 600   // use all Posix features.

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sched.h>
 

#ifdef __cplusplus
extern "C" {
#endif

  // include custom redirect-like include
#include <pkgconf/os_lxrt.h>
#if ORONUM_RTAI_VERSION == 3
#include <rtai_config.h>
#else
#include "rtai_config.h"
#endif

#if RTAI_VERSION_MAJOR == 3
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

int lock_all(int stk, int heap);

#define __LXRT_USERSPACE__


  typedef pthread_t RTOS_TASK;
  typedef pthread_mutex_t rt_mutex_t;

// Time Related

typedef RTIME NANO_TIME;
typedef struct timespec TIME_SPEC;
 

// searching for better implementation
// hrt is in nanoseconds
#define hrt2ts(hrt) ((const TIME_SPEC *) ({ TIME_SPEC timevl; \
            timevl.tv_sec = nano2count(hrt) / 1000000000LL;\
            timevl.tv_nsec = nano2count(hrt) % 1000000000LL;\
            &timevl; }))

// turn this on to have maximum detection of valid system calls.
#define CHECK_SYSTEM_CALLS

#ifdef CHECK_SYSTEM_CALLS
#define CHK_LXRT_CALL() do { if(rt_buddy() == 0) \
        printf("LXRT NOT INITIALISED IN THIS THREAD pid=%d,\n\
    BUT TRIES TO INVOKE LXRT FUNCTION >>%s<< ANYWAY\n", getpid(), __FUNCTION__ );\
        } while(0)
#else
#define CHK_LXRT_CALL()
#endif

    
inline NANO_TIME rtos_get_time_ns(void) { return rt_get_time(); }

inline RTIME systemTimeGet(void) { return rt_get_time(); }

inline RTIME ticksPerSec(void) { return nano2count( 1000 * 1000 * 1000 ); }

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

// will become redundant ?
inline void rtos_enable_fpu(pthread_attr_t *pa) { return; }

// RT FIFO

inline int rtosf_create(int fnr,size_t bytes) 
{
    return -1;
};

inline int rtosf_destroy(int fnr)
{
    return -1;
};

inline int rtosf_put(int fnr, const char * text, size_t bytes) 
{
    return -1;
};

inline int rtosf_get(int fnr, char * text, size_t  bytes)
{
    return -1;
};

inline int rtosf_resize(int fnr, size_t bytes) 
{
    return -1;
};

inline void rtosf_remove_all_handlers(int fnr)
{
}


inline int rtosf_set_handler(int fnr, int (*handler)(unsigned int, int))
{
    //	return rtf_create_handler(fnr,X_FIFO_HANDLER(handler));
    return -1;
}


#ifdef __cplusplus
}
#endif

#endif
