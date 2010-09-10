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
 */

#ifndef __FOSI_H
#define __FOSI_H

#define HAVE_FOSI_API

#ifdef _MSC_VER
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <climits>
#include <cfloat>
#include <cassert>
#else // MINGW32
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <assert.h>
#endif

#include "../oro_limits.h"
#include "../../rtt-config.h"

    // Time Related
#ifdef _MSC_VER
#include <ctime>
#else // MINGW32
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "dlfcn.h"

    RTT_API unsigned int sleep(unsigned int seconds);
	
#if __GNUC__ != 4
    RTT_API int usleep(unsigned int us);
#endif

    typedef long long NANO_TIME;
    typedef long long TICK_TIME;

    // TODO
    const TICK_TIME InfiniteTicks = LLONG_MAX;
    const NANO_TIME InfiniteNSecs = LLONG_MAX;
    const double    InfiniteSeconds = DBL_MAX;

#define ORO_WAIT_ABS 0 /** rtos_task_wait_period may wait less than the duration required to pad the period to 
                            catch-up with overrun timesteps (wait according to an absolute timeline) */
#define ORO_WAIT_REL 1 /** rtos_task_wait_period will always pad the current timestep to the desired period, 
                            regardless of previous overruns (wait according to a relative timeline) */

    typedef struct {
      HANDLE handle;
      DWORD threadId;

      NANO_TIME periodMark;
      NANO_TIME period;

      int sched_type; // currently not used
      int wait_policy;

      char* name;
    } RTOS_TASK;

