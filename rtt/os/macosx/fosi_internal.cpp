/***************************************************************************
    copyright            : (C) 2008 Klaas Gadeyne
    email                : firstname dot lastname at gmail dot com

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
#include "../Mutex.hpp"

#define INTERNAL_QUAL

namespace RTT
{ namespace os {

	INTERNAL_QUAL int rtos_task_create_main(RTOS_TASK* main_task)
	{
        const char* name = "main";
	    main_task->name = strcpy( (char*)malloc( (strlen(name) + 1) * sizeof(char)), name);
        main_task->thread = pthread_self();
	    pthread_attr_init( &(main_task->attr) );
	    struct sched_param sp;
	    sp.sched_priority=0;
	    // Set priority
	    // fixme check return value and bail out if necessary
	    pthread_attr_setschedparam(&(main_task->attr), &sp);
        main_task->priority = sp.sched_priority;
        main_task->wait_policy = ORO_WAIT_ABS;
	    return 0;
	}

	INTERNAL_QUAL int rtos_task_delete_main(RTOS_TASK* main_task)
	{
        pthread_attr_destroy( &(main_task->attr) );
        free( main_task->name );
	    return 0;
	}

	INTERNAL_QUAL int rtos_task_create(RTOS_TASK* task,
					   int priority,
					   unsigned cpu_affinity,
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
            task->wait_policy = ORO_WAIT_ABS;

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

            struct sched_param sp;
            sp.sched_priority=priority;
            // Set priority
            if ( (rv = pthread_attr_setschedparam(&(task->attr), &sp)) != 0 ){
                return rv;
            }
	    rv = pthread_create(&(task->thread), &(task->attr), start_routine, obj);
            log(Debug) <<"Created Posix thread "<< task->thread <<endlog();
            return rv;
	}

	INTERNAL_QUAL void rtos_task_yield(RTOS_TASK* t)
	{
            int ret = sched_yield();
            if ( ret != 0)
            perror("rtos_task_yield");
	}

	INTERNAL_QUAL int rtos_task_is_self(const RTOS_TASK* task) {
	    pthread_t self = pthread_self();
	    if ( pthread_equal(self, task->thread) == 0 ) // zero means false.
	        return 0;
	    return 1;
	}


        INTERNAL_QUAL int rtos_task_set_scheduler(RTOS_TASK* task, int sched_type)
        {
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

        INTERNAL_QUAL int rtos_task_get_scheduler(const RTOS_TASK* task)
        {
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
	    mytask->periodMark = rtos_get_time_ns() + nanosecs;
	}

	INTERNAL_QUAL void rtos_task_set_period( RTOS_TASK* mytask, NANO_TIME nanosecs )
	{
	    mytask->period = nanosecs;
	    mytask->periodMark = rtos_get_time_ns() + nanosecs;
	}

  INTERNAL_QUAL void rtos_task_set_wait_period_policy( RTOS_TASK* task, int policy )
  {
    task->wait_policy = policy;
  }

	INTERNAL_QUAL int rtos_task_wait_period( RTOS_TASK* task )
	{
	    if ( task->period == 0 )
		return 0;

	    //rtos_printf("Time is %lld nsec, Mark is %lld nsec.\n",rtos_get_time_ns(), task->periodMark );
	    // CALCULATE in nsecs
	    NANO_TIME timeRemaining = task->periodMark - rtos_get_time_ns();

        // next wake-up time :
      if (task->wait_policy == ORO_WAIT_ABS)
      {
        task->periodMark += task->period;
      }
      else
      {
        task->periodMark = rtos_get_time_ns() + task->period;
      }

	    if ( timeRemaining > 0 ) {
	        //rtos_printf("Waiting for %lld nsec\n",timeRemaining);
	        TIME_SPEC ts( ticks2timespec( timeRemaining ) );
	        rtos_nanosleep( &ts , NULL );
	        return 0;
	    }

	    return -1;
	}

	INTERNAL_QUAL void rtos_task_delete(RTOS_TASK* mytask)
	{
            pthread_join( mytask->thread, 0);
            pthread_attr_destroy( &(mytask->attr) );
	    free(mytask->name);
	}

        INTERNAL_QUAL int rtos_task_check_scheduler(int* scheduler)
        {
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

            if (*priority < 0){
                log(Warning) << "Forcing priority ("<<*priority<<") of thread to 0." <<endlog();
                *priority = 0;
                ret = -1;
            }
            if (*priority > 63){
                log(Warning) << "Forcing priority ("<<*priority<<") of thread to 63." <<endlog();
                *priority = 63;
                ret = -1;
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

	INTERNAL_QUAL unsigned int rtos_task_get_pid(const RTOS_TASK* task)
	{
		return 0;
	}

	INTERNAL_QUAL int rtos_task_set_cpu_affinity(RTOS_TASK * task, unsigned cpu_affinity)
	{
        return -1;
        }

	INTERNAL_QUAL unsigned rtos_task_get_cpu_affinity(const RTOS_TASK *task)
	{
        return ~0;
        }

	INTERNAL_QUAL const char * rtos_task_get_name(const RTOS_TASK* task)
	{
	    return task->name;
	}

        INTERNAL_QUAL void rtos_task_set_name(RTOS_TASK* task, const char * name)
	{
            if(main_task->name) free(main_task->name);
	    main_task->name = strndup(name, 255);
	}

    }
}

// opaque type to hide C object from C code
typedef struct rt_mutex_impl_t
{
    RTT::os::Mutex mutex;
};

int rtos_mutex_init(rt_mutex_t* m)
{
    assert(m);
    *m = new rt_mutex_impl_t;   // non-realtime
    return (0 != (*m));
}

int rtos_mutex_destroy(rt_mutex_t* m )
{
    assert(m);
    assert(*m);
    delete (*m);               // non-realtime
    (*m) = 0;
    return 0;
}

int rtos_mutex_lock( rt_mutex_t* m)
{
    assert(m);
    assert(*m);
    (*m)->mutex.lock();
    return 0;
}

int rtos_mutex_unlock( rt_mutex_t* m)
{
    assert(m);
    assert(*m);
    (*m)->mutex.unlock();
    return 0;
}

#undef INTERNAL_QUAL
