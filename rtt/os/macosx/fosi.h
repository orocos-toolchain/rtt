/***************************************************************************
                        fosi.h -  description
                           -------------------
    begin                : Mon June 10 2002
    copyright            : (C) 2006 Peter Soetens
    copyright            : (C) 2008 Klaas Gadeyne
    email                : peter.soetens@fmtc.be

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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef __FOSI_H
#define __FOSI_H

#define HAVE_FOSI_API

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _DARWIN_C_SOURCE
#define _DARWIN_C_SOURCE
#endif

#include <stdio.h>
#include <pthread.h>
#include <mach/mach_init.h>
#include <mach/task.h>

#include <errno.h>
#include <stdbool.h>
#include <unistd.h>

#include <limits.h>
#include <float.h>
#include <assert.h>

    // Time Related
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#ifndef RTT_VERIFY
#if defined(NDEBUG)
# define RTT_VERIFY(expr) ((void)(expr))
#else
# define RTT_VERIFY(expr) assert(expr)
#endif
#endif

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

        NANO_TIME periodMark;
        NANO_TIME period;

        char* name;

        int priority;
        int wait_policy;
    } RTOS_TASK;


#define ORO_SCHED_RT    SCHED_FIFO /** Posix FIFO scheduler */
#define ORO_SCHED_OTHER SCHED_OTHER /** Posix normal scheduler */

