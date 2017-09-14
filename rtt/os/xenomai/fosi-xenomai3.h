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


#ifndef _XENO3_FOSI_H
#define _XENO3_FOSI_H

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
// HACK: define this to avoid including copperplate/debug.h
// That causes troubles with log4cpp (also defining debug())
#define _COPPERPLATE_DEBUG_H

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
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <alchemy/mutex.h>
#include <alchemy/sem.h>
#include <alchemy/cond.h>

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

#define ORO_WAIT_ABS 0 /** Not supported for the xenomai target */
#define ORO_WAIT_REL 1 /** Not supported for the xenomai target */

    // hrt is in ticks
static inline TIME_SPEC ticks2timespec(TICK_TIME hrt)
{
    TIME_SPEC timevl;
    timevl.tv_sec = rt_timer_ticks2ns(hrt) / 1000000000LL;
    timevl.tv_nsec = rt_timer_ticks2ns(hrt) % 1000000000LL;
    return timevl;
}

    // hrt is in ticks
static inline TICK_TIME timespec2ticks(const TIME_SPEC* ts)
{
    return  rt_timer_ns2ticks(ts->tv_nsec + ts->tv_sec*1000000000LL);
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

static inline TICK_TIME rtos_get_time_ticks(void) { return rt_timer_read(); }

//WARNING : The resolution can be changed in alchemy.Need a way to get that constant
static inline TICK_TIME ticksPerSec(void) { return rt_timer_ns2ticks( 1000 * 1000 * 1000 ); }

static inline TICK_TIME nano2ticks(NANO_TIME t) { return rt_timer_ns2ticks(t); }
static inline NANO_TIME ticks2nano(TICK_TIME t) { return rt_timer_ticks2ns(t); }

static inline int rtos_nanosleep(const TIME_SPEC *rqtp, TIME_SPEC *rmtp)
    {
        CHK_XENO_CALL();
        RTIME ticks = rqtp->tv_sec * 1000000000LL + rqtp->tv_nsec;
        int ret = rt_task_sleep( rt_timer_ns2ticks(ticks) );
        switch(ret)
        {
            case -EINTR: printf("rtos_nanosleep returned -EINTR is returned if rt_task_unblock() was called for the current task.\n"); break;
            case -ETIMEDOUT: printf("rtos_nanosleep returned -ETIMEDOUT is returned if date has already elapsed.\n"); break;
            case -EPERM: printf("rtos_nanosleep returned -EPERM is returned if this service was called from an invalid context.\n"); break;
        }
        return 0;
    }

    static inline int rtos_sem_init(rt_sem_t* m, int value )
    {
        CHK_XENO_CALL();
        int ret = rt_sem_create( m, 0, value, S_PRIO);
        switch(ret)
        {
            case -EINVAL: printf("rtos_sem_init returned -EINVAL is returned if icount is non-zero and S_PULSE is set in mode, or mode is otherwise invalid.\n"); break;
            case -ENOMEM: printf("rtos_sem_init returned -ENOMEM is returned if the system fails to get memory from the main heap in order to create the semaphore.\n"); break;
            case -EEXIST: printf("rtos_sem_init returned -EEXIST is returned if the name is conflicting with an already registered semaphore.\n"); break;
            case -EPERM: printf("rtos_sem_init returned -EPERM is returned if this service was called from an asynchronous context.\n"); break;
        }
        return ret;
    }

    static inline int rtos_sem_destroy(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        int ret = rt_sem_delete( m );
        switch(ret)
        {
            case -EINVAL: printf("rtos_sem_destroy returned -EINVAL is returned if sem is not a valid semaphore descriptor.\n"); break;
            case -EPERM: printf("rtos_sem_destroy returned -EPERM is returned if this service was called from an asynchronous context.\n"); break;
        }
        return ret;
    }

    static inline int rtos_sem_signal(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        int ret = rt_sem_v( m );
        switch(ret)
        {
            case -ETIMEDOUT: printf("rtos_sem_wait_timed returned -ETIMEDOUT is returned if abs_timeout is reached before the request is satisfied.\n"); break;
            case -EWOULDBLOCK: printf("rtos_sem_wait_timed returned -EWOULDBLOCK is returned if abs_timeout is { .tv_sec = 0, .tv_nsec = 0 } and the semaphore count is zero on entry.\n"); break;
            case -EINTR: printf("rtos_sem_wait_timed returned -EINTR is returned if rt_task_unblock() was called for the current task before the request is satisfied.\n"); break;
            case -EINVAL: printf("rtos_sem_wait_timed returned -EINVAL is returned if sem is not a valid semaphore descriptor.\n"); break;
            case -EIDRM: printf("rtos_sem_wait_timed returned -EIDRM is returned if sem is deleted while the caller was sleeping on it. In such a case, sem is no more valid upon return of this service.\n"); break;
            case -EPERM: printf("rtos_sem_wait_timed returned -EPERM is returned if this service should block, but was not called from a Xenomai thread.\n"); break;
        }
        return ret;
    }

    static inline int rtos_sem_wait(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        int ret = rt_sem_p_timed( m, NULL);
        switch(ret)
        {
            case -ETIMEDOUT: printf("rtos_sem_wait_timed returned -ETIMEDOUT is returned if abs_timeout is reached before the request is satisfied.\n"); break;
            case -EWOULDBLOCK: printf("rtos_sem_wait_timed returned -EWOULDBLOCK is returned if abs_timeout is { .tv_sec = 0, .tv_nsec = 0 } and the semaphore count is zero on entry.\n"); break;
            case -EINTR: printf("rtos_sem_wait_timed returned -EINTR is returned if rt_task_unblock() was called for the current task before the request is satisfied.\n"); break;
            case -EINVAL: printf("rtos_sem_wait_timed returned -EINVAL is returned if sem is not a valid semaphore descriptor.\n"); break;
            case -EIDRM: printf("rtos_sem_wait_timed returned -EIDRM is returned if sem is deleted while the caller was sleeping on it. In such a case, sem is no more valid upon return of this service.\n"); break;
            case -EPERM: printf("rtos_sem_wait_timed returned -EPERM is returned if this service should block, but was not called from a Xenomai thread.\n"); break;
        }
        return ret;
    }

    static inline int rtos_sem_trywait(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        int ret = rt_sem_p( m, TM_NONBLOCK);
        switch(ret)
        {
            case -ETIMEDOUT: printf("rtos_sem_trywait returned -ETIMEDOUT is returned if abs_timeout is reached before the request is satisfied.\n"); break;
            case -EWOULDBLOCK: printf("rtos_sem_trywait returned -EWOULDBLOCK is returned if abs_timeout is { .tv_sec = 0, .tv_nsec = 0 } and the semaphore count is zero on entry.\n"); break;
            case -EINTR: printf("rtos_sem_trywait returned -EINTR is returned if rt_task_unblock() was called for the current task before the request is satisfied.\n"); break;
            case -EINVAL: printf("rtos_sem_trywait returned -EINVAL is returned if sem is not a valid semaphore descriptor.\n"); break;
            case -EIDRM: printf("rtos_sem_trywait returned -EIDRM is returned if sem is deleted while the caller was sleeping on it. In such a case, sem is no more valid upon return of this service.\n"); break;
            case -EPERM: printf("rtos_sem_trywait returned -EPERM is returned if this service should block, but was not called from a Xenomai thread.\n"); break;
        }
        return ret;
    }

    static inline int rtos_sem_value(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        static RT_SEM_INFO sinfo;
        if (rt_sem_inquire(m, &sinfo) == 0 ) {
            return sinfo.count;
        }
        return -1;
    }

    static inline int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay )
    {
        CHK_XENO_CALL();
        int ret = rt_sem_p(m, rt_timer_ns2ticks(delay));
        switch(ret)
        {
            case -ETIMEDOUT: printf("rtos_sem_wait_timed returned -ETIMEDOUT is returned if abs_timeout is reached before the request is satisfied.\n"); break;
            case -EWOULDBLOCK: printf("rtos_sem_wait_timed returned -EWOULDBLOCK is returned if abs_timeout is { .tv_sec = 0, .tv_nsec = 0 } and the semaphore count is zero on entry.\n"); break;
            case -EINTR: printf("rtos_sem_wait_timed returned -EINTR is returned if rt_task_unblock() was called for the current task before the request is satisfied.\n"); break;
            case -EINVAL: printf("rtos_sem_wait_timed returned -EINVAL is returned if sem is not a valid semaphore descriptor.\n"); break;
            case -EIDRM: printf("rtos_sem_wait_timed returned -EIDRM is returned if sem is deleted while the caller was sleeping on it. In such a case, sem is no more valid upon return of this service.\n"); break;
            case -EPERM: printf("rtos_sem_wait_timed returned -EPERM is returned if this service should block, but was not called from a Xenomai thread.\n"); break;
        }
        return ret;
    }

    static inline int rtos_sem_wait_until(rt_sem_t* m, NANO_TIME when )
    {
        CHK_XENO_CALL();
        int ret = rt_sem_p_until(m, rt_timer_ns2ticks(when));
        switch(ret)
        {
            case -ETIMEDOUT: printf("rtos_sem_wait_timed returned -ETIMEDOUT is returned if abs_timeout is reached before the request is satisfied.\n"); break;
            case -EWOULDBLOCK: printf("rtos_sem_wait_timed returned -EWOULDBLOCK is returned if abs_timeout is { .tv_sec = 0, .tv_nsec = 0 } and the semaphore count is zero on entry.\n"); break;
            case -EINTR: printf("rtos_sem_wait_timed returned -EINTR is returned if rt_task_unblock() was called for the current task before the request is satisfied.\n"); break;
            case -EINVAL: printf("rtos_sem_wait_timed returned -EINVAL is returned if sem is not a valid semaphore descriptor.\n"); break;
            case -EIDRM: printf("rtos_sem_wait_timed returned -EIDRM is returned if sem is deleted while the caller was sleeping on it. In such a case, sem is no more valid upon return of this service.\n"); break;
            case -EPERM: printf("rtos_sem_wait_timed returned -EPERM is returned if this service should block, but was not called from a Xenomai thread.\n"); break;
        }
        return ret;
    }

    static inline int rtos_mutex_init(rt_mutex_t* m)
    {
        CHK_XENO_CALL();
        // a Xenomai mutex is always recursive
        int ret = rt_mutex_create(m, 0);
        switch(ret)
        {
            case -ENOMEM: printf("rtos_mutex_init returned -ENOMEM is returned if the system fails to get memory from the main heap in order to create the mutex.\n"); break;
            case -EEXIST: printf("rtos_mutex_init returned -EEXIST is returned if the name is conflicting with an already registered mutex.\n"); break;
            case -EPERM: printf("rtos_mutex_init returned -EPERM is returned if this service was called from an asynchronous context.\n"); break;
        }
        return ret;
    }

    static inline int rtos_mutex_destroy(rt_mutex_t* m )
    {
        CHK_XENO_CALL();
        int ret = rt_mutex_delete(m);
        switch(ret)
        {
            case -EINVAL: printf("rtos_mutex_destroy returned -EINVAL is returned if alarm is not a valid mutex descriptor.\n"); break;
            case -EPERM: printf("rtos_mutex_destroy returned -EPERM is returned if this service was called from an asynchronous context.\n"); break;
            case -EBUSY: printf("rtos_mutex_destroy returned -EBUSY is returned upon an attempt to destroy the object referenced by mutex while it is referenced (for example, while being used in a rt_mutex_acquite(), rt_mutex_acquire_timed() or rt_mutex_acquire_until() by another task).\n"); break;
        }
        return ret;
    }

    static inline int rtos_mutex_rec_init(rt_mutex_t* m)
    {
        return rtos_mutex_init(m);
    }

    static inline int rtos_mutex_rec_destroy(rt_mutex_t* m )
    {
        return rtos_mutex_destroy(m);
    }

    static inline int rtos_mutex_lock( rt_mutex_t* m)
    {
        CHK_XENO_CALL();
        int ret = rt_mutex_acquire_timed(m,NULL);
        switch (ret) {
            case -ETIMEDOUT:
                printf( "rt_mutex_lock -ETIreturned MEDOUT is returned if abs_timeout is reached before the mutex is available. \n");
                break;
            case -EWOULDBLOCK:
                printf( "rt_mutex_lock -EWOULreturned DBLOCK is returned if timeout is { .tv_sec = 0, .tv_nsec = 0 } and the mutex is not immediately available.\n");
                break;
            case -EINTR:
                printf( "rt_mutex_lock returned -EINTR is returned if rt_task_unblock() was called for the current task.\n");
                break;
            case -EINVAL:
                printf( "rt_mutex_lock -returned EINVAL is returned if mutex is not a valid mutex descriptor.\n");
                break;
            case -EIDRM:
                printf( "rt_mutex_lock returned -EIDRM is returned if mutex is deleted while the caller was waiting on it. In such event, mutex is no more valid upon return of this service.\n");
                break;
            case -EPERM:
                printf( "rt_mutex_lock returned -EPERM is returned if this service should block, but was not called from a Xenomai thread.\n");
                break;
        }
        return ret;
    }

    static inline int rtos_mutex_trylock( rt_mutex_t* m)
    {
        CHK_XENO_CALL();
        struct RT_MUTEX_INFO info;
        rt_mutex_inquire(m, &info );
        RT_TASK no_task = NO_ALCHEMY_TASK;
        // If no one locked it, no one owns it.
        // So if info.owner is the same as "no task" (returns 0), it is unlocked
        // If different (returns !=0), then someone has it already, and we cannot lock immediately.
        if (rt_task_same(&info.owner,&no_task) == 0) // Already owned, abord
            return 0;

        int ret =  rt_mutex_acquire(m,TM_NONBLOCK);
        switch (ret) {
            case -ETIMEDOUT:
                printf( "rtos_mutex_trylock returned -ETIMEDOUT is returned if abs_timeout is reached before the mutex is available. \n");
                break;
            case -EWOULDBLOCK:
                printf( "rtos_mutex_trylock returned -EWOULDBLOCK is returned if timeout is { .tv_sec = 0, .tv_nsec = 0 } and the mutex is not immediately available.\n");
                break;
            case -EINTR:
                printf( "rtos_mutex_trylock returned -EINTR is returned if rt_task_unblock() was called for the current task.\n");
                break;
            case -EINVAL:
                printf( "rtos_mutex_trylock returned -EINVAL is returned if mutex is not a valid mutex descriptor.\n");
                break;
            case -EIDRM:
                printf( "rtos_mutex_trylock returned -EIDRM is returned if mutex is deleted while the caller was waiting on it. In such event, mutex is no more valid upon return of this service.\n");
                break;
            case -EPERM:
                printf( "rtos_mutex_trylock returned -EPERM is returned if this service should block, but was not called from a Xenomai thread.\n");
                break;
        }
        return ret;
    }

    static inline int rtos_mutex_lock_until( rt_mutex_t* m, NANO_TIME abs_time)
    {
        CHK_XENO_CALL();
        //return rt_mutex_acquire_until(m, rt_timer_ns2ticks(abs_time) );
        //TIME_SPEC arg_time = ticks2timespec( rt_timer_ns2ticks(abs_time) );
        int ret = rt_mutex_acquire_until(m, rt_timer_ns2ticks(abs_time) );
        switch(ret)
        {
            case -ETIMEDOUT: printf("rtos_mutex_lock_until returned -ETIMEDOUT is returned if abs_timeout is reached before the mutex is available.\n");break;
            case -EWOULDBLOCK: printf("rtos_mutex_lock_until returned -EWOULDBLOCK is returned if timeout is { .tv_sec = 0, .tv_nsec = 0 } and the mutex is not immediately available.\n");break;
            case -EINTR: printf("rtos_mutex_lock_until returned -EINTR is returned if rt_task_unblock() was called for the current task.\n");break;
            case -EINVAL: printf("rtos_mutex_lock_until returned -EINVAL is returned if mutex is not a valid mutex descriptor.\n");break;
            case -EIDRM: printf("rtos_mutex_lock_until returned -EIDRM is returned if mutex is deleted while the caller was waiting on it. In such event, mutex is no more valid upon return of this service.\n");break;
            case -EPERM: printf("rtos_mutex_lock_until returned -EPERM is returned if this service should block, but was not called from a Xenomai thread.\n");break;
        }
        return ret;
    }

    static inline int rtos_mutex_unlock( rt_mutex_t* m)
    {
        CHK_XENO_CALL();
        int ret = rt_mutex_release(m);
        switch (ret) {
            case -EINVAL: printf("rtos_mutex_unlock returned -EINVAL is returned if alarm is not a valid mutex descriptor.\n");break;
            case -EPERM: printf("rtos_mutex_unlock returned -EPERM is returned if mutex is not owned by the current task, or more generally if this service was called from a context which cannot own any mutex (e.g. interrupt context).\n");break;
        }
        return ret;
    }

    static inline int rtos_mutex_rec_lock( rt_rec_mutex_t* m)
    {
        return rtos_mutex_lock(m );
    }

    static inline int rtos_mutex_rec_trylock( rt_rec_mutex_t* m)
    {
        return rtos_mutex_trylock(m);
    }

    static inline int rtos_mutex_rec_lock_until( rt_rec_mutex_t* m, NANO_TIME abs_time)
    {
        return rtos_mutex_lock_until(m, abs_time);
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
        int ret = rt_cond_create(cond, 0);
        switch (ret) {
            case -ENOMEM: printf("rtos_cond_init returned -ENOMEM is returned if the system fails to get memory from the main heap in order to create the condition variable.\n"); break;
            case -EEXIST: printf("rtos_cond_init returned -EEXIST is returned if the name is conflicting with an already registered condition variable.\n"); break;
            case -EPERM: printf("rtos_cond_init returned -EPERM is returned if this service was called from an asynchronous context.\n"); break;
        }
        return ret;
    }

    static inline int rtos_cond_destroy(rt_cond_t *cond)
    {
        CHK_XENO_CALL();
        int ret = rt_cond_delete(cond);
        switch (ret) {
            case -EINVAL: printf("rtos_cond_destroy returned -EINVAL is returned if alarm is not a valid condition variable descriptor.\n");break;
            case -EPERM: printf("rtos_cond_destroy returned -EPERM is returned if this service was called from an asynchronous context.\n");break;
            case -EBUSY: printf("rtos_cond_destroy returned -EBUSY is returned upon an attempt to destroy the object referenced by cond while it is referenced (for example, while being used in a rt_cond_wait(), rt_cond_wait_timed() or rt_cond_wait_until() by another task).\n");break;
        }
        return ret;
    }

    static inline int rtos_cond_wait(rt_cond_t *cond, rt_mutex_t *mutex)
    {
        CHK_XENO_CALL();
        int ret = rt_cond_wait_timed(cond, mutex, NULL);
        switch (ret) {
            case -ETIMEDOUT: printf("rtos_cond_wait returned -ETIMEDOUT is returned if abs_timeout is reached before the condition variable is signaled.\n"); break;
            case -EWOULDBLOCK: printf("rtos_cond_wait returned -EWOULDBLOCK is returned if abs_timeout is { .tv_sec = 0, .tv_nsec = 0 } .\n"); break;
            case -EINTR: printf("rtos_cond_wait returned -EINTR is returned if rt_task_unblock() was called for the current task.\n"); break;
            case -EINVAL: printf("rtos_cond_wait returned -EINVAL is returned if cond is not a valid condition variable descriptor.\n"); break;
            case -EIDRM: printf("rtos_cond_wait returned -EIDRM is returned if cond is deleted while the caller was waiting on the condition variable. In such event, cond is no more valid upon return of this service.\n"); break;
            case -EPERM: printf("rtos_cond_wait returned -EPERM is returned if this service should block, but was not called from a Xenomai thread.\n"); break;
        }
        return ret;
    }

    static inline int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, NANO_TIME abs_time)
    {
        CHK_XENO_CALL();

        // if(cond==NULL)
        // {
        //     printf("COnd is NULL\n");
        // }
        // if(mutex == NULL)
        // {
        //     printf("mutex is NULL\n");
        // }
        //
        // struct RT_MUTEX_INFO info;
        // rt_mutex_inquire(mutex, &info );
        // RT_TASK no_task = NO_ALCHEMY_TASK;
        // // If no one locked it, no one owns it.
        // // So if info.owner is the same as "no task" (returns true), it is unlocked
        // // If different (returns !=0), then someone has it already, and we cannot lock immediately.
        // printf("rtos_cond_timedwait mutex is unlocked %d by %d\n",rt_task_same(&info.owner,&no_task), info.owner);
        //
        // printf("rtos_mutex_lock\n");
        // int r = rtos_mutex_lock(mutex);
        // printf("rtos_mutex_lock->> %d\n",r);
        // printf("rtos_mutex_lock again->> %d\n",rt_mutex_acquire(mutex,10e9));
        //
        //
        // rt_mutex_inquire(mutex, &info );
        // printf("rtos_cond_timedwait mutex now locked ? %d\n",rt_task_same(&info.owner,&no_task));

        int ret = rt_cond_wait_until(cond, mutex, rt_timer_ns2ticks(abs_time) );
        //int ret = rt_cond_wait_timed(cond,mutex,NULL);
        switch (ret) {
            case -ETIMEDOUT: printf("rtos_cond_timedwait %lld ns->%lld ticks at time %lld returned -ETIMEDOUT is returned if abs_timeout is reached before the condition variable is signaled.\n",abs_time,rt_timer_ns2ticks(abs_time),rtos_get_time_ns()); break;
            case -EWOULDBLOCK: printf("rtos_cond_timedwait %lld ns->%lld ticks at time %lld returned -EWOULDBLOCK is returned if abs_timeout is { .tv_sec = 0, .tv_nsec = 0 } .\n",abs_time,rt_timer_ns2ticks(abs_time),rtos_get_time_ns()); break;
            case -EINTR: printf("rtos_cond_timedwait %lld ns->%lld ticks at time %lld returned -EINTR is returned if rt_task_unblock() was called for the current task.\n",abs_time,rt_timer_ns2ticks(abs_time),rtos_get_time_ns()); break;
            case -EINVAL: printf("rtos_cond_timedwait %lld ns->%lld ticks at time %lld returned -EINVAL is returned if cond is not a valid condition variable descriptor.\n",abs_time,rt_timer_ns2ticks(abs_time),rtos_get_time_ns()); break;
            case -EIDRM: printf("rtos_cond_timedwait %lld ns->%lld ticks at time %lld returned -EIDRM is returned if cond is deleted while the caller was waiting on the condition variable. In such event, cond is no more valid upon return of this service.\n",abs_time,rt_timer_ns2ticks(abs_time),rtos_get_time_ns()); break;
            case -EPERM: printf("rtos_cond_timedwait %lld ns->%lld ticks at time %lld returned -EPERM is returned if this service should block, but was not called from a Xenomai thread.\n",abs_time,rt_timer_ns2ticks(abs_time),rtos_get_time_ns()); break;
        }
        return ret;
    }

    static inline int rtos_cond_broadcast(rt_cond_t *cond)
    {
        CHK_XENO_CALL();
        int ret = rt_cond_broadcast(cond);
        switch (ret) {
            case -EINVAL: printf("rtos_cond_wait returned -EINVAL is returned if cond is not a valid condition variable descriptor.\n"); break;
        }
        return ret;
    }


#define rtos_printf printf

#ifdef __cplusplus
}
#endif

#endif
