/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  fosi_internal_interface.h 

                        fosi_internal_interface.h -  description
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
 * @file fosi_internal_interface.h
 * This file has all the (periodic) thread specific interfaces.
 * If a function does not apply to your OS, it may be left empty.
 *
 */
 
#ifndef OS_FOSI_INTERNAL_INTERFACE_HPP
#define OS_FOSI_INTERNAL_INTERFACE_HPP

#error "This file is a template for developers, do not include directly."

#include "ThreadInterface.hpp"
#include "fosi.h"

namespace RTT
{ namespace OS {
	namespace detail {

		int rtos_task_create_main(RTOS_TASK* main_task);

		int rtos_task_delete_main(RTOS_TASK* main_task);

		// Return zero on succes
		int rtos_task_create(RTOS_TASK* task, 
							 int priority,
							 const char * name,
							 void * (*start_routine)(void *), 
							 ThreadInterface* obj);

		void rtos_task_yield(RTOS_TASK*);

		// scheduler type/lanes
		int rtos_task_set_scheduler(RTOS_TASK* t, int sched_type);
		int rtos_task_get_scheduler(RTOS_TASK* t);

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
}}
#endif
