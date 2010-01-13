/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:51 CEST 2005  fosi_internal.hpp

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


#include "../ThreadInterface.hpp"
#include "fosi.h"
#include "../fosi_internal_interface.hpp"
#include "../../Logger.hpp"
#include <cassert>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <cstdlib>
using namespace std;

#define INTERNAL_QUAL

namespace RTT
{ namespace os {

	INTERNAL_QUAL int rtos_task_create_main(RTOS_TASK* main_task)
	{
        const char* name = "main";
	    main_task->name = strcpy( (char*)malloc( (strlen(name) + 1) * sizeof(char)), name);
        main_task->thread = 0;
	    pthread_attr_init( &(main_task->attr) );
	    struct sched_param sp;
	    sp.sched_priority=0;
	    // Set priority
	    // fixme check return value and bail out if necessary
	    pthread_attr_setschedparam(&(main_task->attr), &sp);
        main_task->priority = sp.sched_priority;
	    return 0;
	}

	INTERNAL_QUAL int rtos_task_delete_main(RTOS_TASK* main_task)
	{
        pthread_attr_destroy( &(main_task->attr) );
        free(main_task->name);
	    return 0;
	}

	INTERNAL_QUAL int rtos_task_create(RTOS_TASK* task,
					   int priority,
					   const char * name,
					   int sched_type,
					   size_t stack_size,
					   void * (*start_routine)(void *),
					   ThreadInterface* obj)
	{
        int rv; // return value
        rtos_task_check_priority( &sched_type, &priority );
        // Save priority internally, since the pthread_attr* calls are broken !
        // we will pick it up later in rtos_task_set_scheduler().
        task->priority = priority;

	    // Set name
	    if ( strlen(name) == 0 )
            name = "Thread";
	    task->name = strcpy( (char*)malloc( (strlen(name) + 1) * sizeof(char)), name);

	    if ( (rv = pthread_attr_init(&(task->attr))) != 0 ){
            return rv;
	    }
	    // Set scheduler type (_before_ assigning priorities!)
	    if ( (rv = pthread_attr_setschedpolicy(&(task->attr), sched_type)) != 0){
            return rv;
	    }
        pthread_attr_getschedpolicy(&(task->attr), &rv );
        assert( rv == sched_type );
	    /* SCHED_OTHER tasks are always assigned static priority 0, see
	       man sched_setscheduler
	    */
        struct sched_param sp;
	    if (sched_type != SCHED_OTHER){
            sp.sched_priority=priority;
            // Set priority
            if ( (rv = pthread_attr_setschedparam(&(task->attr), &sp)) != 0 ){
                return rv;
            }
	    }
	    rv = pthread_create(&(task->thread), &(task->attr),
                              start_routine, obj);
        log(Debug) <<"Created Posix thread "<< task->thread <<endlog();
        return rv;
	}

	INTERNAL_QUAL void rtos_task_yield(RTOS_TASK* t) {
#if 0
        //under plain gnulinux, sched_yield may have little influence, so sleep
        // to force rescheduling of other threads.
	    NANO_TIME timeRemaining = 1000; // 1ms
		TIME_SPEC ts( ticks2timespec( timeRemaining ) );
		rtos_nanosleep( &ts , NULL );
#else
        int ret = sched_yield();
        if ( ret != 0)
            perror("rtos_task_yield");
#endif
	}

	INTERNAL_QUAL int rtos_task_is_self(const RTOS_TASK* task) {
	    pthread_t self = pthread_self();
	    if ( pthread_equal(self, task->thread) == 0 ) // zero means false.
	        return 0;
	    return 1;
	}

    INTERNAL_QUAL int rtos_task_set_scheduler(RTOS_TASK* task, int sched_type) {
        int policy = -1;
        struct sched_param param;
        // first check the argument
        if ( task && task->thread != 0 && rtos_task_check_scheduler( &sched_type) == -1 )
            return -1;
        // if sched_type is different, the priority must change as well.
        if (pthread_getschedparam(task->thread, &policy, &param) == 0) {
            // now update the priority
            param.sched_priority = task->priority;
            rtos_task_check_priority( &sched_type, &param.sched_priority );
            // write new policy:
            return pthread_setschedparam( task->thread, sched_type, &param);
        }
        return -1;
    }

    INTERNAL_QUAL int rtos_task_get_scheduler(const RTOS_TASK* task) {
        int policy = -1;
        struct sched_param param;
        // first retrieve thread scheduling parameters:
        if ( task && task->thread != 0 && pthread_getschedparam(task->thread, &policy, &param) == 0)
            return policy;
        return -1;
    }

	INTERNAL_QUAL void rtos_task_make_periodic(RTOS_TASK* mytask, NANO_TIME nanosecs )
	{
	    // set period
	    mytask->period = nanosecs;
	    // set next wake-up time.
	    mytask->periodMark = ticks2timespec( nano2ticks( rtos_get_time_ns() + nanosecs ) );
	}

	INTERNAL_QUAL void rtos_task_set_period( RTOS_TASK* mytask, NANO_TIME nanosecs )
	{
        rtos_task_make_periodic(mytask, nanosecs);
	}

	INTERNAL_QUAL int rtos_task_wait_period( RTOS_TASK* task )
	{
	    if ( task->period == 0 )
            return 0;

        // record this to detect overrun.
	    NANO_TIME now = rtos_get_time_ns();
	    NANO_TIME wake= task->periodMark.tv_sec * 1000000000LL + task->periodMark.tv_nsec;

        // inspired by nanosleep man page for this construct:
        while ( clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &(task->periodMark), NULL) != 0 && errno == EINTR ) {
            errno = 0;
        }

        // program next period:
        // 1. convert period to timespec
        TIME_SPEC ts = ticks2timespec( nano2ticks( task->period) );
        // 2. Add ts to periodMark (danger: tn guards for overflows!)
        NANO_TIME tn = (task->periodMark.tv_nsec + ts.tv_nsec);
        task->periodMark.tv_nsec = tn % 1000000000LL;
        task->periodMark.tv_sec += ts.tv_sec + tn / 1000000000LL;

	    return now > wake ? -1 : 0;
	}

