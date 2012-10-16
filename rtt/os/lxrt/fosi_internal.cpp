/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:50 CEST 2005  fosi_internal.hpp

                        fosi_internal.hpp -  description
                           -------------------
    begin                : Sat May 21 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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
#define OROBLD_OS_LXRT_INTERNAL

#include <iostream>
#include <sched.h>
#include "os/ThreadInterface.hpp"
#include "fosi.h"
#include "../fosi_internal_interface.hpp"
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include "../../rtt-config.h"
#define INTERNAL_QUAL

#include <string.h>

#include "../../Logger.hpp"

namespace RTT
{
    namespace os {

        INTERNAL_QUAL int rtos_task_create_main(RTOS_TASK* main_task)
        {
            if ( geteuid() != 0 ) {
                std::cerr << "You are not root. This program requires that you are root." << std::endl;
                exit(1);
            }

#ifdef OROSEM_OS_LOCK_MEMORY
            int locktype = MCL_CURRENT;
#ifdef OROSEM_OS_LOCK_MEMORY_FUTURE
            locktype |= MCL_FUTURE;
#endif
            // locking of all memory for this process
            int rv = mlockall(locktype);
            if ( rv != 0 ) {
                perror( "rtos_task_create_main: Could not lock memory using mlockall" ); // Logger unavailable.
            }
#endif

            /* check to see if rtai_lxrt module is loaded */
            //         struct module_info modInfo;
            //         size_t retSize;
            //         if ( query_module("rtai_lxrt", QM_INFO, &modInfo,
            //                           sizeof(modInfo), &retSize) != 0 ) {
            //             std::cerr <<"It appears the rtai_lxrt module is not loaded !"<<std::endl;
            //             exit();
            //         }
            unsigned long name = nam2num("main");
            while ( rt_get_adr( name ) != 0 ) // check for existing 'MAINTHREAD'
                ++name;


            const char* tname = "main";
            main_task->name = strcpy( (char*)malloc( (strlen(tname) + 1) * sizeof(char)), tname);

            if( !(main_task->rtaitask = rt_task_init(name, 10,0,0)) ) // priority, stack, msg_size
                {
                    std::cerr << "Cannot rt_task_init() MainThread." << std::endl;
                    exit(1);
                }

            struct sched_param param;

            param.sched_priority = sched_get_priority_max(SCHED_OTHER);
            if (param.sched_priority != -1 )
                sched_setscheduler( 0, SCHED_OTHER, &param);

            // Avoid the LXRT CHANGED MODE (TRAP), PID = 4088, VEC = 14, SIGNO = 11. warning
            rt_task_use_fpu(main_task->rtaitask, 1);

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
            return 0;
        }

        INTERNAL_QUAL int rtos_task_delete_main(RTOS_TASK* main_task)
        {
            // we don't stop the timer
            //stop_rt_timer();
            rt_task_delete(main_task->rtaitask);
            free(main_task->name);
            main_task->name = 0;
            munlockall();
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

            // Schedule in Linux' SCHED_OTHER
            struct sched_param param;
            param.sched_priority = sched_get_priority_max(SCHED_OTHER);
            if (param.sched_priority != -1 )
                sched_setscheduler( 0, SCHED_OTHER, &param);

            // Avoid the LXRT CHANGED MODE (TRAP), PID = 4088, VEC = 14, SIGNO = 11. warning
            rt_task_use_fpu(task->rtaitask, 1);

            // New default: new threads are always hard.
            rt_make_hard_real_time();

            data = wrapper( data );

            // Exit in soft mode to avoid RTAI warnings.
            rt_make_soft_real_time();
            // cleanup here to avoid "LXRT Releases PID" warnings.
            rt_task_delete(task->rtaitask);
            task->rtaitask = 0;
            // See rtos_task_delete for further cleanups.
            return data;
        }

        INTERNAL_QUAL int rtos_task_create(RTOS_TASK* task,
                                           int priority,
                                           unsigned cpu_affinity,
                                           const char* name,
                                           int sched_type,
                                           size_t stack_size,
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

            // Set rtai task struct to zero
            task->rtaitask = 0;

            RTAI_Thread* rt = (RTAI_Thread*)malloc( sizeof(RTAI_Thread) );
            rt->priority = priority;
            rt->data = obj;
            rt->wrapper = start_routine;
            rt->task = task;
            rt->tnum = task_num;
            int retv = pthread_create(&(task->thread), 0,
                                  rtai_thread_wrapper, rt);
            // poll for thread creation to be done.
            int timeout = 0;
            while ( task->rtaitask == 0 && ++timeout < 20)
                usleep(100000);
            return timeout <  20 ? retv : -1;
        }

        INTERNAL_QUAL void rtos_task_yield(RTOS_TASK* mytask) {
            if (mytask->rtaitask == 0)
                return;

            rt_task_yield();
        }

