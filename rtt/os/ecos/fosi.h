/***************************************************************************
  tag:

                        fosi.hpp -  description
                           -------------------
    begin                : Jan 21 2006
    copyright            : (C) 2006 Klaas Gadeyne
    email                : firstname lastname at fmtc be

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

#ifndef RTT_ECOS__FOSI_H
#define RTT_ECOS__FOSI_H

#define HAVE_FOSI_API

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <cyg/kernel/kapi.h>
  // #include <errno.h>
#include "os_ecos.h"
#include <pkgconf/kernel.h>
#include <cyg/infra/diag.h> // for diag_printf

  // Own implementation of recursive mutexes
#include "ecos_rec_mutex.h"

#define SCHED_ECOS_FIFO 0 /** eCos Scheduler */
#define ORO_SCHED_RT    0
#define ORO_SCHED_OTHER 0

#define ORO_WAIT_ABS 0
#define ORO_WAIT_REL 1

    typedef long long NANO_TIME;
    typedef cyg_tick_count_t TICK_TIME;

    const TICK_TIME InfiniteTicks = ULONG_LONG_MAX;
    const NANO_TIME InfiniteNSecs = LONG_LONG_MAX;
    const double    InfiniteSeconds = DBL_MAX;

  typedef struct {
    // the thread
    cyg_thread thread;
    // its name
    char * name;

    // And its handle
    cyg_handle_t handle;

    // Stack pointer
    char * stack;

    /* bool to fake soft or hard RT behaviour (ecos does not
       differentiate between hard and soft realtime)
    */
    bool hrt;

    // STUFF for periodic threads (ecos has no native API for creating
    // periodic threads)
    // Next firetime
    NANO_TIME periodMark;
    // the period
    NANO_TIME period;
    cyg_handle_t counter_hdl;
    cyg_handle_t sys_clk_hdl;
    cyg_handle_t alarm_hdl;
    cyg_alarm alarm_obj;
    cyg_sem_t wakeup_sem;
  } RTOS_TASK;


  // Time Related
#include <time.h>
#include <unistd.h>

  typedef struct timespec TIME_SPEC;

  inline
  TICK_TIME nano2ticks( NANO_TIME nano )
  {
    // FIXME need more efficient calculation...
    return (CYGNUM_HAL_RTC_DENOMINATOR*nano)/CYGNUM_HAL_RTC_NUMERATOR;
  }

  inline
  NANO_TIME ticks2nano( TICK_TIME count )
  {
    // FIXME need more efficient calculation...
    return CYGNUM_HAL_RTC_NUMERATOR/CYGNUM_HAL_RTC_DENOMINATOR*count;
  }

  inline NANO_TIME rtos_get_time_ns( void )
  {
    return ticks2nano(cyg_current_time());
  }

  inline TICK_TIME rtos_get_time_ticks( void )
  {
    return cyg_current_time();
  }

  /*
    inline int rtos_nanosleep( const TIME_SPEC * rqtp, TIME_SPEC * rmtp )
    {
    //    return usleep(rqtp->tv_nsec/1000L);
    return nanosleep( rqtp, rmtp );
    }
  */

  typedef cyg_sem_t rt_sem_t;

  static inline int rtos_sem_init(rt_sem_t* m, int value )
  {
    cyg_semaphore_init(m, value);
    return 0;
  }

  static inline int rtos_sem_destroy(rt_sem_t* m )
  {
    cyg_semaphore_destroy(m);
    return 0;
  }

  static inline int rtos_sem_signal(rt_sem_t* m )
  {
    cyg_semaphore_post(m);
    return 0;
  }

  static inline int rtos_sem_wait(rt_sem_t* m )
  {
    cyg_semaphore_wait(m);
    return 0;
  }

  // Should return 0 if no timeout occurs
  static inline int rtos_sem_trywait(rt_sem_t* m )
  {
    if (cyg_semaphore_trywait(m) == true)
      return 0;
    else
      return -1;
  }

  // Should return 0 if no timeout occurs
  static inline int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay )
  {
    // cyg_semaphore_timed_wait returns true if no timeout occurs
    if (cyg_semaphore_timed_wait(m,cyg_current_time()+nano2ticks(delay)) == true)
      return 0;
    else
      return -1;
  }

  static inline int rtos_sem_value(rt_sem_t* m )
  {
    int val = 0;
    cyg_semaphore_peek(m, &val);
    return val;
  }

  // Mutex functions

  typedef cyg_mutex_t rt_mutex_t;
  typedef cyg_recursive_mutex_t rt_rec_mutex_t;

  //
  static inline int rtos_mutex_init(rt_mutex_t* m)
  {
    cyg_mutex_init(m);
    return 0;
  }

  static inline int rtos_mutex_destroy(rt_mutex_t* m )
  {
    cyg_mutex_release(m);
    cyg_mutex_destroy(m);
    return 0;
  }

  static inline int rtos_mutex_rec_init(rt_rec_mutex_t* m)
  {
    cyg_recursive_mutex_init(m);
    return 0;
  }

  static inline int rtos_mutex_rec_destroy(rt_rec_mutex_t* m )
  {
    cyg_recursive_mutex_destroy(m);
    return 0;
  }

  static inline int rtos_mutex_lock( rt_mutex_t* m)
  {
    return cyg_mutex_lock(m);
  }

  static inline int rtos_mutex_rec_lock( rt_rec_mutex_t* m)
  {
    return cyg_recursive_mutex_lock(m);
  }

  static inline int rtos_mutex_trylock( rt_mutex_t* m)
  {
    if (cyg_mutex_trylock(m) == true)
      return 0;
    else
      return -1;
  }

  static inline int rtos_mutex_rec_trylock( rt_rec_mutex_t* m)
  {
    if (cyg_recursive_mutex_trylock(m) == true)
      return 0;
    else
      return -1;
  }

  static inline int rtos_mutex_unlock( rt_mutex_t* m)
  {
    cyg_mutex_unlock(m);
    return 0;
  }

  static inline int rtos_mutex_rec_unlock( rt_rec_mutex_t* m)
  {
    cyg_recursive_mutex_unlock(m);
    return 0;
  }

    static inline void rtos_enable_rt_warning()
    {
    }

    static inline void rtos_disable_rt_warning()
    {
    }

#define rtos_printf diag_printf

#ifdef __cplusplus
}

#endif
#endif
