/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:15 CET 2005  fosi.c 

                        fosi.c -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 

#include "os/fosi.h"

#ifdef OROBLD_OS_AGNOSTIC
// Encapsulate all RTAI/LXRT specific functions

// include custom redirect-like include
#if ORONUM_RTAI_VERSION == 3
#include <rtai_config.h>
#else
#include "rtai_config.h"
#endif

#if ORONUM_RTAI_VERSION == 3
#include <rtai_lxrt.h>
#include <rtai_posix.h>
#else
#define KEEP_STATIC_INLINE
#include <rtai_declare.h>
#include <rtai_usp_posix.h>
#include <rtai_lxrt_user.h> 
    /**
     * About KEEP_STATIC_INLINE:
     *  What is below is questionable, sometimes it crashes, sometimes not. I disassembled the C and C++ object code,
     *  they were the same for 3.x compilers. (ps)
     *  crashes on C++ stuff when using 2.95 compiler, thus you can not include rtai_lxrt.h or you get multiple
     * defined symbols. Missing defines from rtai_lxrt.h need to be redefined in other headerfile.
     */
//#include <rtai_fifos_lxrt_user.h> 
#endif

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
#define CHK_LXRT_PTR()
#endif

    
NANO_TIME rtos_get_time_ns(void) { return rt_get_time_ns(); }

TICK_TIME systemTimeGet(void) { return rt_get_time(); }

TICK_TIME ticksPerSec(void) { return nano2count( 1000 * 1000 * 1000 ); }

TICK_TIME nano2ticks(NANO_TIME t) { return nano2count(t); }

NANO_TIME ticks2nano(TICK_TIME t) { return count2nano(t); }


int rtos_nanosleep(const TIME_SPEC *rqtp, TIME_SPEC *rmtp) 
{
    CHK_LXRT_CALL();
    nanosleep(rqtp,rmtp); // rtai 24.1.9
    return 0;
}

    int rtos_mutex_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr)
    {
        CHK_LXRT_CALL();
        return pthread_mutex_init_rt(m, 0);
    }

    int rtos_mutex_destroy(rt_mutex_t* m )
    {
        CHK_LXRT_CALL();
		CHK_LXRT_PTR(m);
        return pthread_mutex_destroy_rt(m);
    }

    int rtos_mutex_rec_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr)
    {
        CHK_LXRT_CALL();
        pthread_mutexattr_t ma_t;
        pthread_mutexattr_init(&ma_t);
		pthread_mutexattr_settype(&ma_t,PTHREAD_MUTEX_RECURSIVE_NP);
        return pthread_mutex_init_rt(m, 0);
    }

    int rtos_mutex_rec_destroy(rt_mutex_t* m )
    {
        CHK_LXRT_CALL();
		CHK_LXRT_PTR(m);
        return pthread_mutex_destroy_rt(m);
    }

    int rtos_mutex_lock( rt_mutex_t* m)
    {
        CHK_LXRT_CALL();
		CHK_LXRT_PTR(m);
        return pthread_mutex_lock_rt(m);
    }

    int rtos_mutex_trylock( rt_mutex_t* m)
    {
        CHK_LXRT_CALL();
		CHK_LXRT_PTR(m);
        return pthread_mutex_trylock_rt(m);
    }

    int rtos_mutex_unlock( rt_mutex_t* m)
    {
        CHK_LXRT_CALL();
		CHK_LXRT_PTR(m);
        return pthread_mutex_unlock_rt(m);
    }

    int rtos_cond_init(rt_cond_t *cond, pthread_condattr_t *cond_attr)
    {
        CHK_LXRT_CALL();
        return pthread_cond_init_rt(cond, cond_attr);
    }

    int rtos_cond_destroy(rt_cond_t *cond)
    {
        CHK_LXRT_CALL();
		CHK_LXRT_PTR(cond);
        return pthread_cond_destroy_rt(cond);
    }

    int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, const struct timespec *abstime)
    {
        CHK_LXRT_CALL();
		CHK_LXRT_PTR(cond);
		CHK_LXRT_PTR(mutex);
		CHK_LXRT_PTR(abstime);
        return pthread_cond_timedwait_rt(cond, mutex, abstime);
    }

    int rtos_cond_broadcast(rt_cond_t *cond)
    {
        CHK_LXRT_CALL();
		CHK_LXRT_PTR(cond);
        return pthread_cond_broadcast_rt(cond);
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

 void rtos_enable_fpu(pthread_attr_t *pa) { return; }

// RT FIFO

 int rtosf_create(int fnr,size_t bytes) 
{
    return -1;
};

 int rtosf_destroy(int fnr)
{
    return -1;
};

 int rtosf_put(int fnr, const char * text, size_t bytes) 
{
    return -1;
};

 int rtosf_get(int fnr, char * text, size_t  bytes)
{
    return -1;
};

 int rtosf_resize(int fnr, size_t bytes) 
{
    return -1;
};

 void rtosf_remove_all_handlers(int fnr)
{
}


 int rtosf_set_handler(int fnr, int (*handler)(unsigned int, int))
{
    //	return rtf_create_handler(fnr,X_FIFO_HANDLER(handler));
    return -1;
}