#define ORO_SCHED_RT    0 /** Linux FIFO scheduler */
#define ORO_SCHED_OTHER 1 /** Linux normal scheduler */

    //conflicts with another struct under MSVC
    struct oro_timespec {
        long tv_sec;
        long tv_nsec;
    };

    typedef struct oro_timespec TIME_SPEC;

	// high-resolution time to timespec
	// hrt is in ticks
	inline TIME_SPEC ticks2timespec(TICK_TIME hrt)
	{
		TIME_SPEC timevl;
		timevl.tv_sec = (long)(hrt / 1000000000LL);
		timevl.tv_nsec = (long)(hrt % 1000000000LL);
		return timevl;
	}

    inline NANO_TIME rtos_get_time_ns( void )
    {
		LARGE_INTEGER freq;
		LARGE_INTEGER ticks;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&ticks);

		return(NANO_TIME)(((double)ticks.QuadPart * 1000000000LL) / (double)freq.QuadPart);
    }

    /**
     * This function should return ticks,
     * but we use ticks == nsecs in userspace
     */
    inline TICK_TIME rtos_get_time_ticks()
    {
    	return rtos_get_time_ns();
    }

    inline int win32_nanosleep(long long nano)
    {
		NANO_TIME start = rtos_get_time_ns();
        timeBeginPeriod(1);
		if (nano > 3000000L) Sleep((DWORD)(nano/1000000L) - 1);
		timeEndPeriod(1);
		while(rtos_get_time_ns() - start < nano) Sleep(0);
        return 0;
    }

    inline int rtos_nanosleep( const TIME_SPEC * rqtp, TIME_SPEC * rmtp )
    {
		return win32_nanosleep((NANO_TIME)rqtp->tv_sec * 1000000000LL + rqtp->tv_nsec);
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

    // Semaphore functions

    typedef HANDLE rt_sem_t;

    static inline int rtos_sem_init(rt_sem_t* m, int value )
    {
        *m = CreateSemaphore(NULL, value, 100000, NULL);
        return (*m != NULL) ? 0 : -1; 
    }

    static inline int rtos_sem_destroy(rt_sem_t* m )
    {
        CloseHandle(*m);
        return 0;
    }

    static inline int rtos_sem_signal(rt_sem_t* m )
    {
        return (ReleaseSemaphore(*m, 1, NULL) == 0) ? -1 : 0;
    }

    static inline int rtos_sem_wait(rt_sem_t* m )
    {
        return (WaitForSingleObject(*m, INFINITE) == WAIT_FAILED) ? -1 : 0;
    }

    static inline int rtos_sem_trywait(rt_sem_t* m )
    {
        return (WaitForSingleObject(*m, 0) == WAIT_TIMEOUT) ? -1 : 0;
    }

    static inline int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay )
    {
        return (WaitForSingleObject(*m, (DWORD)(delay/1000000)) == WAIT_TIMEOUT) ? -1 : 0;
    }

    static inline int rtos_sem_wait_until(rt_sem_t* m, NANO_TIME abs_time )
    {
        if (abs_time < rtos_get_time_ns()) return -1;
        NANO_TIME delay = abs_time - rtos_get_time_ns();
        return rtos_sem_wait_timed(m, delay);
    }

    static inline int rtos_sem_value(rt_sem_t* m )
    {
      long previous;

      switch (WaitForSingleObject(*m, 0)) 
      {
        case WAIT_OBJECT_0:
          if (!ReleaseSemaphore(*m, 1, &previous)) return -1;
          return previous + 1;
        case WAIT_TIMEOUT: return 0;
        default:
          return -1;
      }
    }

    // Mutex functions

    typedef CRITICAL_SECTION rt_mutex_t;
    typedef CRITICAL_SECTION rt_rec_mutex_t;

    static inline int rtos_mutex_init(rt_mutex_t* m)
    {
		InitializeCriticalSection(m);
        return 0;
    }

    static inline int rtos_mutex_destroy(rt_mutex_t* m )
    {
		DeleteCriticalSection(m);
        return 0;
    }

    static inline int rtos_mutex_rec_init(rt_rec_mutex_t* m)
    {
        return rtos_mutex_init(m);
    }

    static inline int rtos_mutex_rec_destroy(rt_rec_mutex_t* m )
    {
        return rtos_mutex_destroy(m);
    }

    static inline int rtos_mutex_lock( rt_mutex_t* m)
    {
		  EnterCriticalSection(m);
    	return 0;
    }

    static inline int rtos_mutex_unlock( rt_mutex_t* m)
    {
    	LeaveCriticalSection(m);
		  return 0;
    }

    static inline int rtos_mutex_trylock( rt_mutex_t* m)
    {
		if(TryEnterCriticalSection(m) != 0) return 0;
		return -1;
    }

    static inline int rtos_mutex_rec_trylock( rt_rec_mutex_t* m)
    {
        return rtos_mutex_trylock(m);
    }

    static inline int rtos_mutex_lock_until( rt_mutex_t* m, NANO_TIME abs_time)
    {
		while (ticks2nano(rtos_get_time_ticks()) < abs_time) 
			if (rtos_mutex_trylock(m) == 0) return 0;
		return -1;
    }

    static inline int rtos_mutex_rec_lock_until( rt_mutex_t* m, NANO_TIME abs_time)
    {
        return rtos_mutex_lock_until(m, abs_time);
    }

    static inline int rtos_mutex_rec_lock( rt_rec_mutex_t* m)
    {
        return rtos_mutex_lock(m);
    }

    static inline int rtos_mutex_rec_unlock( rt_rec_mutex_t* m)
    {
        return rtos_mutex_unlock(m);
    }

    static inline void rtos_enable_rt_warning()
    {
    }

    static inline void rtos_disable_rt_warning()
    {
    }

    // Condition variable implementation (SetEvent solution from ACE framework)

    typedef struct 
    {
      enum { SIGNAL = 0, BROADCAST = 1, MAX_EVENTS = 2 };

      HANDLE events_[MAX_EVENTS];

      // Count of the number of waiters.
      unsigned int waiters_count_;
      CRITICAL_SECTION waiters_count_lock_;

    } rt_cond_t;

    static inline int rtos_cond_init(rt_cond_t *cond)
    {
      // Initialize the count to 0.
      cond->waiters_count_ = 0;

      // Create an auto-reset event.
      cond->events_[rt_cond_t::SIGNAL] = CreateEvent (NULL,  // no security
                                         FALSE, // auto-reset event
                                         FALSE, // non-signaled initially
                                         NULL); // unnamed

      // Create a manual-reset event.
      cond->events_[rt_cond_t::BROADCAST] = CreateEvent (NULL,  // no security
                                            TRUE,  // manual-reset
                                            FALSE, // non-signaled initially
                                            NULL); // unnamed

      InitializeCriticalSection(&cond->waiters_count_lock_);
      return 0;
    }

    static inline int rtos_cond_destroy(rt_cond_t *cond)
    {
      CloseHandle(cond->events_[rt_cond_t::SIGNAL]);
      CloseHandle(cond->events_[rt_cond_t::BROADCAST]);

      DeleteCriticalSection(&cond->waiters_count_lock_);

      return 0;
    }

    static inline int rtos_cond_timedwait_internal(rt_cond_t *cond, rt_mutex_t *external_mutex, DWORD ms)
    {
      // Avoid race conditions.
      EnterCriticalSection (&cond->waiters_count_lock_);
      cond->waiters_count_++;
      LeaveCriticalSection (&cond->waiters_count_lock_);

      // It's ok to release the <external_mutex> here since Win32
      // manual-reset events maintain state when used with
      // <SetEvent>.  This avoids the "lost wakeup" bug...
      LeaveCriticalSection (external_mutex);

      // Wait for either event to become signaled due to <pthread_cond_signal>
      // being called or <pthread_cond_broadcast> being called.
      timeBeginPeriod(1);
      int result = WaitForMultipleObjects (2, cond->events_, FALSE, ms);
      timeEndPeriod(1);

      EnterCriticalSection (&cond->waiters_count_lock_);
      cond->waiters_count_--;
      bool last_waiter = result == WAIT_OBJECT_0 + rt_cond_t::BROADCAST && cond->waiters_count_ == 0;
      LeaveCriticalSection (&cond->waiters_count_lock_);

      // Some thread called <pthread_cond_broadcast>.
      if (last_waiter)
        // We're the last waiter to be notified or to stop waiting, so
        // reset the manual event. 
        ResetEvent (cond->events_[rt_cond_t::BROADCAST]); 

      // Reacquire the <external_mutex>.
      EnterCriticalSection (external_mutex);

      return 0;
    }

    static inline int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, NANO_TIME abs_time)
    {
      return rtos_cond_timedwait_internal(cond, mutex, (DWORD)(abs_time / 1000000));
    }

    static inline int rtos_cond_wait(rt_cond_t *cond, rt_mutex_t *mutex)
    {
      return rtos_cond_timedwait_internal(cond, mutex, INFINITE);
    }

    static inline int rtos_cond_broadcast(rt_cond_t *cond)
    {
      // Avoid race conditions.
      EnterCriticalSection (&cond->waiters_count_lock_);
      bool have_waiters = cond->waiters_count_ > 0;
      LeaveCriticalSection (&cond->waiters_count_lock_);

      if (have_waiters) SetEvent (cond->events_[rt_cond_t::BROADCAST]);

      return 0;
    }

    static inline int rtos_cond_signal (rt_cond_t *cond)
    {
      // Avoid race conditions.
      EnterCriticalSection (&cond->waiters_count_lock_);
      bool have_waiters = cond->waiters_count_ > 0;
      LeaveCriticalSection (&cond->waiters_count_lock_);

      if (have_waiters) SetEvent (cond->events_[rt_cond_t::SIGNAL]);

      return 0;
    }

#define rtos_printf printf

int setenv(const char *name, const char *value, int overwrite);

#ifdef __cplusplus
}

#endif
#endif