        INTERNAL_QUAL int rtos_task_is_self(const RTOS_TASK* task) {
            RT_TASK* self = rt_buddy();
            if (self == 0)
                return -1; // non-rtai thread. We could try to compare pthreads like in gnulinux ?
            if ( self == task->rtaitask )
                return 1;
            return 0;
        }

    	INTERNAL_QUAL unsigned int rtos_task_get_pid(const RTOS_TASK* task)
    	{
    		return 0;
    	}

        INTERNAL_QUAL int rtos_task_check_scheduler(int* scheduler)
        {
            if (*scheduler != SCHED_LXRT_HARD && *scheduler != SCHED_LXRT_SOFT ) {
                log(Error) << "Unknown scheduler type." <<endlog();
                *scheduler = SCHED_LXRT_SOFT;
                return -1;
            }
            return 0;
        }

        INTERNAL_QUAL int rtos_task_set_scheduler(RTOS_TASK* t, int s) {
            if ( t->rtaitask == 0 || t->rtaitask != rt_buddy() ) {
                return -1;
            }
            if (rtos_task_check_scheduler(&s) == -1)
                return -1;
            if (s == SCHED_LXRT_HARD)
                rt_make_hard_real_time();
            else if ( s == SCHED_LXRT_SOFT)
                rt_make_soft_real_time();
            return 0;
        }

        INTERNAL_QUAL int rtos_task_get_scheduler(const RTOS_TASK* t) {
            if ( rt_is_hard_real_time( t->rtaitask ) )
                return SCHED_LXRT_HARD;
            return SCHED_LXRT_SOFT;
        }

        INTERNAL_QUAL void rtos_task_make_periodic(RTOS_TASK* mytask, NANO_TIME nanosecs )
        {
            if (mytask->rtaitask == 0)
                return;
            if (rt_buddy() == mytask->rtaitask) {
                // do not suspend/resume my own thread
                // do best effort to change period.
                rtos_task_set_period(mytask,nanosecs);
                return;
            }
            // other thread is instructing us:
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

        INTERNAL_QUAL void rtos_task_set_period( RTOS_TASK* mytask, NANO_TIME nanosecs )
        {
            if (mytask->rtaitask == 0)
                return;
            if ( nanosecs == 0 )
                rt_set_period(mytask->rtaitask, 0 );
            else
                rt_set_period(mytask->rtaitask, nano2count( nanosecs ));
        }

        INTERNAL_QUAL void rtos_task_set_wait_period_policy( RTOS_TASK* task, int policy )
        {
          // Do nothing
        }

        INTERNAL_QUAL int rtos_task_wait_period( RTOS_TASK* mytask )
        {
            if (mytask->rtaitask == 0)
                return -1;
            // only in RTAI 3.2, this returns overrun or not.
            // so do not use retval for compatibility reasons.
            rt_task_wait_period();
            return 0;
        }

        INTERNAL_QUAL void rtos_task_delete(RTOS_TASK* mytask) {
            if ( pthread_join((mytask->thread),0) != 0 )
                Logger::log() << Logger::Critical << "Failed to join "<< mytask->name <<"."<< Logger::endl;

            free( mytask->name );
            mytask->name = 0;
            // rt_task_delete is done in wrapper !
        }

        INTERNAL_QUAL int rtos_task_check_priority(int* scheduler, int* priority)
        {
            int ret = 0;
            // check scheduler first.
            ret = rtos_task_check_scheduler(scheduler);

            // correct priority
            // Hard & Soft:
            if (*priority < 0){
                log(Warning) << "Forcing priority ("<<*priority<<") of thread to 0." <<endlog();
                *priority = 0;
                ret = -1;
            }
            if (*priority > 255){
                log(Warning) << "Forcing priority ("<<*priority<<") of thread to 255." <<endlog();
                *priority = 255;
                ret = -1;
            }
            return ret;
        }

        INTERNAL_QUAL int rtos_task_set_priority(RTOS_TASK * mytask, int priority)
        {
            int rv;

            if (mytask->rtaitask == 0)
                return -1;

            // returns the *old* priority.
            rv = rt_change_prio( mytask->rtaitask, priority);
            if (rv == mytask->priority) {
                mytask->priority = priority;
                return 0;
            }
            return -1;
        }

        INTERNAL_QUAL const char * rtos_task_get_name(const RTOS_TASK* t)
        {
            return t->name;
        }

        INTERNAL_QUAL void rtos_task_set_name(RTOS_TASK* t, const char * name)
	{
            if(t->name) free(t->name);
	    t->name = strndup(name, 255);
	}

        INTERNAL_QUAL int rtos_task_get_priority(const RTOS_TASK *t)
        {
            return t->priority;
        }

	INTERNAL_QUAL int rtos_task_set_cpu_affinity(RTOS_TASK * task, unsigned cpu_affinity)
	{
        return -1;
        }

	INTERNAL_QUAL unsigned rtos_task_get_cpu_affinity(const RTOS_TASK *task)
	{
        return ~0;
        }
    }
}
#undef INTERNAL_QUAL
#endif
