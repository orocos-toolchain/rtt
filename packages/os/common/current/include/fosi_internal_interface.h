
/**
 * @file fosi_task_interface.h
 * This file has all the (periodic) thread specific interfaces.
 * If a function does not apply to your OS, it may be left empty.
 *
 * Rename to 'fosi_internal.h'
 */
 
#ifndef ORO_OS_FOSI_INTERNAL_INTERFACE_HPP
#define ORO_OS_FOSI_INTERNAL_INTERFACE_HPP

#error "This file is a template for developers, do not include directly."

#include "os/ThreadInterface.hpp"
#include "os/fosi.h"

namespace ORO_OS
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
	
  }
}
#endif
