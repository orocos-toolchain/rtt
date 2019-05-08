/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  fosi.h

                        fosi.h -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef _XENO_FOSI_H
#define _XENO_FOSI_H

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600   // use all Posix features.
#endif

#define HAVE_FOSI_API

#ifdef __cplusplus
extern "C" {
#endif

#include "../../rtt-config.h"

	//Xenomai headers
	//#include <linux/types.h>
	// xenomai assumes presence of u_long
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include "../oro_limits.h"

#include <xeno_config.h> // version number
#include <native/task.h>
#include <native/timer.h>
#include <native/mutex.h>
#include <native/sem.h>
#include <native/cond.h>

// BC: support Xenomai < 2.3.0
#if ((CONFIG_XENO_VERSION_MAJOR*1000)+(CONFIG_XENO_VERSION_MINOR*100)+CONFIG_XENO_REVISION_LEVEL) < 2300
#define rt_mutex_acquire rt_mutex_lock
#define rt_mutex_release rt_mutex_unlock
#endif
// BC: support Xenomai < 2.5.0
#if ((CONFIG_XENO_VERSION_MAJOR*1000)+(CONFIG_XENO_VERSION_MINOR*100)+CONFIG_XENO_REVISION_LEVEL) >= 2500
#define ORO_XENO_HAS_ACQUIRE_UNTIL
#endif


	typedef RT_MUTEX rt_mutex_t;
	typedef RT_MUTEX rt_rec_mutex_t;
	typedef RT_SEM rt_sem_t;
	typedef RT_COND rt_cond_t;

	// Time Related
	// 'S' -> Signed long long
	typedef SRTIME          NANO_TIME;
	typedef SRTIME          TICK_TIME;
	typedef struct timespec TIME_SPEC;
	typedef RT_TASK         RTOS_XENO_TASK;

    // Thread/Task related.
    typedef struct {
        char * name;
        RTOS_XENO_TASK xenotask;
        RTOS_XENO_TASK* xenoptr;
        int sched_type;
    } RTOS_TASK;

    static const TICK_TIME InfiniteTicks = LONG_LONG_MAX;
    static const NANO_TIME InfiniteNSecs = LONG_LONG_MAX;
    static const double    InfiniteSeconds = DBL_MAX;

#define SCHED_XENOMAI_HARD 0 /** Hard real-time */
#define SCHED_XENOMAI_SOFT 1 /** Soft real-time */
#define ORO_SCHED_RT    0 /** Hard real-time */
#define ORO_SCHED_OTHER 1 /** Soft real-time */

#define ORO_WAIT_ABS 0
#define ORO_WAIT_REL 1

	// hrt is in ticks
static inline TIME_SPEC ticks2timespec(TICK_TIME hrt)
{
	TIME_SPEC timevl;
	timevl.tv_sec = rt_timer_tsc2ns(hrt) / 1000000000LL;
	timevl.tv_nsec = rt_timer_tsc2ns(hrt) % 1000000000LL;
	return timevl;
}

	// hrt is in ticks
static inline TICK_TIME timespec2ticks(const TIME_SPEC* ts)
{
	return  rt_timer_ns2tsc(ts->tv_nsec + ts->tv_sec*1000000000LL);
}

// turn this on to have maximum detection of valid system calls.
#ifdef OROSEM_OS_XENO_CHECK
#define CHK_XENO_CALL() do { if(rt_task_self() == 0) { \
        printf("RTT: XENO NOT INITIALISED IN THIS THREAD pid=%d,\n\
    BUT TRIES TO INVOKE XENO FUNCTION >>%s<< ANYWAY\n", getpid(), __FUNCTION__ );\
        assert( rt_task_self() != 0 ); }\
        } while(0)
#define CHK_XENO_PTR(ptr) do { if(ptr == 0) { \
        printf("RTT: TRIED TO PASS NULL POINTER TO XENO IN THREAD pid=%d,\n\
    IN TRYING TO INVOKE XENO FUNCTION >>%s<<\n", getpid(), __FUNCTION__ );\
        assert( ptr != 0 ); }\
        } while(0)
