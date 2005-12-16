/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:50 CEST 2005  fosi_internal.hpp 

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

        INTERNAL_QUAL int rtos_thread_create(RTOS_THREAD* thread, void * (*start_routine)(void *), ThreadInterface* obj) {
            return pthread_create(thread, 0, start_routine, obj);
        }

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
            // a task must be runnable when rtos_task_init returns.
            //rt_task_resume( mytask );
            return mytask;
        }

        INTERNAL_QUAL void rtos_task_yield(RTOS_TASK*) {
            rt_task_yield();
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
            if (nanosecs == 0) {
                // in RTAI, to drop from periodic to non periodic, do a 
                // suspend/resume cycle.
                rt_task_suspend( mytask );
                rt_task_resume( mytask );
            }
            else {
	        // same for the inverse
	        rt_task_suspend( mytask );
                rt_task_make_periodic_relative_ns(mytask, 0, nanosecs);
	    }
        }

        INTERNAL_QUAL void rtos_task_set_period( RTOS_TASK* mytask, RTIME nanosecs )
        {
            rt_set_period(mytask, nano2count( nanosecs ));
        }

        INTERNAL_QUAL int rtos_task_wait_period( RTOS_TASK* mytask )
        {
            // only in RTAI 3.2, this returns overrun or not.
            // so do not use retval for compatibility reasons.
            rt_task_wait_period();
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
