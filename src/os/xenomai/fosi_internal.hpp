/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  fosi_internal.hpp 

                        fosi_internal.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 

#ifndef OS_FOSI_INTERNAL_HPP
#define OS_FOSI_INTERNAL_HPP

#define OROBLD_OS_INTERNAL
#include "os/ThreadInterface.hpp"
#include "fosi.h"
#include <iostream>
#define INTERNAL_QUAL static inline

#include "Logger.hpp"

namespace RTT
{ namespace OS {
    namespace detail {

        INTERNAL_QUAL int rtos_task_create_main(RTOS_TASK* main)
        {
            // first check if root.
            if ( geteuid() != 0 ) {
                printf( "You are not root. This program requires that you are root.\n");
                exit(1);
            }

            // locking of all memory for this process
            int rv = mlockall(MCL_CURRENT | MCL_FUTURE);
            if ( rv != 0 ) {
                perror( "rtos_task_create_main: Could not lock memory using mlockall" ); // Logger unavailable.
            }

            struct sched_param param;
            // we set the MT to the highest sched priority to allow the console
            // to interrupt a loose running thread.
            param.sched_priority = sched_get_priority_max(ORO_SCHED_OTHER);
            if (param.sched_priority != -1 )
                sched_setscheduler( 0, ORO_SCHED_OTHER, &param);

            const char* mt_name = "MainThread";

            int ret = -1;
            while( ret != 0) {
                // name, priority, mode
                if ( (ret = rt_task_shadow( &(main->xenotask),mt_name, 10, 0)) != 0 ) {
                    if ( ret == -ENOMEM ) {
                        // fail: abort
                        printf( "Cannot rt_task_create() MainThread: Out of memory.\n");
                        exit(1);
                    }
                    if ( ret == -EBUSY ) {
                        // ok: we are a xeno thread (may log() ):
                        log(Info) << "MainThread already a Xenomai task." <<endlog();
                        break;
                    }
                    if ( ret == -EEXIST ) {
                        // fail: retry without using a name.
                        mt_name = 0; // do not register
                        continue;
                    }
                    if ( ret == -EPERM ) {
                        // fail: abort
                        printf( "Can not rt_task_create() MainThread: No permission.\n");
                        exit(1);
                    }
                }
            }
            // We are a xeno thread now:
            // Only use Logger after this point (i.e. when rt_task_shadow was succesful).
            if ( mt_name == 0) {
                log(Warning) << "'MainThread' name was already in use.\n" <<endlog();
            }

#ifdef OROSEM_OS_XENO_PERIODIC
# if CONFIG_XENO_VERSION_MAJOR == 2 && CONFIG_XENO_VERSION_MINOR == 0
            // time in nanoseconds
            rt_timer_start( ORODAT_OS_XENO_PERIODIC_TICK*1000*1000*1000 );
            Logger::In in("Scheduler");
            Logger::log() << Logger::Info << "Xenomai Periodic Timer started using "<<ORODAT_OS_XENO_PERIODIC_TICK<<" seconds." << Logger::endl;
# else
            Logger::In in("Scheduler");
            Logger::log() << Logger::Error << "Set Xenomai Periodic Timer using the Linux kernel configuration." << Logger::endl;
# endif
#else
# if CONFIG_XENO_VERSION_MAJOR == 2 && CONFIG_XENO_VERSION_MINOR == 0
            rt_timer_start( TM_ONESHOT );
            Logger::log() << Logger::Info << "Xenomai Periodic Timer runs in preemptive 'one-shot' mode." << Logger::endl;
# else
#  if CONFIG_XENO_OPT_TIMING_PERIODIC
            Logger::log() << Logger::Info << "Xenomai Periodic Timer configured in 'periodic' mode." << Logger::endl;
#   else
            Logger::log() << Logger::Info << "Xenomai Periodic Timer runs in preemptive 'one-shot' mode." << Logger::endl;
#  endif
# endif
#endif

            Logger::log() << Logger::Debug << "Xenomai Timer and Main Task Created" << Logger::endl;
            return 0;
        }

        INTERNAL_QUAL int rtos_task_delete_main(RTOS_TASK* main_task)
        {
            rt_task_delete( &(main_task->xenotask) );
            munlockall();
            return 0;
        }


        struct XenoCookie {
            void* data;
            void* (*wrapper)(void*);
        };

        INTERNAL_QUAL void rtos_xeno_thread_wrapper( void* cookie )
        {
            // store 'self'
            RTOS_TASK* task = ((ThreadInterface*)((XenoCookie*)cookie)->data)->getTask();
            task->xenoptr = rt_task_self();

            // create hard mode by default.
            rt_task_set_mode( 0, T_PRIMARY, 0 );

            // call user function
            ((XenoCookie*)cookie)->wrapper( ((XenoCookie*)cookie)->data );
            free(cookie);
        }

        INTERNAL_QUAL int rtos_task_create(RTOS_TASK* task,
                                           int priority,
                                           const char* name,
                                           int sched_type,
                                           void * (*start_routine)(void *),
                                           ThreadInterface* obj) 
        {
            if ( priority < 1 )
                priority = 1;
            if ( priority > 99)
                priority = 99;
            XenoCookie* xcookie = (XenoCookie*)malloc( sizeof(XenoCookie) );
            xcookie->data = obj;
            xcookie->wrapper = start_routine;
            if ( name == 0 || strlen(name) == 0)
                name = "XenoThread";
            task->name = strncpy( (char*)malloc( (strlen(name)+1)*sizeof(char) ), name, strlen(name)+1 );
            int rv;
            // task, name, stack, priority, mode, fun, arg
            // UGLY, how can I check in Xenomai that a name is in use before calling rt_task_spawn ???
            rv = rt_task_spawn(&(task->xenotask), name, 0, priority, 0, rtos_xeno_thread_wrapper, xcookie);
            if ( rv == -EEXIST ) {
                free( task->name );
                task->name = strncpy( (char*)malloc( (strlen(name)+2)*sizeof(char) ), name, strlen(name)+1 );
                task->name[ strlen(name) ] = '0';
                task->name[ strlen(name)+1 ] = 0;
                while ( rv == -EEXIST &&  task->name[ strlen(name) ] != '9') {
                    task->name[ strlen(name) ] += 1;
                    rv = rt_task_spawn(&(task->xenotask), task->name, 0, priority, 0, rtos_xeno_thread_wrapper, xcookie);
                }
            }
            if ( rv == -EEXIST ) {
                log(Warning) << name << ": an object with that name is already existing in Xenomai." << endlog();
                rv = rt_task_spawn(&(task->xenotask), 0, 0, priority, 0, rtos_xeno_thread_wrapper, xcookie);
            }
            if ( rv != 0) {
                log(Error) << name << " : CANNOT INIT Xeno TASK " << task->name <<" error code: "<< rv << endlog();
            }
            return rv;
        }

        INTERNAL_QUAL void rtos_task_yield(RTOS_TASK*) {
            rt_task_yield();
        }

        // we could implement here the interrupt shield logic.
        INTERNAL_QUAL int rtos_task_set_scheduler(RTOS_TASK* t, int sched_type) {
            Logger::In in( t->name );
            // xenoptr was initialised from the thread wrapper.
            if (t->xenoptr != rt_task_self() ) {
                log(Error) << "Xenomai can not change the scheduler type from another thread." <<endlog();
                return -1;
            }
            if (sched_type == SCHED_XENOMAI_HARD)
                return rt_task_set_mode( 0, T_PRIMARY, 0 );
            else if ( sched_type == SCHED_XENOMAI_SOFT)
                return rt_task_set_mode( T_PRIMARY, 0, 0 );
            log(Error) << "Unknown scheduler type for Xenomai. Use SCHED_XENOMAI_SOFT or SCHED_XENOMAI_HARD." <<endlog();
            return -1;
        }

        INTERNAL_QUAL int rtos_task_get_scheduler(const RTOS_TASK* mytask) {

            RT_TASK_INFO info;
            // WORK AROUND constness: (need non-const mytask)
            RT_TASK* tt = mytask->xenoptr;
            if ( tt )
                if ( rt_task_inquire ( tt, &info) == 0 ) 
                    return info.status & T_PRIMARY;
            return -1;
        }

        INTERNAL_QUAL void rtos_task_make_periodic(RTOS_TASK* mytask, RTIME nanosecs )
        {
            if (nanosecs == 0) {
                rt_task_set_periodic( &(mytask->xenotask), TM_NOW, TM_INFINITE);
            }
            else {
                rt_task_set_periodic( &(mytask->xenotask), TM_NOW, rt_timer_ns2ticks(nanosecs) );
            }
        }

        INTERNAL_QUAL void rtos_task_set_period( RTOS_TASK* mytask, RTIME nanosecs )
        {
            rtos_task_make_periodic( mytask, nanosecs);
            //rt_task_set_period(&(mytask->xenotask), rt_timer_ns2ticks( nanosecs ));
        }

        INTERNAL_QUAL int rtos_task_wait_period( RTOS_TASK* mytask )
        {
            // detect overrun.
#if CONFIG_XENO_VERSION_MAJOR == 2 && CONFIG_XENO_VERSION_MINOR == 0
            if ( rt_task_wait_period() == -ETIMEDOUT) 
                return 1;
#else // 2.1, 2.2, 2.3,...
            long unsigned int overrun = 0;
            rt_task_wait_period(&overrun);
            if ( overrun != 0)
                return 1;
#endif
            return 0;
        }

        INTERNAL_QUAL const char* rtos_task_get_name(const RTOS_TASK* mytask) {
            return mytask->name;
        }

        INTERNAL_QUAL int rtos_task_get_priority(const RTOS_TASK* mytask) {
            RT_TASK_INFO info;
            // WORK AROUND constness: (need non-const mytask)
            RT_TASK* tt = mytask->xenoptr;
            if ( tt )
                if ( rt_task_inquire ( tt, &info) == 0 ) 
                    return info.bprio;
            return -1;
        }

        INTERNAL_QUAL void rtos_task_delete(RTOS_TASK* mytask) {
            rt_task_delete(&(mytask->xenotask));
        }

        INTERNAL_QUAL int rtos_task_set_priority(RTOS_TASK * mytask, int priority)
        {
            // ignorint 'type'
            RT_TASK* tt = mytask->xenoptr;
            if ( tt )
                return rt_task_set_priority( tt, priority);
            return -1;
        }
    }
}}
#undef INTERNAL_QUAL
#endif