#else
#define CHK_XENO_CALL()
#define CHK_XENO_PTR( a )
#endif

static inline NANO_TIME rtos_get_time_ns(void) { return rt_timer_ticks2ns(rt_timer_read()); }

static inline TICK_TIME rtos_get_time_ticks(void) { return rt_timer_tsc(); }

static inline TICK_TIME ticksPerSec(void) { return rt_timer_ns2tsc( 1000 * 1000 * 1000 ); }

// WARNING: Orocos 'ticks' are 'Xenomai tsc' and Xenomai `ticks' are not
// used in the Orocos API. Thus Orocos uses `Xenomai tsc' and `Xenomai ns',
// yet Xenomai requires `Xenomai ticks' at the interface
// ==> do not use nano2ticks to convert to `Xenomai ticks' because it
// converts to `Xenomai tsc'.
static inline TICK_TIME nano2ticks(NANO_TIME t) { return rt_timer_ns2tsc(t); }
static inline NANO_TIME ticks2nano(TICK_TIME t) { return rt_timer_tsc2ns(t); }

static inline int rtos_nanosleep(const TIME_SPEC *rqtp, TIME_SPEC *rmtp)
	{
		CHK_XENO_CALL();
		RTIME ticks = rqtp->tv_sec * 1000000000LL + rqtp->tv_nsec;
		rt_task_sleep( rt_timer_ns2ticks(ticks) );
		return 0;
	}

    static inline int rtos_sem_init(rt_sem_t* m, int value )
    {
        CHK_XENO_CALL();
		return rt_sem_create( m, 0, value, S_PRIO);
    }

    static inline int rtos_sem_destroy(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        return rt_sem_delete( m );
    }

    static inline int rtos_sem_signal(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        return rt_sem_v( m );
    }

    static inline int rtos_sem_wait(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        return rt_sem_p( m, TM_INFINITE );
    }

    static inline int rtos_sem_trywait(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        return rt_sem_p( m, TM_NONBLOCK);
    }

    static inline int rtos_sem_value(rt_sem_t* m )
    {
        CHK_XENO_CALL();
	RT_SEM_INFO sinfo;
        if (rt_sem_inquire(m, &sinfo) == 0 ) {
	  return sinfo.count;
	}
	return -1;
    }

    static inline int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay )
    {
        CHK_XENO_CALL();
        return rt_sem_p(m, rt_timer_ns2ticks(delay) ) == 0 ? 0 : -1;
    }

    static inline int rtos_sem_wait_until(rt_sem_t* m, NANO_TIME when )
    {
        CHK_XENO_CALL();
        return rt_sem_p(m, rt_timer_ns2ticks(when) - rt_timer_read() ) == 0 ? 0 : -1;
    }

    static inline int rtos_mutex_init(rt_mutex_t* m)
    {
        CHK_XENO_CALL();
		// a Xenomai mutex is always recursive
        return rt_mutex_create(m, 0);
    }

    static inline int rtos_mutex_destroy(rt_mutex_t* m )
    {
        CHK_XENO_CALL();
        return rt_mutex_delete(m);
    }

    static inline int rtos_mutex_rec_init(rt_rec_mutex_t* m)
    {
        CHK_XENO_CALL();
		// a Xenomai mutex is always recursive
        return rt_mutex_create(m, 0);
    }

    static inline int rtos_mutex_rec_destroy(rt_rec_mutex_t* m )
    {
        CHK_XENO_CALL();
        return rt_mutex_delete(m);
    }

    static inline int rtos_mutex_lock( rt_mutex_t* m)
    {
        CHK_XENO_CALL();
        return rt_mutex_acquire(m, TM_INFINITE );
    }

    static inline int rtos_mutex_trylock( rt_mutex_t* m)
    {
        CHK_XENO_CALL();
        struct rt_mutex_info info;
        rt_mutex_inquire(m, &info );
#if ((CONFIG_XENO_VERSION_MAJOR*1000)+(CONFIG_XENO_VERSION_MINOR*100)+CONFIG_XENO_REVISION_LEVEL) >= 2500
        if (info.locked)
            return 0;
#else
        if (info.lockcnt)
            return 0;
#endif
        // from here on: we're sure our thread didn't lock it
        // now check if any other thread locked it:
        return rt_mutex_acquire(m, TM_NONBLOCK);
    }

    static inline int rtos_mutex_lock_until( rt_mutex_t* m, NANO_TIME abs_time)
    {
        CHK_XENO_CALL();
#if !defined(ORO_XENO_HAS_ACQUIRE_UNTIL) // see top of this file
        // calling the old style API
        return rt_mutex_acquire(m, rt_timer_ns2ticks(abs_time) - rt_timer_read()  );
#else
        // new style API > 2.5.0
        return rt_mutex_acquire_until(m, rt_timer_ns2ticks(abs_time) );
#endif
    }

    static inline int rtos_mutex_trylock_for( rt_mutex_t* m, NANO_TIME relative_time)
    {
        CHK_XENO_CALL();
        return rt_mutex_acquire(m, rt_timer_ns2ticks(relative_time) );
    }

    static inline int rtos_mutex_unlock( rt_mutex_t* m)
    {
        CHK_XENO_CALL();
        return rt_mutex_release(m);
    }

    static inline int rtos_mutex_rec_lock( rt_rec_mutex_t* m)
    {
        return rtos_mutex_lock(m);
    }

    static inline int rtos_mutex_rec_trylock( rt_rec_mutex_t* m)
    {
        return rtos_mutex_trylock(m);
    }

    static inline int rtos_mutex_rec_lock_until( rt_rec_mutex_t* m, NANO_TIME abs_time)
    {
        return rtos_mutex_lock_until(m, abs_time);
    }

    static inline int rtos_mutex_rec_trylock_for( rt_rec_mutex_t* m, NANO_TIME relative_time)
    {
        return rtos_mutex_trylock_for(m, relative_time);
    }

    static inline int rtos_mutex_rec_unlock( rt_rec_mutex_t* m)
    {
        return rtos_mutex_unlock(m);
    }

    static inline void rtos_enable_rt_warning()
    {
        CHK_XENO_CALL();
        rt_task_set_mode(0, T_WARNSW, NULL);
    }

    static inline void rtos_disable_rt_warning()
    {
        CHK_XENO_CALL();
        rt_task_set_mode(T_WARNSW, 0, NULL);
    }

    static inline int rtos_cond_init(rt_cond_t *cond)
    {
        CHK_XENO_CALL();
        return rt_cond_create(cond, 0);
    }

    static inline int rtos_cond_destroy(rt_cond_t *cond)
    {
        CHK_XENO_CALL();
        return rt_cond_delete(cond);
    }

    static inline int rtos_cond_wait(rt_cond_t *cond, rt_mutex_t *mutex)
    {
        CHK_XENO_CALL();
        int ret = rt_cond_wait(cond, mutex, TM_INFINITE);
        if (ret == 0)
            return 0;
        return -1;
    }

    static inline int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, NANO_TIME abs_time)
    {
        CHK_XENO_CALL();
        // BC: support Xenomai < 2.5.0
#if ((CONFIG_XENO_VERSION_MAJOR*1000)+(CONFIG_XENO_VERSION_MINOR*100)+CONFIG_XENO_REVISION_LEVEL) < 2500
        return rt_cond_wait(cond, mutex, rt_timer_ns2ticks(abs_time) - rt_timer_read() );
#else
        return rt_cond_wait_until(cond, mutex, rt_timer_ns2ticks(abs_time) );
#endif
    }

    static inline int rtos_cond_broadcast(rt_cond_t *cond)
    {
        CHK_XENO_CALL();
        return rt_cond_broadcast(cond);
    }


#define rtos_printf printf

#ifdef __cplusplus
}
#endif

#endif
