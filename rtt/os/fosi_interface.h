/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  fosi_interface.h

                        fosi_interface.h -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
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
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


/**
 * @file fosi_interface.h
 * This file defines the FOSI (Framework Operating System Interface) from
 * orocos calls to native RTOS calls
 *
 * Rename to 'fosi.h'
 */

#ifndef ORO_FOSI_INTERFACE_H
#define ORO_FOSI_INTERFACE_H

#error "This file is a template for developers, do not include directly."

#ifdef __cplusplus
extern "C"
{
#endif

  struct MyTask;
  typedef struct MyTask RTOS_TASK;

  // include 'atomic' functions,
  // depends on processor target, not OS.
#include "oro_arch.h"

#define ORO_SCHED_RT    0 /** Hard real-time scheduler type. @see RTT::OS::ThreadInterface::setScheduler */
#define ORO_SCHED_OTHER 1 /** Soft real-time scheduler type. @see RTT::OS::ThreadInterface::setScheduler */

  /**
   * OS representation of nano seconds.
   */
  typedef long long NANO_TIME;
  /**
   * OS representation of system ticks.
   */
  typedef long long TICK_TIME;

  /**
   * Get "system" time in nanoseconds
   */
  NANO_TIME rtos_get_time_ns( void );

  /**
   * Get "system" time in ticks
   * FIXME see <https://proj.fmtc.be/orocos-bugzilla/show_bug.cgi?id=60>
   */
  TICK_TIME rtos_get_time_ticks (void );

  /**
   * Time conversions from nano seconds to system ticks.
   */
  TICK_TIME nano2ticks( NANO_TIME nano );
  /**
   * Time conversions from system ticks to nano seconds.
   */
  NANO_TIME ticks2nano( TICK_TIME count );

  // Semaphore functions
  typedef struct sem_struct rt_sem_t;
  /** All these should return zero in case of succes.  Their meaning
   *   is hopefully obvious
   */
  int rtos_sem_init(rt_sem_t* m, int value );
  int rtos_sem_destroy(rt_sem_t* m );
  int rtos_sem_signal(rt_sem_t* m );
  int rtos_sem_wait(rt_sem_t* m );
  int rtos_sem_trywait(rt_sem_t* m );
  int rtos_sem_value(rt_sem_t* m );

  // Mutex and recursive mutex functions
  typedef struct mutex_struct rt_mutex_t;
  typedef struct recursive_mutex_struct rt_rec_mutex_t;

  int rtos_mutex_init(rt_mutex_t* m);
  int rtos_mutex_destroy(rt_mutex_t* m );
  int rtos_mutex_rec_init(rt_rec_mutex_t* m);
  int rtos_mutex_rec_destroy(rt_rec_mutex_t* m );
  int rtos_mutex_lock( rt_mutex_t* m);
  int rtos_mutex_trylock( rt_mutex_t* m);
  int rtos_mutex_trylock_for( rt_mutex_t* m, NANO_TIME relative_time);
  int rtos_mutex_lock_until( rt_mutex_t* m, NANO_TIME abs_time);
  int rtos_mutex_unlock( rt_mutex_t* m);
  int rtos_mutex_rec_lock( rt_rec_mutex_t* m);
  int rtos_mutex_rec_trylock( rt_rec_mutex_t* m);
  int rtos_mutex_rec_trylock_for( rt_rec_mutex_t* m, NANO_TIME relative_time);
  int rtos_mutex_rec_lock_until( rt_rec_mutex_t* m, NANO_TIME abs_time);
  int rtos_mutex_rec_unlock( rt_rec_mutex_t* m);

  // Condition variables must support waiting, timed waiting and broadcasting.
  typedef struct cond_struct rt_cond_t;
  int rtos_cond_init(rt_cond_t *cond);
  int rtos_cond_destroy(rt_cond_t *cond);
  int rtos_cond_wait(rt_cond_t *cond, rt_mutex_t *mutex);
  int rtos_cond_timedwait(rt_cond_t *cond, rt_mutex_t *mutex, NANO_TIME abs_time);
  int rtos_cond_broadcast(rt_cond_t *cond);

	/**
	 * 'real-time' print function.
	 */
	int rtos_printf( const char *fmt, ... );

    /** Allows the RTOS to print a warning when we violate real-time constraints. */
  void rtos_enable_rt_warning();
    /** Disallows the RTOS to print a warning when we violate real-time constraints. */
  void rtos_disable_rt_warning();

#ifdef __cplusplus
}
#endif

#endif // ORO_FOSI_INTERFACE_H
