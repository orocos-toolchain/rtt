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
 
 
#define OROBLD_OS_LXRT_INTERNAL
#include "os/fosi.h"

#ifdef OROBLD_OS_AGNOSTIC
// Encapsulate all RTAI/LXRT specific functions

// hrt is in nanoseconds
TIME_SPEC ticks2timespec(RTIME hrt)
{
	TIME_SPEC timevl;
	timevl.tv_sec = nano2count(hrt) / 1000000000LL;
	timevl.tv_nsec = nano2count(hrt) % 1000000000LL;
	return timevl;
}

// turn this on to have maximum detection of valid system calls.
#ifdef OROSEM_OS_LXRT_CHECK
#define CHK_LXRT_CALL() do { if(rt_buddy() == 0) { \
        printf("LXRT NOT INITIALISED IN THIS THREAD pid=%d,\n\
    BUT TRIES TO INVOKE LXRT FUNCTION >>%s<< ANYWAY\n", getpid(), __FUNCTION__ );\
        assert( rt_buddy() != 0 ); }\
        } while(0)
#define CHK_LXRT_PTR(ptr) do { if(ptr == 0) { \
        printf("TRIED TO PASS NULL POINTER TO LXRT IN THREAD pid=%d,\n\
    IN TRYING TO INVOKE LXRT FUNCTION >>%s<<\n", getpid(), __FUNCTION__ );\
        assert( ptr != 0 ); }\
        } while(0)
#else
#define CHK_LXRT_CALL()
#define CHK_LXRT_PTR( a )
#endif

    
NANO_TIME rtos_get_time_ns(void) { return rt_get_time_ns(); }

TICK_TIME rtos_get_time_ticks(void) { return rt_get_time(); }

TICK_TIME ticksPerSec(void) { return nano2count( 1000 * 1000 * 1000 ); }

TICK_TIME nano2ticks(NANO_TIME t) { return nano2count(t); }

NANO_TIME ticks2nano(TICK_TIME t) { return count2nano(t); }


