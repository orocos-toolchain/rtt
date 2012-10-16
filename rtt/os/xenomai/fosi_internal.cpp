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



#define OROBLD_OS_INTERNAL
#include "os/ThreadInterface.hpp"
#include "fosi.h"
#include "../fosi_internal_interface.hpp"
#include <cassert>
#define INTERNAL_QUAL

#include "../../Logger.hpp"
#include <signal.h>
#include <execinfo.h>

extern "C"
void warn_upon_switch(int sig __attribute__((unused)))
{
    void *bt[32];
    int nentries;

    /* Dump a backtrace to standard error of the frame which caused the switch to
       secondary mode: */
    nentries = backtrace(bt,sizeof(bt) / sizeof(bt[0]));
    backtrace_symbols_fd(bt,nentries,fileno(stderr));
}


namespace RTT
{
    namespace os {

        INTERNAL_QUAL int rtos_task_create_main(RTOS_TASK* main)
        {
            // first check if root (or if have sufficient privileges)
            if ( geteuid() != 0 ) {
#if ((CONFIG_XENO_VERSION_MAJOR*1000)+(CONFIG_XENO_VERSION_MINOR*100)+CONFIG_XENO_REVISION_LEVEL) >= 2302
                printf( "WARNING: You are not root. This program *may* require that you are root.\n");
                // \todo verify have sufficient privileges
#else
                printf( "You are not root. This program requires that you are root.\n");
                exit(1);
#endif
            }

            // locking of all memory for this process
            int rv = mlockall(MCL_CURRENT | MCL_FUTURE);
            if ( rv != 0 ) {
                perror( "rtos_task_create_main: Could not lock memory using mlockall" ); // Logger unavailable.
                exit(1);
            }

            struct sched_param param;
            // we set the MT to the highest sched priority to allow the console
            // to interrupt a loose running thread.
            param.sched_priority = sched_get_priority_max(ORO_SCHED_OTHER);
            if (param.sched_priority != -1 )
                sched_setscheduler( 0, ORO_SCHED_OTHER, &param);

            const char* mt_name = "MainThread";
            main->sched_type = SCHED_XENOMAI_SOFT; // default for MainThread
            main->name = strncpy( (char*)malloc( (strlen(mt_name)+1)*sizeof(char) ), mt_name, strlen(mt_name)+1 );

            int ret = -1;
            while( ret != 0) {
                // name, priority, mode
                if ( (ret = rt_task_shadow( &(main->xenotask),mt_name, 0, 0)) != 0 ) {
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
                    // uncaught error: abort
                    printf( "Can not rt_task_create() MainThread: Error %d.\n",ret);
                    exit(1);
                }
            }
            // We are a xeno thread now:
            // Only use Logger after this point (i.e. when rt_task_shadow was succesful).
            if ( mt_name == 0) {
                log(Warning) << "'MainThread' name was already in use. Registered empty name with Xenomai.\n" <<endlog();
            }

            // main is created in main thread.
            main->xenoptr = rt_task_self();

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
            log(Info) << "Installing SIGXCPU handler." <<endlog();
            //signal(SIGXCPU, warn_upon_switch);
            struct sigaction sa;
            sa.sa_handler = warn_upon_switch;
            sigemptyset( &sa.sa_mask );
            sa.sa_flags = 0;
            sigaction(SIGXCPU, &sa, 0);

            Logger::log() << Logger::Debug << "Xenomai Timer and Main Task Created" << Logger::endl;
            return 0;
        }

