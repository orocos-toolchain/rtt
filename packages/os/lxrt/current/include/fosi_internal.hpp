#ifndef ORO_OS_FOSI_INTERNAL_HPP
#define ORO_OS_FOSI_INTERNAL_HPP

#include "os/ThreadInterface.hpp"
#define OROBLD_OS_LXRT_INTERNAL
#include "os/fosi.h"
#include <iostream>
#define INTERNAL_QUAL static inline

namespace ORO_OS
{
    namespace detail {

        INTERNAL_QUAL RTOS_TASK* rtos_task_init( ThreadInterface* thread )
        {
            RTOS_TASK* mytask = 0;
            unsigned long mytask_name;
            mytask_name = nam2num( thread->getName() );
            // name, priority, stack_size, msg_size, policy, cpus_allowed ( 1111 = 4 first cpus)
            if (!(mytask = rt_task_init(mytask_name, thread->getPriority(), 0, 0))) {
                std::cerr << thread->getName() << " : CANNOT INIT LXRT TASK " << mytask_name <<std::endl;
                std::cerr << "Exiting this thread." <<std::endl;
                exit (-1); // can not return 0 because main is blocked on sem.
            }
            return mytask;
        }

        INTERNAL_QUAL void rtos_task_make_hard_real_time(RTOS_TASK*) {
            rt_make_hard_real_time();
        }

        INTERNAL_QUAL void rtos_task_make_soft_real_time(RTOS_TASK*) {
            rt_make_soft_real_time();
        }

        INTERNAL_QUAL int rtos_task_is_hard_real_time(RTOS_TASK* t) {
            return rt_is_hard_real_time( t );
        }

        INTERNAL_QUAL void rtos_task_make_periodic(RTOS_TASK* mytask, RTIME nanosecs )
        {
            rt_task_make_periodic_relative_ns(mytask, 0, nanosecs);
        }

        INTERNAL_QUAL void rtos_task_set_period( RTOS_TASK* mytask, RTIME nanosecs )
        {
            rt_set_period(mytask, nano2count( nanosecs ));
        }

        INTERNAL_QUAL void rtos_task_wait_period( RTOS_TASK* )
        {
            rt_task_wait_period();
        }

        INTERNAL_QUAL void rtos_task_delete(RTOS_TASK* mytask) {
            rt_task_delete(mytask);
        }

        // for both SingleTread and PeriodicThread
        template<class T>
        INTERNAL_QUAL void rtos_thread_init( T* thread, std::string name ) {
            if ( name.empty() )
                name = "Thread";
            if ( rt_get_adr(nam2num( name.c_str() )) == 0 )
                thread->setName(name.c_str());
            else {
                unsigned long nname = nam2num( name.c_str() );
                while ( rt_get_adr( nname ) != 0 ) // check for existing 'NAME'
                    ++nname;
                char taskName[7];
                num2nam( nname, taskName); // set taskName to new name
                taskName[6] = 0;
                thread->setName( taskName );
            }
        }

        INTERNAL_QUAL int rtos_set_scheduler(int type, int priority)
        {
            // init the scheduler. The rt_task_initschmod code is broken, so we do it ourselves.
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