#ifndef CLOCK_REALTIME
    /* fake clock_gettime for systems like darwin */
    #define  CLOCK_REALTIME 0
    static inline int clock_gettime(int clk_id /*ignored*/, struct timespec *tp)
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
#endif

    // high-resolution time to timespec
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
    static inline NANO_TIME rtos_get_time_ticks(void)
    {
        return rtos_get_time_ns();
    }

    static inline int rtos_nanosleep( const TIME_SPEC * rqtp, TIME_SPEC * rmtp )
    {
        //    return usleep(rqtp->tv_nsec/1000L);
        return nanosleep( rqtp, rmtp );
    }

    static inline long long nano2ticks( long long nano )
    {
        return nano;
    }

    static inline long long ticks2nano( long long count )
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
        mach_timespec_t mach_delayvl = {
#ifdef __cplusplus
            static_cast<unsigned int>(delayvl.tv_sec),
            static_cast<clock_res_t> (delayvl.tv_nsec)
#else
            delayvl.tv_sec,
            delayvl.tv_nsec
#endif
                                        };

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

        /// \todo avoid race condition where arg_time < now

        // calculate delay from abs_time
        delayvl.tv_sec = arg_time.tv_sec - timevl.tv_sec;
        delayvl.tv_nsec = arg_time.tv_nsec - timevl.tv_nsec;
		// tv_nsec is signed long in 10.6 (see sys/_structs.h)
        if ( delayvl.tv_nsec >= 1000000000) {
            ++delayvl.tv_sec;
            delayvl.tv_nsec -= 1000000000;
        }
        if ( delayvl.tv_nsec < 0) {
            --delayvl.tv_sec;
            delayvl.tv_nsec += 1000000000;
        }

        assert( 0 <= delayvl.tv_sec);
        assert( 0 <= delayvl.tv_nsec);
        assert( delayvl.tv_nsec < 1000000000 );

        mach_timespec_t mach_delayvl = {
#ifdef __cplusplus
            static_cast<unsigned int>(delayvl.tv_sec),
            static_cast<clock_res_t> (delayvl.tv_nsec)
#else
            delayvl.tv_sec,
            delayvl.tv_nsec
#endif
                                        };
        int rc = semaphore_timedwait( *m, mach_delayvl);
        // map to return values from gnulinux, and expected by the calling layer
        return (KERN_OPERATION_TIMED_OUT == rc ? -1 : 0);
    }

    // semaphore_value is not supported on darwin
    /*     static inline int rtos_sem_value(rt_sem_t* m ) */
    /*     { */
    /* 		int val = 0; */
    /*         if ( sem_getvalue(m, &val) == 0) */
    /* 			return val; */
    /* 		return -1; */
    /*     } */

    // Mutex functions


    // Note: Apple's implementation of pthread_mutex_t does not have pthread_mutex_timedlock.
    // Workaround: Pair the mutex with a condition variable that is signaled whenever the mutex is unlocked.
    // This is also how boost::timed_mutex is implemented for the case BOOST_THREAD_USES_PTHREAD_TIMEDLOCK is
    // not defined (cf. https://github.com/boostorg/thread/blob/boost-1.62.0/include/boost/thread/pthread/mutex.hpp#L181).
    typedef struct {
        pthread_mutex_t m;
        pthread_cond_t cond;
        bool is_locked;
    } rt_mutex_t;
    typedef struct {
        pthread_mutex_t m;
        pthread_cond_t cond;
        bool is_locked;
        pthread_t owner;
        unsigned count;
    } rt_rec_mutex_t;

    #define _RTOS_INTERNAL_MUTEX_LOCK \
      RTT_VERIFY(!(ret = pthread_mutex_lock((&(m->m))))); \
      if (ret != 0) return ret

    #define _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN \
      RTT_VERIFY(!pthread_mutex_unlock(&(m->m))); \
      return

    static inline int rtos_mutex_init(rt_mutex_t* m)
    {
        pthread_mutexattr_t attr;
        int ret = 0;

        RTT_VERIFY(!(pthread_mutexattr_init(&attr)));
        if (ret != 0) return ret;

        // The mutex only protects the internal flags, so no use to enable priority inheritance.
        // A thread waiting on the condition variable in pthread_cond_wait would not inherit the
        // priority of the thread that holds the mutex.

        // initialize mutex
        RTT_VERIFY(!(ret = pthread_mutex_init(&(m->m), &attr)));
        if (ret != 0) {
            RTT_VERIFY(!pthread_mutexattr_destroy(&attr));
            return ret;
        }
        pthread_mutexattr_destroy(&attr);

        // initialize condition variable (for rtos_mutex_lock_until)
        RTT_VERIFY(!(ret = pthread_cond_init(&(m->cond), NULL)));
        if (ret != 0) {
            RTT_VERIFY(!pthread_mutex_destroy(&(m->m)));
            return ret;
        }

        m->is_locked = false;
        return ret;
    }

    static inline int rtos_mutex_destroy(rt_mutex_t* m )
    {
        RTT_VERIFY(!pthread_cond_destroy(&(m->cond)));
        int ret = 0;
        RTT_VERIFY(!(ret = pthread_mutex_destroy(&(m->m))));
        return ret;
    }

    static inline int rtos_mutex_rec_init(rt_rec_mutex_t* m)
    {
        m->count = 0;
        return rtos_mutex_init((rt_mutex_t *) m);
    }

    static inline int rtos_mutex_rec_destroy(rt_rec_mutex_t* m )
    {
        return rtos_mutex_destroy((rt_mutex_t *) m);
    }

    static inline int rtos_mutex_lock( rt_mutex_t* m)
    {
        int ret = 0;
        _RTOS_INTERNAL_MUTEX_LOCK;

        while (m->is_locked) {
            RTT_VERIFY(!(ret = pthread_cond_wait(&(m->cond), &(m->m))));
            if (ret) {
                _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ret;
            }
        }

        m->is_locked = true;
        _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ret;
    }

    static inline int rtos_mutex_rec_lock( rt_rec_mutex_t* m)
    {
        int ret = 0;
        _RTOS_INTERNAL_MUTEX_LOCK;

        if (m->is_locked && pthread_equal(m->owner, pthread_self())) {
            ++(m->count);
            _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ret;
        }

        while (m->is_locked) {
            RTT_VERIFY(!(ret = pthread_cond_wait(&(m->cond), &(m->m))));
            if (ret) {
                _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ret;
            }
        }

        m->is_locked = true;
        RTT_VERIFY(m->count == 0);
        ++(m->count);
        m->owner = pthread_self();
        _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ret;
    }

    static inline int rtos_mutex_lock_until( rt_mutex_t* m, NANO_TIME abs_time)
    {
        TIME_SPEC arg_time = ticks2timespec( abs_time );
        int ret = 0;
        _RTOS_INTERNAL_MUTEX_LOCK;

        while (m->is_locked)
        {
            ret = pthread_cond_timedwait(&(m->cond), &(m->m), &arg_time);
            if (ret == ETIMEDOUT) {
                break;
            }
            RTT_VERIFY(!ret);
        }

        if (m->is_locked) {
            _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ETIMEDOUT;
        }

        m->is_locked = true;
        _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ret;
    }

    static inline int rtos_mutex_rec_lock_until( rt_rec_mutex_t* m, NANO_TIME abs_time)
    {
        TIME_SPEC arg_time = ticks2timespec( abs_time );
        int ret = 0;
        _RTOS_INTERNAL_MUTEX_LOCK;

        if (m->is_locked && pthread_equal(m->owner, pthread_self())) {
            ++(m->count);
            _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ret;
        }

        while (m->is_locked)
        {
            ret = pthread_cond_timedwait(&(m->cond), &(m->m), &arg_time);
            if (ret == ETIMEDOUT) {
                break;
            }
            RTT_VERIFY(!ret);
        }

        if (m->is_locked) {
            _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ETIMEDOUT;
        }

        m->is_locked = true;
        RTT_VERIFY(m->count == 0);
        ++(m->count);
        m->owner = pthread_self();
        _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ret;
    }

    static inline int rtos_mutex_trylock( rt_mutex_t* m)
    {
        int ret = 0;
        _RTOS_INTERNAL_MUTEX_LOCK;

        if (m->is_locked) {
            _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN EBUSY;
        }

        m->is_locked = true;
        _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ret;
    }

    static inline int rtos_mutex_trylock_for( rt_mutex_t* m, NANO_TIME relative_time)
    {
        return rtos_mutex_lock_until(m, rtos_get_time_ns() + relative_time);
    }

    static inline int rtos_mutex_rec_trylock( rt_rec_mutex_t* m)
    {
        int ret = 0;
        _RTOS_INTERNAL_MUTEX_LOCK;

        if (m->is_locked && !pthread_equal(m->owner, pthread_self())) {
            _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN EBUSY;
        }

        m->is_locked = true;
        ++(m->count);
        m->owner = pthread_self();
        _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ret;
    }

    static inline int rtos_mutex_rec_trylock_for( rt_rec_mutex_t* m, NANO_TIME relative_time)
    {
        return rtos_mutex_rec_lock_until(m, rtos_get_time_ns() + relative_time);
    }

    static inline int rtos_mutex_unlock( rt_mutex_t* m)
    {
        int ret = 0;
        _RTOS_INTERNAL_MUTEX_LOCK;

        m->is_locked = false;

        RTT_VERIFY(!pthread_cond_signal(&(m->cond)));
        _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ret;
    }

    static inline int rtos_mutex_rec_unlock( rt_rec_mutex_t* m)
    {
        int ret = 0;
        _RTOS_INTERNAL_MUTEX_LOCK;

        if (--(m->count) == 0) {
            m->is_locked = false;
        }

        RTT_VERIFY(!pthread_cond_signal(&(m->cond)));
        _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN ret;
    }

    #undef _RTOS_INTERNAL_MUTEX_LOCK
    #undef _RTOS_INTERNAL_MUTEX_UNLOCK_AND_RETURN

    typedef pthread_cond_t rt_cond_t;

    static inline int rtos_cond_init(rt_cond_t *cond)
    {
        int ret = 0;
        RTT_VERIFY(!(ret = pthread_cond_init(cond, NULL)));
        return ret;
    }

    static inline int rtos_cond_destroy(rt_cond_t *cond)
    {
        int ret = 0;
        RTT_VERIFY(!(ret = pthread_cond_destroy(cond)));
        return ret;
    }

    static inline int rtos_cond_wait(rt_cond_t *cond, rt_mutex_t *mutex)
    {
        int ret = 0;
        RTT_VERIFY(!(ret = pthread_mutex_lock((&(mutex->m)))));
        if (ret != 0) return ret;

        if (!mutex->is_locked) {
            RTT_VERIFY(!(ret = pthread_mutex_unlock((&(mutex->m)))));
            return EINVAL;
        }

        mutex->is_locked = false;
        RTT_VERIFY(!pthread_cond_signal(&(mutex->cond)));

        RTT_VERIFY(!(ret = pthread_cond_wait(cond, &(mutex->m))));

        mutex->is_locked = true;

        RTT_VERIFY(!pthread_mutex_unlock(&(mutex->m)));
        return ret;
    }

    static inline int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, NANO_TIME abs_time)
    {
        int ret = 0;
        RTT_VERIFY(!(ret = pthread_mutex_lock((&(mutex->m)))));
        if (ret != 0) return ret;

        if (!mutex->is_locked) {
            RTT_VERIFY(!(ret = pthread_mutex_unlock((&(mutex->m)))));
            return EINVAL;
        }

        mutex->is_locked = false;
        RTT_VERIFY(!pthread_cond_signal(&(mutex->cond)));

        TIME_SPEC arg_time = ticks2timespec( abs_time );
        ret = pthread_cond_timedwait(cond, &(mutex->m), &arg_time);

        mutex->is_locked = true;

        if (ret == ETIMEDOUT) {
            RTT_VERIFY(!pthread_mutex_unlock(&(mutex->m)));
            return ret;
        }
        RTT_VERIFY(!ret);

        RTT_VERIFY(!pthread_mutex_unlock(&(mutex->m)));
        return ret;
    }

    static inline int rtos_cond_broadcast(rt_cond_t *cond)
    {
        int ret = 0;
        RTT_VERIFY(!(ret = pthread_cond_broadcast(cond)));
        return ret;
    }

    static inline void rtos_enable_rt_warning(void)
    {
    }

    static inline void rtos_disable_rt_warning(void)
    {
    }

#define rtos_printf printf

#ifdef __cplusplus
}

#endif
#endif
