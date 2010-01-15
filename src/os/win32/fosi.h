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

#ifdef __cplusplus
extern "C"
{
#endif

#define _XOPEN_SOURCE 600   // use all Posix features.
#include <windows.h>
#undef interface	// To avoid name clash with namespace interface and Windows SDK objbase.h

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

    typedef struct {
      HANDLE handle;
      DWORD threadId;

      NANO_TIME periodMark;
      NANO_TIME period;

      int sched_type; // currently not used

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

    inline int rtos_nanosleep( const TIME_SPEC * rqtp, TIME_SPEC * rmtp )
    {
		NANO_TIME start = rtos_get_time_ns();
		NANO_TIME t = (NANO_TIME)rqtp->tv_sec * 1000000000LL + rqtp->tv_nsec;
        timeBeginPeriod(1);
		if (t > 3000000L) Sleep((DWORD)(t/1000000L) - 1);
		timeEndPeriod(1);
		while(rtos_get_time_ns() - start < t) Sleep(0);
        return 0;
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

	//typedef sem_t rt_sem_t;

    typedef struct {
    	HANDLE lock; // used to lock acces to the count value of the semaphore
    	HANDLE sem;
    	int count;
    } rt_sem_t;

	inline int lockSem( rt_sem_t *m){
		if( WaitForSingleObject(m->lock, INFINITE) == WAIT_OBJECT_0 )
			return 1;
		return 0;
	}
	inline int unlockSem(rt_sem_t *m){
		if( !ReleaseMutex(m->lock) )
			return 1;
		return 0;
	};

    //typedef HANDLE rt_sem_t;

    static inline void printSem(rt_sem_t* m){
    	DWORD tid = GetCurrentThreadId();
    	printf("T:%u -> S:%p  ", (unsigned int)tid,m);
    }

    static inline int rtos_sem_init(rt_sem_t* m, int value )
    {
        assert(m != NULL);
        rt_sem_t sem;
        sem.lock = CreateMutex(NULL, FALSE, NULL);
        if( sem.lock == NULL )
        	return -1;
        sem.sem = CreateSemaphore(NULL, value, 100000, NULL);
        if( sem.sem == NULL ){
        	CloseHandle( sem.lock );
        	return -1;
        }
        sem.count = value;
        *m = sem;
        // printSem(m);
        // printf(" rtos_sem_init \n" );
        return 0;
    }

    static inline int rtos_sem_destroy(rt_sem_t* m )
    {
    	// printSem(m);
    	// printf(" rtos_sem_destroy \n" );
    	DWORD res = CloseHandle(m->lock);
    	res &= CloseHandle(m->sem);
    	if( res==0 )
    		return -1;
        return 0;
    }

    static inline int rtos_sem_signal(rt_sem_t* m )
    {
    	// printSem(m);
    	// printf(" rtos_sem_signal " );
    	if(!lockSem(m)) return -1;
    	// printf(" L");
    	BOOL res = ReleaseSemaphore(m->sem, 1, NULL);
    	if( res == 0 ){
    		// printf(" FAIL ");
    		unlockSem(m);
    		// printf("UL \n");
    		return -1;
    	}
    	m->count++;
    	//int count = m->count;
    	// printf(" SUCCES count %i", count);
    	unlockSem(m);
    	// printf("UL \n");
        return 0;
    }

    static inline int rtos_sem_wait(rt_sem_t* m )
    {
    	// printSem(m);
    	// printf(" rtos_sem_wait " );
    	if(!lockSem(m)) return -1;
    	// printf(" L ");
    	m->count--;
    	int count = m->count;
    	// printf(" count %i ", count);
    	unlockSem(m);
    	// printf(" UL ");
    	DWORD res = WaitForSingleObject(m->sem, INFINITE);
    	if( res != 0 ){
    		// printf(" FAIL %i", count);
        	if(!lockSem(m)) return -1;
        	// printf(" L ");
        	m->count++;
        	count = m->count;
        	// printf(" count %i", count);
        	unlockSem(m);
        	// printf(" UL \n");
    		return -1;
    	}
    	// printf(" SUCCES \n");
        return 0;
    }

    static inline int rtos_sem_trywait(rt_sem_t* m )
    {
    	// printSem(m);
        //return sem_trywait(m);
    	// printf(" rtos_sem_try_timed \n");
    	if(!lockSem(m)) return -1;
    	DWORD res = WaitForSingleObject(m->sem, 0);
    	if( res!=0 ){
    		unlockSem(m);
    		return -1;
    	}
    	m->count--;
    	unlockSem(m);
        return 0;
    }

    static inline int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay )
    {
    	// printSem(m);
    	// printf("rtos_sem_wait_timed %li \n", (long)(delay/1000000LL) );

    	if(!lockSem(m)) return -1;
    	m->count--;
    	unlockSem(m);
    	DWORD res = WaitForSingleObject(m->sem, (DWORD)(delay)/1000000LL);
    	if( res==WAIT_OBJECT_0 ){
    		return 0;
    	}
    	if(!lockSem(m)) return -1;
    	m->count++;
    	unlockSem(m);
        return -1;
    }

    static inline int rtos_sem_wait_until(rt_sem_t* m, NANO_TIME abs_time )
    {
    	// printSem(m);
    	// printf("rtos_sem_wait_until %li \n", (long)((abs_time-rtos_get_time_ns())/1000000LL));
    	return rtos_sem_wait_timed(m, (abs_time-rtos_get_time_ns()) );
    }

    static inline int rtos_sem_value(rt_sem_t* m )
    {
    	// printSem(m);
    	// printf(" rtos_sem_value " );
    	int count;
    	if(!lockSem(m)) return -1;
    	count = m->count;
    	unlockSem(m);
        return count;
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

#define rtos_printf printf

#ifdef __cplusplus
}

#endif
#endif
