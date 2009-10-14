/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:42:55 CEST 2002  fosi.h

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

#define HAVE_FOSI_API

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _XOPEN_SOURCE
#if _XOPEN_SOURCE < 600
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600   // use all Posix features.
#endif
#else
#define _XOPEN_SOURCE 600   // use all Posix features.
#endif

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <assert.h>
#include "../oro_limits.h"
	// Orocos Implementation (i386 specific)
#include "../oro_atomic.h"

    // Time Related
#include <sys/time.h>
#include <time.h>
#include <unistd.h>


    typedef long long NANO_TIME;
    typedef long long TICK_TIME;
    typedef struct timespec TIME_SPEC;


    const TICK_TIME InfiniteTicks = LLONG_MAX;
    const NANO_TIME InfiniteNSecs = LLONG_MAX;
    const double    InfiniteSeconds = DBL_MAX;

  typedef struct {
    pthread_t thread;
    pthread_attr_t attr;

    TIME_SPEC periodMark;
    NANO_TIME period;

    char* name;

    int priority;
  } RTOS_TASK;


#define ORO_SCHED_RT    SCHED_FIFO /** Linux FIFO scheduler */
#define ORO_SCHED_OTHER SCHED_OTHER /** Linux normal scheduler */


	// high-resolution time to timespec
	// hrt is in ticks
	inline TIME_SPEC ticks2timespec(TICK_TIME hrt)
	{
		TIME_SPEC timevl;
		timevl.tv_sec = hrt / 1000000000LL;
		timevl.tv_nsec = hrt % 1000000000LL;
		return timevl;
	}

    inline NANO_TIME rtos_get_time_ns( void )
    {

        TIME_SPEC tv;
        clock_gettime(CLOCK_MONOTONIC, &tv);
        // we can not include the C++ Time.hpp header !
#ifdef __cplusplus
        return NANO_TIME( tv.tv_sec ) * 1000000000LL + NANO_TIME( tv.tv_nsec );
#else
        return ( NANO_TIME ) ( tv.tv_sec * 1000000000LL ) + ( NANO_TIME ) ( tv.tv_nsec );
#endif
    }

    /**
     * This function should return ticks,
     * but we use ticks == nsecs in userspace
     */
    inline NANO_TIME rtos_get_time_ticks()
    {
        return rtos_get_time_ns();
    }

    inline int rtos_nanosleep( const TIME_SPEC * rqtp, TIME_SPEC * rmtp )
    {
        //    return usleep(rqtp->tv_nsec/1000L);
        return nanosleep( rqtp, rmtp );
    }

    /**
     * No conversions are needed in userspace.
     * The HBGenerator needs this for accurate timekeeping,
     * which is an anachronism in userspace.
     */
    inline
    long long nano2ticks( long long nano )
    {
        return nano;
    }

    inline
    long long ticks2nano( long long count )
    {
        return count;
    }

	typedef sem_t rt_sem_t;

    static inline int rtos_sem_init(rt_sem_t* m, int value )
    {
        return sem_init(m, 0, value);
    }

    static inline int rtos_sem_destroy(rt_sem_t* m )
    {
        return sem_destroy(m);
    }

    static inline int rtos_sem_signal(rt_sem_t* m )
    {
        return sem_post(m);
    }

    static inline int rtos_sem_wait(rt_sem_t* m )
    {
        return sem_wait(m);
    }

    static inline int rtos_sem_trywait(rt_sem_t* m )
    {
        return sem_trywait(m);
    }

    static inline int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay )
    {
		TIME_SPEC timevl, delayvl;
        clock_gettime(CLOCK_REALTIME, &timevl);
        delayvl = ticks2timespec(delay);

        // add current time with delay, detect&correct overflows.
        timevl.tv_sec += delayvl.tv_sec;
        timevl.tv_nsec += delayvl.tv_nsec;
        if ( timevl.tv_nsec >= 1000000000) {
            ++timevl.tv_sec;
            timevl.tv_nsec -= 1000000000;
        }

        assert( timevl.tv_nsec < 1000000000 );

        return sem_timedwait( m, &timevl);
    }

    static inline int rtos_sem_wait_until(rt_sem_t* m, NANO_TIME abs_time )
    {
        TIME_SPEC arg_time = ticks2timespec( abs_time );
        return sem_timedwait( m, &arg_time);
    }

    static inline int rtos_sem_value(rt_sem_t* m )
    {
		int val = 0;
        if ( sem_getvalue(m, &val) == 0)
			return val;
		return -1;
    }

    // Mutex functions

    typedef pthread_mutex_t rt_mutex_t;
    typedef pthread_mutex_t rt_rec_mutex_t;

    static inline int rtos_mutex_init(rt_mutex_t* m)
    {
        return pthread_mutex_init(m, 0 );
    }

    static inline int rtos_mutex_destroy(rt_mutex_t* m )
    {
        return pthread_mutex_destroy(m);
    }

    static inline int rtos_mutex_rec_init(rt_mutex_t* m)
    {
        pthread_mutexattr_t ma_t;
        pthread_mutexattr_init(&ma_t);
		pthread_mutexattr_settype(&ma_t,PTHREAD_MUTEX_RECURSIVE_NP);
        return pthread_mutex_init(m, &ma_t );
    }

    static inline int rtos_mutex_rec_destroy(rt_mutex_t* m )
    {
        return pthread_mutex_destroy(m);
    }

    static inline int rtos_mutex_lock( rt_mutex_t* m)
    {
        return pthread_mutex_lock(m);
    }

    static inline int rtos_mutex_rec_lock( rt_mutex_t* m)
    {
        return pthread_mutex_lock(m);
    }

    static inline int rtos_mutex_trylock( rt_mutex_t* m)
    {
        return pthread_mutex_trylock(m);
    }

    static inline int rtos_mutex_rec_trylock( rt_mutex_t* m)
    {
        return pthread_mutex_trylock(m);
    }

    static inline int rtos_mutex_unlock( rt_mutex_t* m)
    {
        return pthread_mutex_unlock(m);
    }

    static inline int rtos_mutex_rec_unlock( rt_mutex_t* m)
    {
        return pthread_mutex_unlock(m);
    }


#define rtos_printf printf

#ifdef __cplusplus
}

#endif
#endif
