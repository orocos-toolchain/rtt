/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  fosi_interface.h 

                        fosi_interface.h -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
#include "oro_atomic.h"
#include "oro_bitops.h"

  /**
   * Time functions.
   */
  typedef long long NANO_TIME;
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
   * Time conversions.
   */
  TICK_TIME nano2ticks( NANO_TIME nano );
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
  int rtos_sem_wait_timed(rt_sem_t* m, NANO_TIME delay );
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
  int rtos_mutex_unlock( rt_mutex_t* m);
  int rtos_mutex_rec_lock( rt_rec_mutex_t* m);
  int rtos_mutex_rec_trylock( rt_rec_mutex_t* m);
  int rtos_mutex_rec_unlock( rt_rec_mutex_t* m);

  // 'real-time' print functions

  int rtos_printf( const char *fmt, ... );

#ifdef __cplusplus
}
#endif

#endif // ORO_FOSI_INTERFACE_H
