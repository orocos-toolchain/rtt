
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

struct TaskType {
};

struct ThreadType {
};

namespace ORO_OS
{
    namespace detail {
		// Bug: rtos_thread_join(RTOS_THREAD* thread) is not defined.

        int rtos_thread_create(RTOS_THREAD* thread, void * (*start_routine)(void *), ThreadInterface* obj);

        RTOS_TASK* rtos_task_init( ThreadInterface* thread );

        void rtos_task_yield(RTOS_TASK*);

        void rtos_task_make_hard_real_time(RTOS_TASK*);

        void rtos_task_make_soft_real_time(RTOS_TASK*);

        int rtos_task_is_hard_real_time(RTOS_TASK*);

        void rtos_task_make_periodic(RTOS_TASK* mytask, NANO_TIME nanosecs );

        void rtos_task_set_period( RTOS_TASK* mytask, NANO_TIME nanosecs );

        int rtos_task_wait_period( RTOS_TASK* task );

        void rtos_task_delete(RTOS_TASK* mytask);

        // for both SingleTread and PeriodicThread
        template<class T>
        void rtos_thread_init( T* thread, const std::string& name );

        int rtos_set_scheduler(int type, int priority);
    }
}
#endif
