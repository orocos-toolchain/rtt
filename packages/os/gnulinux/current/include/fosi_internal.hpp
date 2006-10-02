/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:51 CEST 2005  fosi_internal.hpp 

                        fosi_internal.hpp -  description
                           -------------------
    begin                : Sat May 21 2005
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
 
 
#ifndef OS_FOSI_INTERNAL_HPP
#define OS_FOSI_INTERNAL_HPP

#include "ThreadInterface.hpp"
#include "fosi.h"
#include <iostream>
#include <string.h>
#include "../Logger.hpp"

#define INTERNAL_QUAL static inline

namespace RTT
{ namespace OS {
    namespace detail {

	INTERNAL_QUAL int rtos_task_create_main(RTOS_TASK* main_task)
	{
	    main_task->name = "main";
	    pthread_attr_init( &(main_task->attr) );
	    struct sched_param sp;
	    sp.sched_priority=0;
	    // Set priority
	    // fixme check return value and bail out if necessary
	    pthread_attr_setschedparam(&(main_task->attr), &sp);
	    return 0;
	}

	INTERNAL_QUAL int rtos_task_delete_main(RTOS_TASK* main_task)
	{
	    return 0;
	}

	INTERNAL_QUAL int rtos_task_create(RTOS_TASK* task,
					   int priority,
					   const char * name,
					   int sched_type,
					   void * (*start_routine)(void *), 
					   ThreadInterface* obj) 
	{
	    int rv; // return value
      
	    // Set name
	    if ( strlen(name) == 0 )
		name = "Thread";
	    task->name = strcpy( (char*)malloc( (strlen(name) + 1) * sizeof(char)), name);
      
	    if ( (rv = pthread_attr_init(&(task->attr))) != 0 ){
		return rv;
	    }
	    // Set scheduler type (_before_ assigning priorities!)
	    if ( (rv = pthread_attr_setschedpolicy(&(task->attr), sched_type)) != 0){
		return rv;
	    }
	    /* SCHED_OTHER tasks are always assigned static priority 0, see
	       man sched_setscheduler
	    */
	    if (sched_type != SCHED_OTHER){
		struct sched_param sp;
		sp.sched_priority=priority;
		// Set priority
		if ( (rv = pthread_attr_setschedparam(&(task->attr), &sp)) != 0 ){
		    return rv;
		}
	    }
	    else {
            Logger::In in( task->name );
            log(Warning) << "Forcing priority of thread with SCHED_OTHER policy to 0." <<endlog();
	    }
	    return pthread_create(&(task->thread), &(task->attr), 
				  start_routine, obj);
	}

	INTERNAL_QUAL void rtos_task_yield(RTOS_TASK*) {
	}

	INTERNAL_QUAL void rtos_task_make_hard_real_time(RTOS_TASK*) {
	}
    
	INTERNAL_QUAL void rtos_task_make_soft_real_time(RTOS_TASK*) {
	}

	INTERNAL_QUAL int rtos_task_is_hard_real_time(const RTOS_TASK*) {
	    return 0;
	}

	INTERNAL_QUAL void rtos_task_make_periodic(RTOS_TASK* mytask, NANO_TIME nanosecs )
	{
	    // set period
	    mytask->period = nanosecs;
	    // set next wake-up time.
	    mytask->periodMark = rtos_get_time_ns() + nanosecs;
	}
        
	INTERNAL_QUAL void rtos_task_set_period( RTOS_TASK* mytask, NANO_TIME nanosecs )
	{
	    mytask->period = nanosecs;
	    mytask->periodMark = rtos_get_time_ns() + nanosecs;
	}

	INTERNAL_QUAL NANO_TIME rtos_task_get_period(const RTOS_TASK* t) {
	    return t->period;
	} 

	INTERNAL_QUAL int rtos_task_wait_period( RTOS_TASK* task )
	{
	    if ( task->period == 0 )
		return 0;
      
	    //rtos_printf("Time is %lld nsec, Mark is %lld nsec.\n",rtos_get_time_ns(), task->periodMark );
	    // CALCULATE in nsecs
	    NANO_TIME timeRemaining = task->periodMark - rtos_get_time_ns();

	    if ( timeRemaining > 0 ) {
		//rtos_printf("Waiting for %lld nsec\n",timeRemaining);
		TIME_SPEC ts( ticks2timespec( timeRemaining ) );
		rtos_nanosleep( &ts , NULL );
	    }
	    //             else
	    //                 rtos_printf( "GNULinux task did not get deadline !\n" );

	    // next wake-up time :
	    task->periodMark += task->period;

	    return 0;
	}

	INTERNAL_QUAL void rtos_task_delete(RTOS_TASK* mytask) {
	    free(mytask->name);
	};

	INTERNAL_QUAL int rtos_task_set_priority(RTOS_TASK * task, int priority)
	{
	    // FIXME, only works on the current task
	    // init the scheduler. The rt_task_initschmod code is broken, so we do it ourselves.
	    struct sched_param mysched;
	    mysched.sched_priority = sched_get_priority_max(OROSEM_OS_SCHEDTYPE) - priority;
	    // check lower bounds :
	    if (OROSEM_OS_SCHEDTYPE == SCHED_OTHER && mysched.sched_priority != 0 ) {
		mysched.sched_priority = 0; // SCHED_OTHER must be zero
	    } else if (OROSEM_OS_SCHEDTYPE == !SCHED_OTHER &&  mysched.sched_priority < 1 ) {
		mysched.sched_priority = 1; // !SCHED_OTHER must be 1 or higher
	    }
	    // check upper bound
	    if ( mysched.sched_priority > 99)
		mysched.sched_priority = 99;
	    // set scheduler
	    return sched_setscheduler(0, OROSEM_OS_SCHEDTYPE, &mysched);
	}

	INTERNAL_QUAL int rtos_task_get_priority(const RTOS_TASK *t)
	{
	    struct sched_param sp;
	    int succeeded;
	    succeeded = pthread_attr_getschedparam(&(t->attr), 
						   &sp);
	    return sp.sched_priority;
	}

	INTERNAL_QUAL const char * rtos_task_get_name(const RTOS_TASK* t)
	{
	    return t->name;
	}

    }
}}
#undef INTERNAL_QUAL
#endif