int rtos_nanosleep(const TIME_SPEC *rqtp, TIME_SPEC *rmtp) 
{
    CHK_LXRT_CALL();
    nanosleep(rqtp,rmtp); // rtai 24.1.9
    return 0;
}

    int rtos_sem_init(rt_sem_t* m, int value )
    {
        CHK_LXRT_CALL();
		// store the pointer in m->opaque...
        m->sem = rt_sem_init( rt_get_name(0) , value);
        return m->sem == 0 ? -1 : 0;
    }

    int rtos_sem_destroy(rt_sem_t* m )
    {
        CHK_LXRT_CALL();
        return rt_sem_delete(m->sem);
    }

    int rtos_sem_signal(rt_sem_t* m )
    {
        CHK_LXRT_CALL();
        return rt_sem_signal(m->sem);
    }

    int rtos_sem_wait(rt_sem_t* m )
    {
        CHK_LXRT_CALL();
        return rt_sem_wait(m->sem);
    }

    int rtos_sem_trywait(rt_sem_t* m )
    {
        CHK_LXRT_CALL();
        return rt_sem_wait_if(m->sem);
    }

    int rtos_sem_value(rt_sem_t* m )
    {
        CHK_LXRT_CALL();
        return rt_sem_count(m->sem);
    }

    int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay )
    {
      int ret;
        CHK_LXRT_CALL();
        ret = rt_sem_wait_timed(m->sem, nano2count(delay) );
#if defined(CONFIG_RTAI_VERSION_MINOR) && defined(CONFIG_RTAI_VERSION_MAJOR)
#  if CONFIG_RTAI_VERSION_MAJOR == 3 && CONFIG_RTAI_VERSION_MINOR > 3
	return (ret == RTE_TIMOUT) ? -1 : 0;
#  else
	return (ret == SEM_TIMOUT) ? -1 : 0;
#  endif
#else
	return (ret == SEM_TIMOUT) ? -1 : 0;
#endif
    }

    int rtos_sem_wait_until(rt_sem_t* m, NANO_TIME when )
    {
        int ret;
        CHK_LXRT_CALL();
        ret = rt_sem_wait_until(m->sem, nano2count(when) ) ;
#if defined(CONFIG_RTAI_VERSION_MINOR) && defined(CONFIG_RTAI_VERSION_MAJOR)
#  if CONFIG_RTAI_VERSION_MAJOR == 3 && CONFIG_RTAI_VERSION_MINOR > 3
        return (ret == RTE_TIMOUT) ? -1 : 0;
#  else
        return (ret == SEM_TIMOUT) ? -1 : 0;
#  endif
#else
        return (ret == SEM_TIMOUT) ? -1 : 0;
#endif
    }

    int rtos_mutex_init(rt_mutex_t* m)
    {
        CHK_LXRT_CALL();
        m->sem = rt_typed_sem_init( rt_get_name(0),1, BIN_SEM | PRIO_Q);
        return m->sem == 0 ? -1 : 0;
    }

    int rtos_mutex_destroy(rt_mutex_t* m )
    {
        CHK_LXRT_CALL();
        CHK_LXRT_PTR(m);
        return rt_sem_delete(m->sem);
    }

    int rtos_mutex_rec_init(rt_mutex_t* m)
    {
        CHK_LXRT_CALL();
        // RES_SEM is PRIO_Q anyhow.
        m->sem = rt_typed_sem_init( rt_get_name(0), 1, RES_SEM);
        return m->sem == 0 ? -1 : 0;
    }

    int rtos_mutex_rec_destroy(rt_mutex_t* m )
    {
        CHK_LXRT_CALL();
        CHK_LXRT_PTR(m->sem);
        return rt_sem_delete(m->sem);
    }

    int rtos_mutex_rec_lock( rt_rec_mutex_t* m)
    {
        CHK_LXRT_CALL();
        return rt_sem_wait(m->sem);
    }

    int rtos_mutex_rec_trylock( rt_rec_mutex_t* m)
    {
        CHK_LXRT_CALL();
        return rt_sem_wait_if(m->sem) > 0 ? 0 : -EAGAIN;
    }

    int rtos_mutex_lock_until( rt_mutex_t* m, NANO_TIME abs_time)
    {
        CHK_LXRT_CALL();
        return rt_sem_wait_until(m->sem, nano2count(abs_time)) < SEM_TIMOUT ? 0 : -EAGAIN;
    }

    int rtos_mutex_rec_lock_until( rt_rec_mutex_t* m, NANO_TIME abs_time)
    {
        CHK_LXRT_CALL();
        return rt_sem_wait_until(m->sem, nano2count(abs_time)) < SEM_TIMOUT ? 0 : -EAGAIN;
    }

    int rtos_mutex_rec_unlock( rt_rec_mutex_t* m)
    {
        CHK_LXRT_CALL();
        return rt_sem_signal(m->sem);
    }

    int rtos_mutex_lock( rt_mutex_t* m)
    {
        CHK_LXRT_CALL();
        CHK_LXRT_PTR(m->sem);
        return rt_sem_wait(m->sem);
    }

    int rtos_mutex_trylock( rt_mutex_t* m)
    {
        CHK_LXRT_CALL();
        CHK_LXRT_PTR(m->sem);
        return rt_sem_wait_if(m->sem) > 0 ? 0 : -EAGAIN;
    }

    int rtos_mutex_unlock( rt_mutex_t* m)
    {
        CHK_LXRT_CALL();
        CHK_LXRT_PTR(m->sem);
        return rt_sem_signal(m->sem);
    }

    int rtos_cond_init(rt_cond_t *cond)
    {
        CHK_LXRT_CALL();
        cond->cond = rt_cond_init(0);
        return cond->cond == 0 ? -1 : 0;
    }

    int rtos_cond_destroy(rt_cond_t *cond)
    {
        CHK_LXRT_CALL();
        return rt_cond_delete(cond->cond);
    }

    int rtos_cond_wait(rt_cond_t *cond, rt_mutex_t *mutex)
    {
        CHK_LXRT_CALL();
        int ret = rt_cond_wait(cond->cond, mutex->sem );
        if (ret == 0)
            return 0;
        return -1;
    }

    int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, NANO_TIME abstime)
    {
        CHK_LXRT_CALL();
        int ret = rt_cond_wait_until(cond->cond, mutex->sem, nano2count(abs_time) );
        if (ret == 0)
            return 0;
        if ( ret == SEM_TIMOUT )
            return ETIMEOUT;
        return -1;
    }

    int rtos_cond_broadcast(rt_cond_t *cond)
    {
        CHK_LXRT_CALL();
        return rt_cond_broadcast(cond->cond);
    }

int rtos_printf(const char *fmt, ...)
{
    va_list list;
    char printkbuf [2000];
    printkbuf[0] = '\0';
    va_start (list, fmt);
    vsprintf(printkbuf, fmt, list);
    va_end (list);
    // XXX revert to print to screen when debugging is over
    return rtai_print_to_screen(printkbuf);
    //return printf(printkbuf);
}

#endif // OROBLD_OS_AGNOSTIC

