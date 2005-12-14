#ifndef __FOSI_H
#define __FOSI_H

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600   // use all Posix features.
#endif

#define HAVE_FOSI_API

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sched.h>
#include <assert.h> 

#ifdef __cplusplus
extern "C" {
#endif

	// Orocos Implementation (CPU specific)
#include "oro_atomic.h"
#include "oro_bitops.h"

#include "pkgconf/os.h"
#include "pkgconf/os_xenomai.h"
#if !defined(OROBLD_OS_AGNOSTIC) || defined(OROBLD_OS_INTERNAL)
	//Xenomai redefinitions.
#else
	//Xenomai headers
#include <native/task.h>
#include <native/mutex.h>
#include <native/sem.h>
#include <native/timer.h>

#endif // OROBLD_OS_AGNOSTIC // for RTAI header files.

	typedef RT_MUTEX rt_mutex_t;
	typedef RT_SEM rt_sem_t;
	
	// Time Related
	
	// 'S' -> Signed long long
	typedef SRTIME NANO_TIME;
	typedef SRTIME TICK_TIME;
	typedef struct timespec TIME_SPEC;

#ifndef OROBLD_OS_AGNOSTIC
	
	// hrt is in ticks
inline TIME_SPEC ticks2timespec(TICK_TIME hrt)
{
	TIME_SPEC timevl;
	timevl.tv_sec = rt_timer_ticks2ns(hrt) / 1000000000LL;
	timevl.tv_nsec = rt_timer_ticks2ns(hrt) % 1000000000LL;
	return timevl;
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
    
inline NANO_TIME rtos_get_time_ns(void) { return rt_timer_read(); }

inline TICK_TIME systemTimeGet(void) { return rt_timer_tsc(); }

inline TICK_TIME systemNSecsTimeGet(void) { return rt_timer_read(); }

inline TICK_TIME ticksPerSec(void) { return rt_timer_ns2ticks( 1000 * 1000 * 1000 ); }

	inline TICK_TIME nano2ticks(NANO_TIME t) { return rt_timer_ns2ticks(t); }
	inline NANO_TIME ticks2nano(TICK_TIME t) { return rt_timer_ticks2ns(t); }

	inline int rtos_nanosleep(const TIME_SPEC *rqtp, TIME_SPEC *rmtp)
	{
		CHK_XENO_CALL();
		RTIME ticks = rqtp->tv_sec * 1000000000LL + rqtp->tv_nsec;
		rt_task_sleep( ticks );
		return 0;
	}

    static inline int rtos_sem_init(rt_sem_t* m, int value )
    {
        CHK_XENO_CALL();
		return rt_sem_create( m, 0, value, 0);
    }

    static inline int rtos_sem_destroy(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        return rt_sem_delete( m );
    }

    static inline int rtos_sem_signal(rt_sem_t* m )
    {
        CHK_XENO_CALL();
        return rt_sem_v( m, 0 );
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
        return rt_sem_count(m->sem);
    }

    static inline int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay )
    {
        CHK_XENO_CALL();
        return rt_sem_wait_timed(m->sem, nano2count(delay) );
    }

    static inline int rtos_mutex_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr)
    {
        CHK_XENO_CALL();
        return pthread_mutex_init_rt(m, 0);
    }

    static inline int rtos_mutex_destroy(rt_mutex_t* m )
    {
        CHK_XENO_CALL();
        return pthread_mutex_destroy_rt(m);
    }

    static inline int rtos_mutex_rec_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr)
    {
        CHK_XENO_CALL();
        pthread_mutexattr_t ma_t;
        pthread_mutexattr_init(&ma_t);
		pthread_mutexattr_settype(&ma_t,PTHREAD_MUTEX_RECURSIVE_NP);
        return pthread_mutex_init_rt(m, 0);
    }

    static inline int rtos_mutex_rec_destroy(rt_mutex_t* m )
    {
        CHK_XENO_CALL();
        return pthread_mutex_destroy_rt(m);
    }

    static inline int rtos_mutex_lock( rt_mutex_t* m)
    {
        CHK_XENO_CALL();
        return pthread_mutex_lock_rt(m);
    }

    static inline int rtos_mutex_trylock( rt_mutex_t* m)
    {
        CHK_XENO_CALL();
        return pthread_mutex_trylock_rt(m);
    }

    static inline int rtos_mutex_unlock( rt_mutex_t* m)
    {
        CHK_XENO_CALL();
        return pthread_mutex_unlock_rt(m);
    }


    typedef pthread_cond_t rt_cond_t;
    static inline int rtos_cond_init(rt_cond_t *cond, pthread_condattr_t *cond_attr)
    {
        CHK_XENO_CALL();
        return pthread_cond_init_rt(cond, cond_attr);
    }

    static inline int rtos_cond_destroy(rt_cond_t *cond)
    {
        CHK_XENO_CALL();
        return pthread_cond_destroy_rt(cond);
    }

    static inline int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, const struct timespec *abstime)
    {
        CHK_XENO_CALL();
        return pthread_cond_timedwait_rt(cond, mutex, abstime);
    }

    static inline int rtos_cond_broadcast(rt_cond_t *cond)
    {
        CHK_XENO_CALL();
        return pthread_cond_broadcast_rt(cond);
    }

inline
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

#else  // OSBLD_OS_AGNOSTIC

/**
 * Only function prototypes.
 */

TIME_SPEC ticks2timespec(TICK_TIME hrt);

NANO_TIME rtos_get_time_ns(void);

TICK_TIME systemTimeGet(void);

NANO_TIME systemNSecsTimeGet(void);

TICK_TIME ticksPerSec(void);

TICK_TIME nano2ticks(NANO_TIME t);

NANO_TIME ticks2nano(TICK_TIME t);

int rtos_nanosleep(const TIME_SPEC *rqtp, TIME_SPEC *rmtp) ;

int rtos_mutex_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr);

int rtos_mutex_destroy(rt_mutex_t* m );

int rtos_mutex_rec_init(rt_mutex_t* m, const pthread_mutexattr_t *mutexattr);

int rtos_mutex_rec_destroy(rt_mutex_t* m );

int rtos_mutex_lock( rt_mutex_t* m);

int rtos_mutex_trylock( rt_mutex_t* m);

int rtos_mutex_unlock( rt_mutex_t* m);

typedef pthread_cond_t rt_cond_t;
int rtos_cond_init(rt_cond_t *cond, pthread_condattr_t *cond_attr);

int rtos_cond_destroy(rt_cond_t *cond);

int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, const struct timespec *abstime);

int rtos_cond_broadcast(rt_cond_t *cond);

int rtos_printf(const char *fmt, ...);

int rtos_sem_init(rt_sem_t* m, int value );
int rtos_sem_destroy(rt_sem_t* m );
int rtos_sem_signal(rt_sem_t* m );
int rtos_sem_wait(rt_sem_t* m );
int rtos_sem_trywait(rt_sem_t* m );
int rtos_sem_value(rt_sem_t* m );
int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay );

#endif // OSBLD_OS_AGNOSTIC



// will become redundant ?
void rtos_enable_fpu(pthread_attr_t *pa);

// RT FIFO

int rtosf_create(int fnr,size_t bytes);

int rtosf_destroy(int fnr);

int rtosf_put(int fnr, const char * text, size_t bytes);

int rtosf_get(int fnr, char * text, size_t  bytes);

int rtosf_resize(int fnr, size_t bytes) ;

void rtosf_remove_all_handlers(int fnr);


int rtosf_set_handler(int fnr, int (*handler)(unsigned int, int));


#ifdef __cplusplus
}
#endif

#endif
