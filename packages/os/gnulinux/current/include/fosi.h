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

#define _XOPEN_SOURCE 600   // use all Posix features.

#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
    struct GNUTask;
    typedef struct GNUTask RTOS_TASK;
	typedef pthread_t      RTOS_THREAD;

	// Orocos Implementation (i386 specific)
#include "oro_atomic.h"
#include "oro_bitops.h"

    // Time Related
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
    typedef long long NANO_TIME;
    typedef long long TICK_TIME;

    typedef struct timespec TIME_SPEC;

    inline NANO_TIME rtos_get_time_ns( void )
    {

        struct timeval tv;

        int res = gettimeofday( &tv, 0 );

        if ( res == -1 )
            printf( "ERROR invoking gettimeofday in fosi.h\n" );

#ifdef __cplusplus 
        //  printf("TimeOFDay: %e ns\n", double(tv.tv_usec*1000LL + tv.tv_sec*1000000000LL));
        return NANO_TIME( tv.tv_sec ) * 1000000000LL + NANO_TIME( tv.tv_usec ) * 1000LL;
#else
        return ( NANO_TIME ) ( tv.tv_sec * 1000000000LL ) + ( NANO_TIME ) ( tv.tv_usec * 1000LL );
#endif 
    }

    /**
     * This function should return ticks,
     * but we use ticks == nsecs in userspace
     */
    inline NANO_TIME systemTimeGet()
    {
        return rtos_get_time_ns();
    }

    /**
     * This function should return ticks,
     * but we use ticks == nsecs in userspace
     */
    inline NANO_TIME systemNSecsTimeGet()
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
		NANO_TIME end;
		TIME_SPEC timevl;
		end = rtos_get_time_ns() + delay;
		// sleep for 11 ms.
		timevl.tv_sec = 0;
		timevl.tv_nsec = 11000000LL;

		while ( sem_trywait(m) != 0 ) {
			nanosleep( &timevl, 0);
			if ( end < rtos_get_time_ns() )
				return sem_trywait(m);
		}
		return 0;
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

    static inline int rtos_mutex_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr  /*__attribute__((__unused__))*/)
    {
        return pthread_mutex_init(m, 0 );
    }

    static inline int rtos_mutex_destroy(rt_mutex_t* m )
    {
        return pthread_mutex_destroy(m);
    }

    static inline int rtos_mutex_rec_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr /* __attribute__((__unused__))*/)
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

    static inline int rtos_mutex_trylock( rt_mutex_t* m)
    {
        return pthread_mutex_trylock(m);
    }

    static inline int rtos_mutex_unlock( rt_mutex_t* m)
    {
        return pthread_mutex_unlock(m);
    }


    typedef pthread_cond_t rt_cond_t;
    static inline int rtos_cond_init(rt_cond_t *cond, pthread_condattr_t *cond_attr)
    {
        return pthread_cond_init(cond, cond_attr);
    }

    static inline int rtos_cond_destroy(rt_cond_t *cond)
    {
        return pthread_cond_destroy(cond);
    }

    static inline int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, const struct timespec *abstime)
    {
        return pthread_cond_timedwait(cond, mutex, abstime);
    }

    static inline int rtos_cond_broadcast(rt_cond_t *cond)
    {
        return pthread_cond_broadcast(cond);
    }

#define rtos_printf printf

    inline void rtos_enable_fpu( pthread_attr_t * pt /*__attribute__((__unused__))*/ )
    {}

    // RT FIFO emulated by a socket
    // this is done because an rtfifo really is a socket (it has 2 endpoints
    // both of which can be written to/read from)

#ifdef __cplusplus
    extern "C"
    {
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <stdlib.h>

        // ugly hack
#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108
#endif


        typedef int ( *fifohandler ) ( unsigned int, int );

        /*
         * Tries to be the equivalent of rtai's rtf_create
         * @param fnr the number of the fifo you wish to create
         * @param bytes the length of the ring buffer associated with the fifo
         *  this parameter is ignored
         * @return 0 if okay
         *  negative otherwise
         */
        int rtosf_create( int fnr, size_t bytes );

        void rtosf_destroy( int fnr );

        int rtosf_get( int fnr, char * msg, size_t bytes );

        int rtosf_resize( int fnr, size_t size );

        int rtosf_put( int fnr, const char * text, size_t bytes ) ;

        int rtosf_set_handler( int fnr, int ( *h ) ( unsigned int, int ) );

        void rtosf_remove_all_handlers( int fnr );

#ifdef __cplusplus
    }

#endif

// high-resolution time to timespec
// hrt is in ticks
inline TIME_SPEC ticks2timespec(TICK_TIME hrt)
{
	TIME_SPEC timevl;
	timevl.tv_sec = hrt / 1000000000LL;
	timevl.tv_nsec = hrt % 1000000000LL;
	return timevl;
}

#ifdef __cplusplus
}

#endif
#endif
