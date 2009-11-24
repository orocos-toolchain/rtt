/***************************************************************************
    copyright            : (C) 2008 Klaas Gadeyne
    email                : firstname dot lastname at gmail dot com

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


#ifndef __FOSI_H
#define __FOSI_H

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600   // use all Posix features.
#endif

#define HAVE_FOSI_API

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdio.h>
#include <pthread.h>
#include <mach/mach_init.h>
#include <mach/task.h>

#include <errno.h>

#include <limits.h>
#include <float.h>
#include <assert.h>

    typedef long long NANO_TIME;
    typedef long long TICK_TIME;

    const TICK_TIME InfiniteTicks = LLONG_MAX;
    const NANO_TIME InfiniteNSecs = LLONG_MAX;
    const double    InfiniteSeconds = DBL_MAX;

    typedef struct {
        pthread_t thread;
        pthread_attr_t attr;

        NANO_TIME periodMark;
        NANO_TIME period;

        char* name;

        int priority;
    } RTOS_TASK;


#define ORO_SCHED_RT    SCHED_FIFO /** Posix FIFO scheduler */
#define ORO_SCHED_OTHER SCHED_OTHER /** Posix normal scheduler */

    /*
     * Time Related stuff
     */
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

    typedef struct timespec TIME_SPEC;

    /* fake clock_gettime for systems like darwin */
    #define  CLOCK_REALTIME 0
    inline int clock_gettime(int clk_id /*ignored*/, struct timespec *tp)
    {
        struct timeval now;
        int rv = gettimeofday(&now, NULL);
        if (rv != 0){
            tp->tv_sec = 0;
            tp->tv_nsec = 0;
            return rv;
        }
        tp->tv_sec = now.tv_sec;
        tp->tv_nsec = now.tv_usec * 1000;
        return 0;
    }

    // high-resolution time to timespec
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
        clock_gettime(CLOCK_REALTIME, &tv);
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

    inline long long nano2ticks( long long nano )
    {
        return nano;
    }

    inline long long ticks2nano( long long count )
    {
        return count;
    }

    /*
     * Semaphore functions
     * See
     * http://developer.apple.com/documentation/Darwin/Conceptual/KernelProgramming/synchronization/chapter_15_section_2.html
     */
#include <mach/semaphore.h>
    typedef semaphore_t rt_sem_t;

    static inline int rtos_sem_init(rt_sem_t* m, int value )
    {
        return semaphore_create(mach_task_self(), m, SYNC_POLICY_FIFO, value);
    }

    static inline int rtos_sem_destroy(rt_sem_t* m )
    {
        return semaphore_destroy(mach_task_self(), *m);
    }

    static inline int rtos_sem_signal(rt_sem_t* m )
    {
        return semaphore_signal(*m);
    }

    static inline int rtos_sem_wait(rt_sem_t* m )
    {
        return semaphore_wait(*m);
    }

    static inline int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay )
    {
        TIME_SPEC delayvl = ticks2timespec(delay);
        mach_timespec_t mach_delayvl = { delayvl.tv_sec, delayvl.tv_nsec };

        return semaphore_timedwait( *m, mach_delayvl);
    }

    static inline int rtos_sem_trywait(rt_sem_t* m )
    {
        return rtos_sem_wait_timed(m,0);
    }

    static inline int rtos_sem_wait_until(rt_sem_t* m, NANO_TIME abs_time )
    {
        TIME_SPEC timevl, delayvl;
        TIME_SPEC arg_time = ticks2timespec( abs_time );
        clock_gettime(CLOCK_REALTIME, &timevl);

        // calculate delay from abs_time
        delayvl.tv_sec = arg_time.tv_sec - timevl.tv_sec;
        delayvl.tv_nsec = arg_time.tv_nsec - timevl.tv_nsec;
        if ( delayvl.tv_nsec >= 1000000000) { // tv_nsec is unsigned
                                              // int !!
            --timevl.tv_sec;
            timevl.tv_nsec += 1000000000;
        }

        assert( delayvl.tv_nsec < 1000000000 );
        // What if delayvl.tv_sec is < 0 (is unsigned int)

        mach_timespec_t mach_delayvl = { delayvl.tv_sec, delayvl.tv_nsec };
        return semaphore_timedwait( *m, mach_delayvl);
    }

    // semaphore_value is not supported on darwin
    /*     static inline int rtos_sem_value(rt_sem_t* m ) */
    /*     { */
    /* 		int val = 0; */
    /*         if ( sem_getvalue(m, &val) == 0) */
    /* 			return val; */
    /* 		return -1; */
    /*     } */

    static inline void rtos_enable_rt_warning()
    {
    }

    static inline void rtos_disable_rt_warning()
    {
    }


#define rtos_printf printf

#ifdef __cplusplus
}

#endif
#endif
