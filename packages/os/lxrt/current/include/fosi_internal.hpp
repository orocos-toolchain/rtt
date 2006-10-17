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
 
 
#ifndef OS_FOSI_INTERNAL_HPP
#define OS_FOSI_INTERNAL_HPP
#define OROBLD_OS_LXRT_INTERNAL

#include <iostream>
#include <sched.h>
#include "ThreadInterface.hpp"
#include "fosi.h"
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include "pkgconf/system.h"
#define INTERNAL_QUAL static inline

#include <string.h>

#include "rtt/Logger.hpp"

namespace RTT
{ namespace OS {
    namespace detail {

        INTERNAL_QUAL int rtos_task_create_main(RTOS_TASK* main_task)
        {
            if ( getuid() != 0 ) {
                std::cerr << "You are not root. This program requires that you are root." << std::endl;
                exit(1);
            }

            /* check to see if rtai_lxrt module is loaded */
            //         struct module_info modInfo;
            //         size_t retSize;
            //         if ( query_module("rtai_lxrt", QM_INFO, &modInfo, 
            //                           sizeof(modInfo), &retSize) != 0 ) {
            //             std::cerr <<"It appears the rtai_lxrt module is not loaded !"<<std::endl;
            //             exit();
            //         }
            struct sched_param param;

            param.sched_priority = sched_get_priority_max(SCHED_OTHER);
            if (param.sched_priority != -1 )
                sched_setscheduler( 0, SCHED_OTHER, &param);

            unsigned long name = nam2num("main");
            while ( rt_get_adr( name ) != 0 ) // check for existing 'MAINTHREAD'
                ++name;

            main_task->name = "main";

            if( !(main_task->rtaitask = rt_task_init(name, 10,0,0)) ) // priority, stack, msg_size
                {
                    std::cerr << "Cannot rt_task_init() MainThread." << std::endl;
                    exit(1);
                }
                
#ifdef OROSEM_OS_LXRT_PERIODIC
            rt_set_periodic_mode();
            start_rt_timer( nano2count( NANO_TIME(ORODAT_OS_LXRT_PERIODIC_TICK*1000*1000*1000) ) );
            Logger::log() << Logger::Info << "RTAI Periodic Timer ticks at "<<ORODAT_OS_LXRT_PERIODIC_TICK<<" seconds." << Logger::endl;
#else
            // BE SURE TO SET rt_preempt_always(1) when using one shot mode
            rt_set_oneshot_mode();
            // only call this function for RTAI 3.0 or older
#if defined(CONFIG_RTAI_VERSION_MINOR) && defined(CONFIG_RTAI_VERSION_MAJOR)
#  if CONFIG_RTAI_VERSION_MAJOR == 3 && CONFIG_RTAI_VERSION_MINOR == 0
            rt_preempt_always(1);
#  endif
#else
            rt_preempt_always(1);
#endif
            start_rt_timer(0);
            Logger::log() << Logger::Info << "RTAI Periodic Timer runs in preemptive 'one-shot' mode." << Logger::endl;
#endif
            Logger::log() << Logger::Debug << "RTAI Task Created" << Logger::endl;
            // stack, heap : see touchall.c in lxrt/lib
            // lock_all(stk,hp);
            mlockall(MCL_CURRENT | MCL_FUTURE );
            return 0;
        }

        INTERNAL_QUAL int rtos_task_delete_main(RTOS_TASK* main_task)
        {
            // we don't stop the timer
            //stop_rt_timer();
            rt_task_delete(main_task->rtaitask);
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

            // New default: new threads are always hard.
            rt_make_hard_real_time();

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
      
            // Set priority
            task->priority = priority;
	    
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

        INTERNAL_QUAL int rtos_task_set_scheduler(RTOS_TASK* t, int s) {
            Logger::In in( t->name );
            if ( t->rtaitask != rt_buddy() ) {
                log(Error) << "RTAI/LXRT can not change the scheduler type from another thread." <<endlog();
                return -1;
            }
            if (s == SCHED_LXRT_HARD)
                rt_make_hard_real_time();
            else if ( s == SCHED_LXRT_SOFT)
                rt_make_soft_real_time();
            else {
                log(Error) << "Unknown scheduler type for RTAI/LXRT. Use SCHED_LXRT_HARD or SCHED_LXRT_SOFT." <<endlog();
                return -1;
            }
            return 0;
        }

        INTERNAL_QUAL int rtos_task_get_scheduler(const RTOS_TASK* t) {
            if ( rt_is_hard_real_time( t->rtaitask ) )
                return SCHED_LXRT_HARD;
            return SCHED_LXRT_SOFT;
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
}}
#undef INTERNAL_QUAL
#endif
