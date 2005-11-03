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
 
 
#ifndef ORO_OS_FOSI_INTERNAL_HPP
#define ORO_OS_FOSI_INTERNAL_HPP

#include "os/ThreadInterface.hpp"
#include "os/fosi.h"
#include <iostream>

#define INTERNAL_QUAL static inline

struct GNUTask {
    GNUTask( NANO_TIME periodi ) : periodMark(0), period( periodi ) {}
    NANO_TIME periodMark;
    NANO_TIME period;
};

namespace ORO_OS
{
    namespace detail {

        INTERNAL_QUAL RTOS_TASK* rtos_task_init( ThreadInterface* thread )
        {
            return new GNUTask( thread->getPeriodNS() );
        }

        INTERNAL_QUAL void rtos_task_make_hard_real_time(RTOS_TASK*) {
        }

        INTERNAL_QUAL void rtos_task_make_soft_real_time(RTOS_TASK*) {
        }

        INTERNAL_QUAL int rtos_task_is_hard_real_time(RTOS_TASK*) {
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

        INTERNAL_QUAL void rtos_task_wait_period( RTOS_TASK* task )
        {
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
        }

        INTERNAL_QUAL void rtos_task_delete(RTOS_TASK* mytask) {
            delete mytask;
        }

        // for both SingleTread and PeriodicThread
        template<class T>
        INTERNAL_QUAL void rtos_thread_init( T* thread, const std::string& name ) {
            if ( name.empty() )
               thread->setName( "Thread" );
            else
                thread->setName( name.c_str() );
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
