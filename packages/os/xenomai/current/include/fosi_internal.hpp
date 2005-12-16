
#ifndef ORO_OS_FOSI_INTERNAL_HPP
#define ORO_OS_FOSI_INTERNAL_HPP

#include "os/ThreadInterface.hpp"
#define OROBLD_OS_INTERNAL
#include "os/fosi.h"
#include <iostream>
#define INTERNAL_QUAL static inline

namespace ORO_OS
{
    namespace detail {

        INTERNAL_QUAL int rtos_thread_create(RTOS_THREAD* thread, void * (*start_routine)(void *), ThreadInterface* obj) {
            return pthread_create(thread, 0, start_routine, obj);
        }

        INTERNAL_QUAL RTOS_TASK* rtos_task_init( ThreadInterface* thread )
        {
            RTOS_TASK* mytask = 0;
            // start not suspended.
            int priority;
            if ( thread )
                priority = thread->getPosixPriority();
            else
                priority = 99;
            if ( rt_task_shadow( mytask, 0, priority , 0) != 0 ) {
                std::cerr << thread->getName() << " : CANNOT INIT XENO TASK " << thread->getName() <<std::endl;
                std::cerr << "Exiting this thread." <<std::endl;
                exit (-1); // can not return 0 because main is blocked on sem.
            }
            // a task must be runnable when rtos_task_init returns.
            return mytask;
        }

        INTERNAL_QUAL void rtos_task_yield(RTOS_TASK*) {
            rt_task_yield();
        }

        // we could implement here the interrupt shield logic.
        INTERNAL_QUAL void rtos_task_make_hard_real_time(RTOS_TASK*) {
            rt_task_set_mode( 0, T_PRIMARY, 0 );
        }

        INTERNAL_QUAL void rtos_task_make_soft_real_time(RTOS_TASK*) {
            rt_task_set_mode( T_PRIMARY, 0, 0 );
        }

        INTERNAL_QUAL int rtos_task_is_hard_real_time(RTOS_TASK* t) {
            int mode;
            rt_task_set_mode( 0, 0, &mode );
            return mode & T_PRIMARY;
        }

        INTERNAL_QUAL void rtos_task_make_periodic(RTOS_TASK* mytask, RTIME nanosecs )
        {
            if (nanosecs == 0) {
                rt_task_set_periodic( mytask, TM_NOW, TM_INFINITE);
            }
            else {
                rt_task_set_periodic( mytask, TM_NOW, nano2ticks(nanosecs) );
            }
        }

        INTERNAL_QUAL void rtos_task_set_period( RTOS_TASK* mytask, RTIME nanosecs )
        {
            rtos_task_make_periodic( mytask, nanosecs);
            //rt_task_set_period(mytask, nano2ticks( nanosecs ));
        }

        INTERNAL_QUAL int rtos_task_wait_period( RTOS_TASK* mytask )
        {
            // detect overrun.
            if ( rt_task_wait_period() == -ETIMEDOUT) 
                return 1;
            return 0;
        }

        INTERNAL_QUAL void rtos_task_delete(RTOS_TASK* mytask) {
            rt_task_delete(mytask);
        }

        // for both SingleTread and PeriodicThread
        template<class T>
        INTERNAL_QUAL void rtos_thread_init( T* thread, std::string name ) {
            if ( name.empty() )
                name = "Thread";
            thread->setName(name.c_str());
        }

        INTERNAL_QUAL int rtos_set_scheduler(int type, int priority)
        {
            // init the scheduler.
            struct sched_param mysched;
            mysched.sched_priority = sched_get_priority_max(type) - priority;
            // check lower bounds :
            if (type == SCHED_OTHER && mysched.sched_priority != 0 ) {
                mysched.sched_priority = 0; // SCHED_OTHER must be zero
            } else if (type == !SCHED_OTHER &&  mysched.sched_priority < 1 ) {
                mysched.sched_priority = 1; // !SCHED_OTHER must be 1 or higher
            }
            // check upper bound
            if ( mysched.sched_priority > 99)
                mysched.sched_priority = 99;
            // set scheduler
            return sched_setscheduler(0, type, &mysched);
        }
    }
}
#undef INTERNAL_QUAL
#endif
