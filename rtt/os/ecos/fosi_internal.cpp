/***************************************************************************
  tag:

                        fosi_internal.hpp -  description
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


#include <pkgconf/kernel.h>
#include <pkgconf/os_ecos.h>
#include "ThreadInterface.hpp"
#include "fosi.h"
#include "../fosi_internal_interface.hpp"
#include <cyg/kernel/kapi.h>
#include <iostream>
#include <string>

#define INTERNAL_QUAL

namespace RTT
{
  namespace os {
	INTERNAL_QUAL int rtos_task_create_main(RTOS_TASK* main_task)
	{
        const char* name = "main";
	    main_task->name = strcpy( (char*)malloc( (strlen(name) + 1) * sizeof(char)), name);
	    return 0;
	}

	INTERNAL_QUAL int rtos_task_delete_main(RTOS_TASK* main_task)
	{
        free(main_task->name);
        main_task->name = 0;
	    return 0;
	}


    INTERNAL_QUAL int rtos_task_create(RTOS_TASK * task,
				       int priority,
				       unsigned cpu_affinity,
				       const char * name,
				       int sched_type,
				       size_t stack_size,
				       void * (*start_routine)(void *),
				       ThreadInterface* obj) {
      /* sched_type is unused in eCos */
      // Allocate room for threads name
      if ( strlen(name) == 0 )
          name = "Thread";
      task->name = strcpy( (char*)malloc( (strlen(name) + 1) * sizeof(char)), name);

      // Allocate necessary stack...
      task->stack = (char *)malloc(stack_size?stack_size:OROSEM_OS_ECOS_STACK_SIZE);

      // Create the thread
      cyg_thread_create((cyg_addrword_t) priority, // priority
			(cyg_thread_entry_t *) start_routine, // Entry point
			(cyg_addrword_t) obj, // Entry point data
			task->name, // Name
			task->stack, // the stack
			OROSEM_OS_ECOS_STACK_SIZE, // stacksize
			&(task->handle),
			&(task->thread));
      // Start in fake Soft Real-Time mode
      task->hrt = false;
      // initialize the rest to zero
      task->periodMark = 0;
      task->period = 0;
      task->counter_hdl = 0;
      task->alarm_hdl = 0;
      // Resume (start) thread
      cyg_thread_resume(task->handle);
      return 0;
    }

    INTERNAL_QUAL void rtos_task_yield(RTOS_TASK*) {
      cyg_thread_yield();
    }

    INTERNAL_QUAL void wakeup_handler(cyg_handle_t alarm_handle,cyg_addrword_t data)
    {
      cyg_sem_t * wakeup_sem = (cyg_sem_t *) data;
      cyg_semaphore_post(wakeup_sem);
    }

    INTERNAL_QUAL void rtos_task_make_periodic(RTOS_TASK* mytask, NANO_TIME nanosecs )
    {
      if (nanosecs != 0)
	{ // Init stuff (fixme, better check if only called once?)
	  // diag_printf("fosi_internal.hpp rtos_task_make_periodic() ticks = %d...\n",nano2ticks(nanosecs));
	  // Get handle to system clock
	  mytask->sys_clk_hdl = cyg_real_time_clock();
	  // Convert clock to counter handle
	  cyg_clock_to_counter(mytask->sys_clk_hdl,&(mytask->counter_hdl));
	  // Initialise semaphore
	  cyg_semaphore_init(&(mytask->wakeup_sem),0);
	  cyg_alarm_create(mytask->counter_hdl,
			   wakeup_handler,
			   (cyg_addrword_t) (&(mytask->wakeup_sem)),
			   &(mytask->alarm_hdl),
			   &(mytask->alarm_obj));
	  // set next wake-up time.
	  cyg_alarm_initialize(mytask->alarm_hdl,
			       rtos_get_time_ticks() + nano2ticks(nanosecs),
			       nano2ticks(nanosecs));
	} else /* period is zero */ {
        // Check if alarm already created...
        if (mytask->alarm_hdl != 0) {
            // diag_printf("fosi_internal.hpp rtos_task_set_period() WARNING: Disabling alarm...\n");
            cyg_alarm_disable(mytask->alarm_hdl);
        }

      // set period
      mytask->period = nanosecs;
    }

    INTERNAL_QUAL void rtos_task_set_period( RTOS_TASK* mytask, NANO_TIME nanosecs )
    {
      mytask->period = nanosecs;
      // FIXME:: Can one reinitialize an alarm??
      // diag_printf("fosi_internal.hpp WARNING: Can one reinitialize an alarm??\n");
      if (nanosecs != 0){
	// Check if alarm already created...
        if (mytask->alarm_hdl == 0){
	  // diag_printf("fosi_internal.hpp rtos_task_set_period() WARNING: Alarm not yet initialized, doing this now...\n");
	  rtos_task_make_periodic(mytask,nanosecs);
	}
	else {
	  cyg_alarm_initialize(mytask->alarm_hdl,
			       rtos_get_time_ticks() + nano2ticks(nanosecs),
			       nano2ticks(nanosecs));
	  // diag_printf("fosi_internal.hpp rtos_task_set_period() Setting period to %d ticks...\n",nano2ticks(nanosecs));
	// FIXME need call to cyg_alarm_enable here, or is this
	// "included" in initialize??
	}
      }
      else /* period is zero */ {
	// Check if alarm already created...
        if (mytask->alarm_hdl != 0) {
	  // diag_printf("fosi_internal.hpp rtos_task_set_period() WARNING: Disabling alarm...\n");
	  cyg_alarm_disable(mytask->alarm_hdl);
	}
      }
    }

    INTERNAL_QUAL void rtos_task_set_wait_period_policy( RTOS_TASK* task, int policy )
    {
      // Do nothing
    }

    INTERNAL_QUAL int rtos_task_wait_period( RTOS_TASK* task )
    {
      cyg_semaphore_wait(&(task->wakeup_sem));
      // should be used to detect overruns
      return 0;
    }

    INTERNAL_QUAL void rtos_task_delete(RTOS_TASK* mytask) {
      // Free name
      free(mytask->name);
      mytask->name = 0;
      // KG: Peter does not check return values, it appears...
      bool succeed = cyg_thread_delete(mytask->handle);
      if (succeed == false)
          diag_printf("cyg_thread_delete: Error deleting task\n");
      // Free stack space
      free(mytask->stack);
      mytask->stack = 0;
    }

    // for both SingleTread and PeriodicThread
    INTERNAL_QUAL const char * rtos_task_get_name(const RTOS_TASK* t)
    {
      cyg_thread_info info;
      bool succeed = cyg_thread_get_info(t->handle,cyg_thread_get_id(t->handle),&info);
      if (succeed == false)
          diag_printf("fosi_internal.hpp rtos_task_get_name() WARNING: cyg_thread_get_info returned false...\n");
      return info.name;
    }

    INTERNAL_QUAL void rtos_task_set_name(RTOS_TASK* t, const char * name)
    {
      return;
    }

    INTERNAL_QUAL int rtos_task_set_priority(RTOS_TASK *t, int priority)
    {
      cyg_thread_set_priority(t->handle,(cyg_priority_t) priority);
      return int(cyg_thread_get_priority(t->handle)) == priority ? 0 : 1;
    }

    INTERNAL_QUAL int rtos_task_get_priority(const RTOS_TASK *t)
    {
      return (int) cyg_thread_get_priority(t->handle);
    }

    INTERNAL_QUAL int rtos_task_set_cpu_affinity(RTOS_TASK * task, unsigned cpu_affinity)
    {
    return -1;
    }

    INTERNAL_QUAL unsigned rtos_task_get_cpu_affinity(const RTOS_TASK *task)
    {
    return ~0;
    }

	INTERNAL_QUAL unsigned int rtos_task_get_pid(const RTOS_TASK* task)
	{
		return 0;
	}

      INTERNAL_QUAL int rtos_task_set_scheduler(RTOS_TASK* t, int sched_type) {
          if (sched_type == SCHED_ECOS_FIFO )
              return 0;
          return -1;
      }

      INTERNAL_QUAL int rtos_task_get_scheduler(const RTOS_TASK* mytask) {
          return SCHED_ECOS_FIFO;
      }

    INTERNAL_QUAL int rtos_task_check_scheduler(int* scheduler)
    {
        if (*scheduler != SCHED_ECOS_FIFO )
            log(Error) << "Unknown scheduler type." <<endlog();
            *scheduler = SCHED_ECOS_FIFO;
            return -1;
        }
        return 0;
    }

      INTERNAL_QUAL int rtos_task_check_priority(int* scheduler, int* priority)
      {
          int ret = 0;
          ret = rtos_task_check_scheduler(&scheduler);

          // FIXME: what are the valid priority ranges ???

          return ret;
      }






  }
}
#undef INTERNAL_QUAL

