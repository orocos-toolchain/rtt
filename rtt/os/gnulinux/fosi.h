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

#ifndef _XOPEN_SOURCE
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

    // Time Related
#include <sys/time.h>
#include <time.h>
#include <unistd.h>


    typedef long long NANO_TIME;
    typedef long long TICK_TIME;
    typedef struct timespec TIME_SPEC;


    static const TICK_TIME InfiniteTicks = LLONG_MAX;
    static const NANO_TIME InfiniteNSecs = LLONG_MAX;
    static const double    InfiniteSeconds = DBL_MAX;

#define ORO_WAIT_ABS 0 /** rtos_task_wait_period may wait less than the duration required to pad the period to 
                            catch-up with overrun timesteps (wait according to an absolute timeline) */
#define ORO_WAIT_REL 1 /** rtos_task_wait_period will always pad the current timestep to the desired period, 
                            regardless of previous overruns (wait according to a relative timeline) */

  typedef struct {
    pthread_t thread;
    pthread_attr_t attr;

    TIME_SPEC periodMark;
    NANO_TIME period;

    char* name;

    int priority;
    int wait_policy;
    pid_t pid;
  } RTOS_TASK;


#define ORO_SCHED_RT    SCHED_FIFO /** Linux FIFO scheduler */
#define ORO_SCHED_OTHER SCHED_OTHER /** Linux normal scheduler */


	// high-resolution time to timespec
	// hrt is in ticks
	static inline TIME_SPEC ticks2timespec(TICK_TIME hrt)
	{
		TIME_SPEC timevl;
		timevl.tv_sec = hrt / 1000000000LL;
		timevl.tv_nsec = hrt % 1000000000LL;
		return timevl;
	}

    static inline NANO_TIME rtos_get_time_ns( void )
    {
        TIME_SPEC tv;
        clock_gettime(CLOCK_MONOTONIC, &tv);
        return ( NANO_TIME ) ( tv.tv_sec * 1000000000LL ) + ( NANO_TIME ) ( tv.tv_nsec );
    }

    // internal use only, for rtos_mutex_trylock_for/rtos_mutex_rec_trylock_for!
    static inline NANO_TIME rtos_get_realtime_ns( void )
    {
        TIME_SPEC tv;
        clock_gettime(CLOCK_REALTIME, &tv);
        return ( NANO_TIME ) ( tv.tv_sec * 1000000000LL ) + ( NANO_TIME ) ( tv.tv_nsec );
    }

    /**
     * This function should return ticks,
     * but we use ticks == nsecs in userspace
     */
    static inline NANO_TIME rtos_get_time_ticks(void)
    {
        return rtos_get_time_ns();
    }

    static inline int rtos_nanosleep( const TIME_SPEC * rqtp, TIME_SPEC * rmtp )
    {
        //    return usleep(rqtp->tv_nsec/1000L);
        return nanosleep( rqtp, rmtp );
    }

    /**
     * No conversions are needed in userspace.
     * The HBGenerator needs this for accurate timekeeping,
     * which is an anachronism in userspace.
     */
    static inline
    long long nano2ticks( long long nano )
    {
        return nano;
    }

    static inline
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

    static inline int rtos_mutex_rec_init(rt_rec_mutex_t* m)
    {
        pthread_mutexattr_t attr;
        int ret = pthread_mutexattr_init(&attr);
        if (ret != 0) return ret;

        // make mutex recursive
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
        if (ret != 0) return ret;

        ret = pthread_mutex_init(m, &attr);

        pthread_mutexattr_destroy(&attr);
        return ret;
    }

    static inline int rtos_mutex_rec_destroy(rt_rec_mutex_t* m )
    {
        return pthread_mutex_destroy(m);
    }

    static inline int rtos_mutex_lock( rt_mutex_t* m)
    {
        return pthread_mutex_lock(m);
    }

    static inline int rtos_mutex_rec_lock( rt_rec_mutex_t* m)
    {
        return pthread_mutex_lock(m);
    }

    static inline int rtos_mutex_trylock_for( rt_mutex_t* m, NANO_TIME relative_time)
    {
        // pthread_mutex_timedlock() does not support relative timeouts, nor CLOCK_MONOTONIC.
        // Workaround: add the relative time period to an absolute time retrieved
        // by rtos_get_realtime_ns() using CLOCK_REALTIME (may be affected by time
        // adjustments while waiting)
        TIME_SPEC arg_time = ticks2timespec( rtos_get_realtime_ns() + relative_time );
        return pthread_mutex_timedlock(m, &arg_time);
    }

    static inline int rtos_mutex_rec_trylock_for( rt_rec_mutex_t* m, NANO_TIME relative_time)
    {
        // pthread_mutex_timedlock() does not support relative timeouts, nor CLOCK_MONOTONIC.
        // Workaround: add the relative time period to an absolute time retrieved
        // by rtos_get_realtime_ns() using CLOCK_REALTIME (may be affected by time
        // adjustments while waiting)
        TIME_SPEC arg_time = ticks2timespec( rtos_get_realtime_ns() + relative_time );
        return pthread_mutex_timedlock(m, &arg_time);
    }

    static inline int rtos_mutex_lock_until( rt_mutex_t* m, NANO_TIME abs_time)
    {
        // pthread_mutex_timedlock() does not accept an absolute CLOCK_MONOTONIC timestamp.
        // Workaround: convert abs_time to a relative time using rtos_get_time_ns()
        // and fall back to rtos_mutex_trylock_for().
        NANO_TIME relative_time = abs_time - rtos_get_time_ns();
        if (relative_time < 0) return ETIMEDOUT;
        return rtos_mutex_trylock_for( m, relative_time );
    }

    static inline int rtos_mutex_rec_lock_until( rt_rec_mutex_t* m, NANO_TIME abs_time)
    {
        // pthread_mutex_timedlock() does not accept an absolute CLOCK_MONOTONIC timestamp.
        // Workaround: convert abs_time to a relative time using rtos_get_time_ns()
        // and fall back to rtos_mutex_rec_trylock_for().
        NANO_TIME relative_time = abs_time - rtos_get_time_ns();
        if (relative_time < 0) return ETIMEDOUT;
        return rtos_mutex_rec_trylock_for( m, relative_time );
    }

    static inline int rtos_mutex_trylock( rt_mutex_t* m)
    {
        return pthread_mutex_trylock(m);
    }

    static inline int rtos_mutex_rec_trylock( rt_rec_mutex_t* m)
    {
        return pthread_mutex_trylock(m);
    }

    static inline int rtos_mutex_unlock( rt_mutex_t* m)
    {
        return pthread_mutex_unlock(m);
    }

    static inline int rtos_mutex_rec_unlock( rt_rec_mutex_t* m)
    {
        return pthread_mutex_unlock(m);
    }

    static inline void rtos_enable_rt_warning(void)
    {
    }

    static inline void rtos_disable_rt_warning(void)
    {
    }

    typedef pthread_cond_t rt_cond_t;

    static inline int rtos_cond_init(rt_cond_t *cond)
    {
        pthread_condattr_t attr;
        int ret = pthread_condattr_init(&attr);
        if (ret != 0) return ret;

        // set the clock selection condition variable attribute to CLOCK_MONOTONIC
        ret = pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
        if (ret != 0) {
            pthread_condattr_destroy(&attr);
            return ret;
        }

        ret = pthread_cond_init(cond, &attr);
        pthread_condattr_destroy(&attr);

        return ret;
    }

    static inline int rtos_cond_destroy(rt_cond_t *cond)
    {
        return pthread_cond_destroy(cond);
    }

    static inline int rtos_cond_wait(rt_cond_t *cond, rt_mutex_t *mutex)
    {
        return pthread_cond_wait(cond, mutex);
    }

    static inline int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, NANO_TIME abs_time)
    {
        TIME_SPEC arg_time = ticks2timespec( abs_time );
        return pthread_cond_timedwait(cond, mutex, &arg_time);
    }

    static inline int rtos_cond_broadcast(rt_cond_t *cond)
    {
        return pthread_cond_broadcast(cond);
    }

#define rtos_printf printf

#ifdef __cplusplus
}

#endif
#endif