	INTERNAL_QUAL void rtos_task_delete(RTOS_TASK* mytask) {
        pthread_join( mytask->thread, 0);
        pthread_attr_destroy( &(mytask->attr) );
	    free(mytask->name);
	}

    INTERNAL_QUAL int rtos_task_check_scheduler(int* scheduler)
    {
        if (*scheduler != SCHED_OTHER && geteuid() != 0 ) {
            // they're not root and they want a real-time priority, which _might_
            // be acceptable if they're using pam_limits and have set the rtprio ulimit
            // (see "/etc/security/limits.conf" and "ulimit -a")
            struct rlimit	r;
            if ((0 != getrlimit(RLIMIT_RTPRIO, &r)) || (0 == r.rlim_cur))
            {
                log(Warning) << "Lowering scheduler type to SCHED_OTHER for non-privileged users.." <<endlog();
                *scheduler = SCHED_OTHER;
                return -1;
            }
        }
        if (*scheduler != SCHED_OTHER && *scheduler != SCHED_FIFO && *scheduler != SCHED_RR ) {
            log(Error) << "Unknown scheduler type." <<endlog();
            *scheduler = SCHED_OTHER;
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
        if (*scheduler == SCHED_OTHER) {
            if ( *priority != 0 ) {
                log(Warning) << "Forcing priority ("<<*priority<<") of thread with SCHED_OTHER policy to 0." <<endlog();
                *priority = 0;
                ret = -1;
            }
        } else {
            // SCHED_FIFO/SCHED_RR:
            if (*priority <= 0){
                log(Warning) << "Forcing priority ("<<*priority<<") of thread with !SCHED_OTHER policy to 1." <<endlog();
                *priority = 1;
                ret = -1;
            }
            if (*priority > 99){
                log(Warning) << "Forcing priority ("<<*priority<<") of thread with !SCHED_OTHER policy to 99." <<endlog();
                *priority = 99;
                ret = -1;
            }
            // and limit them according to pam_Limits (only if not root)
            if ( geteuid() != 0 )
            {
                struct rlimit	r;
                if (0 == getrlimit(RLIMIT_RTPRIO, &r))
                {
                    if (*priority > (int)r.rlim_cur)
                    {
                        log(Warning) << "Forcing priority ("<<*priority<<") of thread with !SCHED_OTHER policy to the pam_limit of " << r.rlim_cur <<endlog();
                        *priority = r.rlim_cur;
                        ret = -1;
                    }
                }
                else
                {
                    // this should not be possible, but do something intelligent
                    *priority = 1;
                    ret = -1;
                }
            }
        }
        return ret;
    }

	INTERNAL_QUAL int rtos_task_set_priority(RTOS_TASK * task, int priority)
	{
        int policy = 0;
        struct sched_param param;
        // first retrieve thread scheduling parameters:
        if( task && task->thread != 0 && pthread_getschedparam(task->thread, &policy, &param) == 0) {
            if ( rtos_task_check_priority( &policy, &priority ) != 0 )
                return -1;
            param.sched_priority = priority;
            task->priority = priority; // store for set_scheduler
            // write new policy:
            return pthread_setschedparam( task->thread, policy, &param);
        }
        return -1;
    }

	INTERNAL_QUAL int rtos_task_get_priority(const RTOS_TASK *task)
	{
        // if sched_other, return the 'virtual' priority
        int policy = 0;
        struct sched_param param;
        // first retrieve thread scheduling parameters:
        if ( task == 0 )
            return -1;
        if ( task->thread == 0 || pthread_getschedparam(task->thread, &policy, &param) != 0)
            return task->priority;
        return param.sched_priority;
	}

	INTERNAL_QUAL const char * rtos_task_get_name(const RTOS_TASK* task)
	{
	    return task->name;
	}

    }
}
#undef INTERNAL_QUAL
