/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  fosi_internal_interface.h 

                        fosi_internal_interface.h -  description
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
 * @file fosi_task_interface.h
 * This file has all the (periodic) thread specific interfaces.
 * If a function does not apply to your OS, it may be left empty.
 *
 * Rename to 'fosi_internal.h'
 */
 
#ifndef OS_FOSI_INTERNAL_INTERFACE_HPP
#define OS_FOSI_INTERNAL_INTERFACE_HPP

#error "This file is a template for developers, do not include directly."

#include "ThreadInterface.hpp"
#include "fosi.h"

namespace OS
{
  namespace detail {
    // Return zero on succes
    int rtos_task_create(RTOS_TASK* task, 
			 int priority,
			 const char * name,
			 void * (*start_routine)(void *), 
			 ThreadInterface* obj);

    void rtos_task_yield(RTOS_TASK*);

    // these should disappear in the future...
    void rtos_task_make_hard_real_time(RTOS_TASK*);
    void rtos_task_make_soft_real_time(RTOS_TASK*);
    int rtos_task_is_hard_real_time(RTOS_TASK*);

    void rtos_task_make_periodic(RTOS_TASK* mytask, NANO_TIME nanosecs );
    void rtos_task_set_period( RTOS_TASK* mytask, NANO_TIME nanosecs );
    NANOTIME rtos_task_get_period( RTOS_TASK* mytask );
    
    // return zero if no "overruns" did occur
    int rtos_task_wait_period( RTOS_TASK* task );
    
    void rtos_task_delete(RTOS_TASK* mytask);

    int rtos_task_set_priority(RTOS_TASK * task,int priority);
    int rtos_task_get_priority(RTOS_TASK * task);

    const char * rtos_task_get_name(const RTOS_TASK* t);
  }
}
#endif