        INTERNAL_QUAL int rtos_task_delete_main(RTOS_TASK* main_task)
        {
            //rt_task_delete( &(main_task->xenotask) );
            free (main_task->name);
            main_task->name = 0;
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
            assert( task->xenoptr );

            // call user function
            ((XenoCookie*)cookie)->wrapper( ((XenoCookie*)cookie)->data );
            free(cookie);
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
            rtos_task_check_priority(&sched_type, &priority);
            XenoCookie* xcookie = (XenoCookie*)malloc( sizeof(XenoCookie) );
            xcookie->data = obj;
            xcookie->wrapper = start_routine;
            if ( name == 0 || strlen(name) == 0)
                name = "XenoThread";
            task->name = strncpy( (char*)malloc( (strlen(name)+1)*sizeof(char) ), name, strlen(name)+1 );
            task->sched_type = sched_type; // User requested scheduler.
            int rv;

            unsigned int aff = 0;
            if ( cpu_affinity != 0 ) {
                // calculate affinity:
                for(unsigned i = 0; i < 8*sizeof(cpu_affinity); i++) {
                    if(cpu_affinity & (1 << i)) { 
                        // RTHAL_NR_CPUS is defined in the kernel, not in user space. So we just limit up to 7, until Xenomai allows us to get the maximum.
                        if ( i > 7 ) {
                            const unsigned int all_cpus = ~0;
                            if ( cpu_affinity != all_cpus ) // suppress this warning when ~0 is provided
                                log(Warning) << "rtos_task_create: ignoring cpu_affinity for "<< name << " on CPU " << i << " since it's larger than RTHAL_NR_CPUS - 1 (="<< 7 <<")"<<endlog();
                        } else {
                            aff |= T_CPU(i); 
                        }
                    }
                }
            }
            
            if (stack_size == 0) {
                log(Debug) << "Raizing default stack size to 128kb for Xenomai threads in Orocos." <<endlog();
                stack_size = 128000;
            }

            // task, name, stack, priority, mode, fun, arg
            // UGLY, how can I check in Xenomai that a name is in use before calling rt_task_spawn ???
            rv = rt_task_spawn(&(task->xenotask), name, stack_size, priority, T_JOINABLE | (aff & T_CPUMASK), rtos_xeno_thread_wrapper, xcookie);
            if ( rv == -EEXIST ) {
                free( task->name );
                task->name = strncpy( (char*)malloc( (strlen(name)+2)*sizeof(char) ), name, strlen(name)+1 );
                task->name[ strlen(name) ] = '0';
                task->name[ strlen(name)+1 ] = 0;
                while ( rv == -EEXIST &&  task->name[ strlen(name) ] != '9') {
                    task->name[ strlen(name) ] += 1;
                    rv = rt_task_spawn(&(task->xenotask), task->name, stack_size, priority, T_JOINABLE | (aff & T_CPUMASK), rtos_xeno_thread_wrapper, xcookie);
                }
            }
            if ( rv == -EEXIST ) {
                log(Warning) << name << ": an object with that name is already existing in Xenomai." << endlog();
                rv = rt_task_spawn(&(task->xenotask), 0, stack_size, priority, T_JOINABLE | (aff & T_CPUMASK), rtos_xeno_thread_wrapper, xcookie);
            }
            if ( rv != 0) {
                log(Error) << name << " : CANNOT INIT Xeno TASK " << task->name <<" error code: "<< rv << endlog();
                return rv;
            }

            rt_task_yield();
            return 0;
        }

        INTERNAL_QUAL void rtos_task_yield(RTOS_TASK*) {
            rt_task_yield();
        }

        INTERNAL_QUAL int rtos_task_is_self(const RTOS_TASK* task) {
            RT_TASK* self = rt_task_self();
            if (self == 0 || task == 0)
                return -1; // non-xeno thread. We could try to compare pthreads like in gnulinux ?
#if ((CONFIG_XENO_VERSION_MAJOR*1000)+(CONFIG_XENO_VERSION_MINOR*100)+CONFIG_XENO_REVISION_LEVEL) >= 2500
            if ( rt_task_same( self, task->xenoptr ) != 0 )
                return 1;
#else
            // older versions:
            if ( self == task->xenoptr ) // xenoptr is also set by rt_task_self() during construction.
                return 1;
#endif
            return 0;
        }


    INTERNAL_QUAL int rtos_task_check_scheduler(int* scheduler)
    {
        if (*scheduler != SCHED_XENOMAI_HARD && *scheduler != SCHED_XENOMAI_SOFT ) {
            log(Error) << "Unknown scheduler type." <<endlog();
            *scheduler = SCHED_XENOMAI_SOFT;
            return -1;
        }
        return 0;
    }

	INTERNAL_QUAL int rtos_task_check_priority(int* scheduler, int* priority)
    {
        int ret = 0;
        // check scheduler first.
        ret = rtos_task_check_scheduler(scheduler);

        // correct priority
        // Hard & Soft:
#if ((CONFIG_XENO_VERSION_MAJOR*10000)+(CONFIG_XENO_VERSION_MINOR*100)+CONFIG_XENO_REVISION_LEVEL) >= 20500
        const int minprio = 0;
#else
        const int minprio = 1;
#endif
        if (*priority < minprio){
            log(Warning) << "Forcing priority ("<<*priority<<") of thread to " << minprio <<"." <<endlog();
            *priority = minprio;
            ret = -1;
        }
        if (*priority > 99){
            log(Warning) << "Forcing priority ("<<*priority<<") of thread to 99." <<endlog();
            *priority = 99;
            ret = -1;
        }
        return ret;
    }


