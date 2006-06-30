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
#define OROBLD_OS_LXRT_INTERNAL

#include "os/ThreadInterface.hpp"
#include "os/fosi.h"
#include <iostream>
#define INTERNAL_QUAL static inline

#include <string.h>

namespace ORO_OS
{
    namespace detail {

        /**
         * Helper function to convert RTAI to POSIX priority.
         */
        INTERNAL_QUAL int internal_posix_priority(int priority)
        {
            // FIXME mytask is not taken into account
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
            sched_setscheduler(0, OROSEM_OS_SCHEDTYPE, &mysched);
            return 0;
        }

        struct RTAI_Thread
        {
            void *(*wrapper)(void*);
            void *data;
            RTOS_TASK* task;
            int priority;
            unsigned int tnum;
        };

        INTERNAL_QUAL void* rtai_thread_wrapper( void * arg ) {
            RTAI_Thread* d = (RTAI_Thread*)arg;
            RTOS_TASK* task = d->task;
            void* data =  d->data;
            int priority = d->priority;
            unsigned int tnum = d->tnum;
            void*(*wrapper)(void*) = d->wrapper;
            free( d );

            if (!(task->rtaitask = rt_task_init(tnum, priority, 0, 0))) {
                std::cerr << "CANNOT INIT LXRT Thread " << task->name <<std::endl;
                std::cerr << "Exiting this thread." <<std::endl;
                exit(-1);
            }
            // Set POSIX priority of this thread.
            internal_posix_priority(priority);

            return wrapper( data );
        }

        INTERNAL_QUAL int rtos_task_create(RTOS_TASK* task,
                                           int priority,
                                           const char* name,
                                           int sched_type,
                                           void * (*start_routine)(void *),
                                           ThreadInterface* obj) 
        {
            char taskName[7];
            if ( strlen(name) == 0 )
                name = "Thread";
            strncpy(taskName, name, 7);
            unsigned long task_num = nam2num( taskName );
            if ( rt_get_adr(nam2num( taskName )) != 0 ) {
                unsigned long nname = nam2num( taskName );
                while ( rt_get_adr( nname ) != 0 ) // check for existing 'NAME'
                    ++nname;
                num2nam( nname, taskName); // set taskName to new name
                taskName[6] = 0;
                task_num = nname;
            }

            // Set and truncate name
            task->name = strcpy( (char*)malloc( (strlen(name)+1)*sizeof(char) ), name);
            // name, priority, stack_size, msg_size, policy, cpus_allowed ( 1111 = 4 first cpus)
      
            RTAI_Thread* rt = (RTAI_Thread*)malloc( sizeof(RTAI_Thread) );
            rt->priority = priority;
            rt->data = obj;
            rt->wrapper = start_routine;
            rt->task = task;
            rt->tnum = task_num;
            return pthread_create(&(task->thread), 0, 
                                  rtai_thread_wrapper, rt);
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

        INTERNAL_QUAL int rtos_task_is_hard_real_time(const RTOS_TASK* t) {
            return rt_is_hard_real_time( t->rtaitask );
        }

        INTERNAL_QUAL void rtos_task_make_periodic(RTOS_TASK* mytask, RTIME nanosecs )
        {
            if (nanosecs == 0) {
                // in RTAI, to drop from periodic to non periodic, do a 
                // suspend/resume cycle.
                rt_task_suspend( mytask->rtaitask );
                rt_task_resume( mytask->rtaitask );
            }
            else {
	        // same for the inverse
	        rt_task_suspend( mytask->rtaitask );
                rt_task_make_periodic_relative_ns(mytask->rtaitask, 0, nanosecs);
	    }
        }

        INTERNAL_QUAL void rtos_task_set_period( RTOS_TASK* mytask, RTIME nanosecs )
        {
            rt_set_period(mytask->rtaitask, nano2count( nanosecs ));
        }

        INTERNAL_QUAL int rtos_task_wait_period( RTOS_TASK* mytask )
        {
            // only in RTAI 3.2, this returns overrun or not.
            // so do not use retval for compatibility reasons.
            rt_task_wait_period();
            return 0;
        }

        INTERNAL_QUAL void rtos_task_delete(RTOS_TASK* mytask) {
            free( mytask->name );
            rt_task_delete(mytask->rtaitask);
        }

        INTERNAL_QUAL int rtos_task_set_priority(RTOS_TASK * mytask, int priority)
        {
            // FIXME mytask is not taken into account
            internal_posix_priority(priority);
            int rv;
            if ( (rv = rt_change_prio( mytask->rtaitask, priority)) == 0) {
                mytask->priority = priority;
                return 0;
            }
            return rv;
        }

        INTERNAL_QUAL const char * rtos_task_get_name(const RTOS_TASK* t)
        {
            return t->name;
        }
        
        INTERNAL_QUAL int rtos_task_get_priority(const RTOS_TASK *t)
        {
            return t->priority;
        }

    }
}
#undef INTERNAL_QUAL
#endif
