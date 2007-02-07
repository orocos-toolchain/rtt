/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:15 CET 2005  fosi.c 

                        fosi.c -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
#define OROBLD_OS_INTERNAL
#include "os/fosi.h"
#include <assert.h>

#ifdef OROBLD_OS_AGNOSTIC

	// hrt is in ticks
TIME_SPEC ticks2timespec(TICK_TIME hrt)
{
	TIME_SPEC timevl;
	timevl.tv_sec = rt_timer_ticks2ns(hrt) / 1000000000LL;
	timevl.tv_nsec = rt_timer_ticks2ns(hrt) % 1000000000LL;
	return timevl;
}

	// hrt is in ticks
TICK_TIME timespec2ticks(const TIME_SPEC* ts)
{
	return  rt_timer_ns2ticks(ts->tv_nsec + ts->tv_sec*1000000000LL);
}

// turn this on to have maximum detection of valid system calls.
#ifdef OROSEM_OS_XENO_CHECK
#define CHK_XENO_CALL() do { if(rt_task_self() == 0) { \
        printf("XENO NOT INITIALISED IN THIS THREAD pid=%d,\n\
    BUT TRIES TO INVOKE XENO FUNCTION >>%s<< ANYWAY\n", getpid(), __FUNCTION__ );\
        assert( rt_task_self() != 0 ); }\
        } while(0)
#define CHK_XENO_PTR(ptr) do { if(ptr == 0) { \
        printf("TRIED TO PASS NULL POINTER TO XENO IN THREAD pid=%d,\n\
    IN TRYING TO INVOKE XENO FUNCTION >>%s<<\n", getpid(), __FUNCTION__ );\
        assert( ptr != 0 ); }\
        } while(0)
#else
#define CHK_XENO_CALL()
#define CHK_XENO_PTR( a )
#endif
    
// WARNING: Orocos 'ticks' are 'Xenomai tsc' and Xenomai `ticks' are not
// used in the Orocos API. Thus Orocos uses `Xenomai tsc' and `Xenomai ns', 
// yet Xenomai requires `Xenomai ticks' at the interface
// ==> do not use nano2ticks to convert to `Xenomai ticks' because it
// converts to `Xenomai tsc'.

NANO_TIME rtos_get_time_ns(void) { return rt_timer_ticks2ns(rt_timer_read()); }

TICK_TIME rtos_get_time_ticks(void) { return rt_timer_tsc(); }

TICK_TIME ticksPerSec(void) { return rt_timer_ns2tsc( 1000 * 1000 * 1000 ); }

	TICK_TIME nano2ticks(NANO_TIME t) { return rt_timer_ns2tsc(t); }
	NANO_TIME ticks2nano(TICK_TIME t) { return rt_timer_tsc2ns(t); }

	int rtos_nanosleep(const TIME_SPEC *rqtp, TIME_SPEC *rmtp)
	{
		CHK_XENO_CALL();
		RTIME ticks = rqtp->tv_sec * 1000000000LL + rqtp->tv_nsec;
		rt_task_sleep( rt_timer_ns2ticks(ticks) );
		return 0;
	}

     int rtos_sem_init(rt_sem_t* m, int value )
    {
        CHK_XENO_CALL();
		// priority based blocking:
		return rt_sem_create( m, 0, value, S_PRIO);
    }

     int rtos_sem_destroy(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        return rt_sem_delete( m );
    }

     int rtos_sem_signal(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        return rt_sem_v( m );
    }

     int rtos_sem_wait(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        return rt_sem_p( m, TM_INFINITE );
    }

     int rtos_sem_trywait(rt_sem_t* m )
    {
        CHK_XENO_CALL();
		// returns 0 if sem was grabbed, -EWOULDBLOCK otherwise.
        return rt_sem_p( m, TM_NONBLOCK);
    }

     int rtos_sem_value(rt_sem_t* m )
    {
        CHK_XENO_CALL();
		RT_SEM_INFO sinfo;
        if (rt_sem_inquire(m, &sinfo) == 0 ) {
			return sinfo.count;
		}
		return -1;
    }

     int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay )
    {
        CHK_XENO_CALL();
        return rt_sem_p(m, rt_timer_ns2ticks(delay) );
    }

     int rtos_mutex_init(rt_mutex_t* m)
    {
        CHK_XENO_CALL();
		// a Xenomai mutex is always recursive
        return rt_mutex_create(m, 0);
    }

     int rtos_mutex_destroy(rt_mutex_t* m )
    {
        CHK_XENO_CALL();
        return rt_mutex_delete(m);
    }

     int rtos_mutex_rec_init(rt_mutex_t* m)
    {
        CHK_XENO_CALL();
		// a Xenomai mutex is always recursive
        return rt_mutex_create(m, 0);
    }

     int rtos_mutex_rec_destroy(rt_mutex_t* m )
    {
        CHK_XENO_CALL();
        return rt_mutex_delete(m);
    }

     int rtos_mutex_lock( rt_mutex_t* m)
    {
        CHK_XENO_CALL();
        return rt_mutex_lock(m, TM_INFINITE);
    }

     int rtos_mutex_trylock( rt_mutex_t* m)
    {
        CHK_XENO_CALL();
		return rt_mutex_lock(m, TM_NONBLOCK);
    }

     int rtos_mutex_unlock( rt_mutex_t* m)
    {
        CHK_XENO_CALL();
        return rt_mutex_unlock(m);
    }

    int rtos_mutex_rec_lock( rt_rec_mutex_t* m)
    {
        CHK_XENO_CALL();
        return rt_mutex_lock(m, TM_INFINITE );
    }

    int rtos_mutex_rec_trylock( rt_rec_mutex_t* m)
    {
        CHK_XENO_CALL();
        return rt_mutex_lock(m, TM_NONBLOCK);
    }

    int rtos_mutex_rec_unlock( rt_rec_mutex_t* m)
    {
        CHK_XENO_CALL();
        return rt_mutex_unlock(m);
    }

int rtos_printf(const char *fmt, ...)
{
    va_list list;
    char printkbuf [2000];
    printkbuf[0] = '\0';
    va_start (list, fmt);
    vsprintf(printkbuf, fmt, list);
    va_end (list);
    return printf(printkbuf);
}

#endif