        // we could implement here the interrupt shield logic.
        INTERNAL_QUAL int rtos_task_set_scheduler(RTOS_TASK* t, int sched_type) {
            // xenoptr was initialised from the thread wrapper.
            if (t->xenoptr != rt_task_self() ) {
                return -1;
            }

            if ( rtos_task_check_scheduler( &sched_type ) == -1)
                return -1;

#if ((CONFIG_XENO_VERSION_MAJOR*1000)+(CONFIG_XENO_VERSION_MINOR*100)+CONFIG_XENO_REVISION_LEVEL) < 2600
            if (sched_type == SCHED_XENOMAI_HARD) {
                if ( rt_task_set_mode( 0, T_PRIMARY, 0 ) == 0 ) {
                    t->sched_type = SCHED_XENOMAI_HARD;
                    return 0;
                } else {
                    return -1;
                }
            } else {
                if ( sched_type == SCHED_XENOMAI_SOFT) {
                    // This mode setting is only temporary. See rtos_task_wait_period() as well !
                    if (rt_task_set_mode( T_PRIMARY, 0, 0 ) == 0 ) {
                        t->sched_type = SCHED_XENOMAI_SOFT;
                        return 0;
                    } else {
                        return -1;
                    }
                }
            }
            assert(false);
            return -1;
#else
	    t->sched_type = sched_type;
	    return 0;
#endif
        }

        INTERNAL_QUAL int rtos_task_get_scheduler(const RTOS_TASK* mytask) {
#if 0
            // WORK AROUND constness: (need non-const mytask)
            RT_TASK* tt = mytask->xenoptr;
            RT_TASK_INFO info;
            if ( tt )
                if ( rt_task_inquire( tt, &info) == 0 )
                    if ( info.status & XNRELAX )
                        return SCHED_XENOMAI_SOFT;
                    else
                        return SCHED_XENOMAI_HARD;
            return -1;
#else
            return mytask->sched_type;
#endif
        }

        INTERNAL_QUAL void rtos_task_make_periodic(RTOS_TASK* mytask, NANO_TIME nanosecs )
        {
            if (nanosecs == 0) {
                rt_task_set_periodic( &(mytask->xenotask), TM_NOW, TM_INFINITE);
            }
            else {
                rt_task_set_periodic( &(mytask->xenotask), TM_NOW, rt_timer_ns2ticks(nanosecs) );
            }
        }

        INTERNAL_QUAL void rtos_task_set_period( RTOS_TASK* mytask, NANO_TIME nanosecs )
        {
            rtos_task_make_periodic( mytask, nanosecs);
            //rt_task_set_period(&(mytask->xenotask), rt_timer_ns2ticks( nanosecs ));
        }

        INTERNAL_QUAL void rtos_task_set_wait_period_policy( RTOS_TASK* task, int policy )
        {
          // Do nothing
        }

        INTERNAL_QUAL int rtos_task_wait_period( RTOS_TASK* mytask )
        {
            // detect overrun.
#if CONFIG_XENO_VERSION_MAJOR == 2 && CONFIG_XENO_VERSION_MINOR == 0
            if ( rt_task_wait_period() == -ETIMEDOUT)
                return 1;
#else // 2.1, 2.2, 2.3, 2.4,...
            long unsigned int overrun = 0;
            rt_task_wait_period(&overrun);

#if ((CONFIG_XENO_VERSION_MAJOR*1000)+(CONFIG_XENO_VERSION_MINOR*100)+CONFIG_XENO_REVISION_LEVEL) < 2600
            // When running soft, switch to secondary mode:
            if ( mytask->sched_type == SCHED_XENOMAI_SOFT )
                rt_task_set_mode(T_PRIMARY, 0, 0 );
#endif

            if ( overrun != 0)
                return 1;
#endif
            return 0;
        }

        INTERNAL_QUAL int rtos_task_set_cpu_affinity(RTOS_TASK * task, unsigned cpu_affinity)
        {
            log(Error) << "rtos_task_set_cpu_affinity: Xenomai tasks don't allow to migrate to another CPU once created." << endlog();
            return -1;
        }

        INTERNAL_QUAL unsigned rtos_task_get_cpu_affinity(const RTOS_TASK *task)
        {
            return 0;
        }

        INTERNAL_QUAL const char* rtos_task_get_name(const RTOS_TASK* mytask) {
            return mytask->name;
        }

        INTERNAL_QUAL void rtos_task_set_name(RTOS_TASK* task, const char * name)
	{
            return;
	}

    	INTERNAL_QUAL unsigned int rtos_task_get_pid(const RTOS_TASK* task)
    	{
    		return 0;
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
            if ( rt_task_join(&(mytask->xenotask)) != 0 ) {
                log(Error) << "Failed to join with thread " << mytask->name << endlog();
            }
            rt_task_delete(&(mytask->xenotask));
            free (mytask->name);
            mytask->name = 0;
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
}
#undef INTERNAL_QUAL

