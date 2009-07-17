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

#ifdef _MSC_VER
#include <cstdio>
#include <windows.h>
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
	// Orocos Implementation (i386 specific)
#include "../oro_atomic.h"

    // Time Related
#ifdef _MSC_VER
#include <ctime>
#else // MINGW32
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#endif

    void sleep(long s);
    void usleep(long us);

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

#ifdef _MSC_VER
    //conflicts with another struct under MSVC
    struct oro_timespec {
        time_t tv_sec;
        long tv_nsec;
    };

    typedef struct oro_timespec TIME_SPEC;
#else // MINGW32
    typedef struct timespec TIME_SPEC;
#endif

	// high-resolution time to timespec
	// hrt is in ticks
	inline TIME_SPEC ticks2timespec(TICK_TIME hrt)
	{
		/*LARGE_INTEGER freq;
		if(!QueryPerformanceFrequency(&freq) )
			assert(false);

		TIME_SPEC timevl;
		timevl.tv_sec = hrt / freq.QuadPart;
		timevl.tv_nsec = hrt*( 1000000000LL / freq.QuadPart );
*/
		TIME_SPEC timevl;
		timevl.tv_sec = (time_t)(hrt / 1000000000LL);
		timevl.tv_nsec = (long)(hrt % 1000000000LL);
		return timevl;
	}

    inline NANO_TIME rtos_get_time_ns( void )
    {
    	return GetTickCount() * 1000000LL;
    }

    /**
     * This function should return ticks,
     * but we use ticks == nsecs in userspace
     */
    inline TICK_TIME rtos_get_time_ticks()
    {
    	/*LARGE_INTEGER ticks;
    	if( !QueryPerformanceCounter(&ticks) ){
    		assert(false);
    	}
    	return (TICK_TIME) ticks.QuadPart;
    	*/
    	return rtos_get_time_ns();
    }

    inline int rtos_nanosleep( const TIME_SPEC * rqtp, TIME_SPEC * rmtp )
    {
    	// printf("rtos_nanosleep %li ", (rqtp->tv_sec * 1000L + rqtp->tv_nsec/1000000L));
        Sleep(DWORD(rqtp->tv_sec * 1000L) + rqtp->tv_nsec/1000000L);
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

    //typedef pthread_mutex_t rt_mutex_t;
    //typedef pthread_mutex_t rt_rec_mutex_t;
    typedef HANDLE rt_mutex_t;
    typedef HANDLE rt_rec_mutex_t;

    static inline void printMutex(rt_mutex_t* m){
    	DWORD tid = GetCurrentThreadId();
    	printf("T:%u -> S:%p  ", (unsigned int)tid,m);
    }

    static inline int rtos_mutex_init(rt_mutex_t* m)
    {
        assert(m != NULL);
        *m = CreateMutex(NULL, FALSE, NULL);
    	// printMutex(m);
    	// printf(" rtos_mutex_init \n" );
        return *m == NULL;
    }

    static inline int rtos_mutex_destroy(rt_mutex_t* m )
    {
    	// printMutex(m);
    	// printf(" rtos_mutex_destroy \n" );
        return !CloseHandle(*m);
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
    	// printMutex(m);
    	// printf(" rtos_mutex_lock \n" );
    	if( WaitForSingleObject(*m, INFINITE) == WAIT_OBJECT_0 )
    		return 0;
        return -1;
    }

    static inline int rtos_mutex_unlock( rt_mutex_t* m)
    {
    	// printMutex(m);
    	// printf(" rtos_mutex_unlock \n" );
    	if( ReleaseMutex(*m) == 0)
    		return -1;
        return 0;
    }

    static inline int rtos_mutex_trylock( rt_mutex_t* m)
    {
    	// printMutex(m);
    	// printf(" rtos_mutex_trylock \n" );
    	if( WaitForSingleObject(*m, 0) == WAIT_OBJECT_0 )
    		return 0;
        return -1;
    }

    static inline int rtos_mutex_rec_trylock( rt_rec_mutex_t* m)
    {
        return rtos_mutex_trylock(m);
    }

    static inline int rtos_mutex_rec_lock( rt_rec_mutex_t* m)
    {
        return rtos_mutex_lock(m);
    }

    static inline int rtos_mutex_rec_unlock( rt_rec_mutex_t* m)
    {
        return rtos_mutex_unlock(m);
    }

#define rtos_printf printf

#ifdef __cplusplus
}

#endif
#endif
