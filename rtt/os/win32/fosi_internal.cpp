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
#include <iostream>
#include <cstdlib>
using namespace std;




#define INTERNAL_QUAL

namespace RTT
{ namespace os {

void ErrorHandler(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code.

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message.

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR));
    /*StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
        */
    MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK);

	// Free error-handling buffer allocations.

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}


    struct ThreadWrapperData {
          void* (*realThread)(void*);
          void* realData;
    };

    DWORD WINAPI ThreadWrapper(void* threadData)
    {
        ThreadWrapperData* threadWrapperData =
                (ThreadWrapperData*) threadData;

        void* (*realThread)(void*) = threadWrapperData->realThread;
        void* realData = threadWrapperData->realData;
        delete threadWrapperData;

        return (DWORD) realThread(realData);
    }


    //TODO
	INTERNAL_QUAL int rtos_task_create_main(RTOS_TASK* main_task)
	{
        const char* name = "main";
        main_task->wait_policy = ORO_WAIT_ABS;
	    main_task->name = strcpy( (char*)malloc( (strlen(name) + 1) * sizeof(char)), name);
        main_task->threadId = GetCurrentThreadId();
        main_task->handle = 0;
	    //pthread_attr_init( &(main_task->attr) );
	    //struct sched_param sp;
	    //sp.sched_priority=0;
	    // Set priority
	    // fixme check return value and bail out if necessary
	    //pthread_attr_setschedparam(&(main_task->attr), &sp);
        //main_task->priority = sp.sched_priority;
	    return 0;
	}

	INTERNAL_QUAL int rtos_task_delete_main(RTOS_TASK* main_task)
	{
        //pthread_attr_destroy( &(main_task->attr) );
		// printf("rtos_task_delete_main");
		free(main_task->name);
                main_task->name = 0;
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
        //int rv; // return value
        // TODO implement scheduler by using CreateProcess
        // Set name
        task->wait_policy = ORO_WAIT_ABS;
        if (name == 0 || strlen(name) == 0)
            name = "Thread";
        task->name = strncpy((char*) malloc((strlen(name) + 1)
                                           * sizeof(char)), name, strlen(name) + 1);

        ThreadWrapperData* data = new ThreadWrapperData;
        data->realThread = start_routine;
        data->realData = obj;

        task->handle = CreateThread(NULL, 2*1000*1000, ThreadWrapper, data, 0,
                                    &task->threadId);

        if (task->handle == NULL)
        {
           ErrorHandler(TEXT("CreateThread"));
           ExitProcess(3);
        }

        SetThreadPriority(task->handle, priority);

        return 0;
    }

    INTERNAL_QUAL void rtos_task_yield(RTOS_TASK* mytask) {
        // printf("T:%u -> ", (unsigned int) mytask);
        // printf(" yield \n ");
        Sleep(0);
    }

    INTERNAL_QUAL int rtos_task_is_self(const RTOS_TASK* task) {
        DWORD self = GetCurrentThreadId();
        if ( self == task->threadId )
            return 1;
        return 0;
    }

    INTERNAL_QUAL int rtos_task_set_scheduler(RTOS_TASK* task, int sched_type) {

    	//int policy = -1;
        //struct sched_param param;
        // first check the argument
        if ( task && task->handle != NULL && rtos_task_check_scheduler( &sched_type) == -1 )
            return -1;
        // if sched_type is different, the priority must change as well.
        //if (pthread_getschedparam(task->thread, &policy, &param) == 0) {
            // now update the priority
        //    param.sched_priority = task->priority;
        //    rtos_task_check_priority( &sched_type, &param.sched_priority );
            // write new policy:
        //    return pthread_setschedparam( task->thread, sched_type, &param);
        //}
        // TODO: change the scheduling of the thread
        task->sched_type = sched_type;
        return 0;
    }

    INTERNAL_QUAL int rtos_task_get_scheduler(const RTOS_TASK* task) {
    	return task->sched_type;
    }

	INTERNAL_QUAL unsigned int rtos_task_get_pid(const RTOS_TASK* task)
	{
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

    INTERNAL_QUAL NANO_TIME rtos_task_get_period(const RTOS_TASK* t) {
      return t->period;
    }

    INTERNAL_QUAL void rtos_task_set_wait_period_policy( RTOS_TASK* task, int policy )
    {
      task->wait_policy = policy;
    }

    INTERNAL_QUAL int rtos_task_wait_period( RTOS_TASK* task )
    {
      if ( task->period == 0 )
    	  return 0;

      // rtos_printf("Time is %lld nsec, Mark is %lld nsec.\n",rtos_get_time_ns(), task->periodMark );
      // CALCULATE in nsecs
      NANO_TIME timeRemaining = task->periodMark - rtos_get_time_ns();

      if ( timeRemaining > 0 ) {
 		TIME_SPEC ts( ticks2timespec( timeRemaining ) );
		rtos_nanosleep( &ts , NULL );
      }
      //             else
      //                 rtos_printf( "GNULinux task did not get deadline !\n" );

      // next wake-up time :
      if (task->wait_policy == ORO_WAIT_ABS)
        task->periodMark += task->period;
      else
        task->periodMark = rtos_get_time_ns() + task->period;

      return 0;
    }

    INTERNAL_QUAL void rtos_task_delete(RTOS_TASK* mytask) {
      // printf("T:%u -> ", (unsigned int) mytask);
      //printf(" rtos_task_delete ");
      //DWORD exitCode;
      //TerminateThread(mytask->handle, exitCode);
      WaitForSingleObject( mytask->handle, INFINITE );

      CloseHandle(mytask->handle);
      free(mytask->name);
      mytask->name = NULL;
      mytask->handle = NULL;

      // printf(" success \n ");
    };

    INTERNAL_QUAL int rtos_task_check_scheduler(int* scheduler)
    {

        /*if (*scheduler != SCHED_OTHER && geteuid() != 0 ) {
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
        }*/
        return 0;
    }

	INTERNAL_QUAL int rtos_task_check_priority(int* scheduler, int* priority)
    {
        int ret = 0;
        // TODO: check scheduler first.
        //ret = rtos_task_check_scheduler(scheduler);

        if (*priority <= -15){
            log(Warning) << "Forcing priority ("<<*priority<<") of thread with !SCHED_OTHER policy to -15." <<endlog();
            *priority = -15;
            ret = -1;
        }
        if (*priority > 15){
            log(Warning) << "Forcing priority ("<<*priority<<") of thread with !SCHED_OTHER policy to 15." <<endlog();
            *priority = 15;
            ret = -1;
        }
        return ret;
    }

    INTERNAL_QUAL int rtos_task_set_priority(RTOS_TASK * task, int priority)
    {
      // FIXME, only works on the current task
      // init the scheduler. The rt_task_initschmod code is broken, so we do it ourselves.
        SetThreadPriority(task->handle, priority);
        return 0;
    }

    INTERNAL_QUAL int rtos_task_get_priority(const RTOS_TASK *t)
    {
        return GetThreadPriority(t->handle);
    }

    INTERNAL_QUAL int rtos_task_set_cpu_affinity(RTOS_TASK * task, unsigned cpu_affinity)
    {
    return -1;
    }

    INTERNAL_QUAL unsigned rtos_task_get_cpu_affinity(const RTOS_TASK *task)
    {
    return ~0;
    }

    INTERNAL_QUAL const char * rtos_task_get_name(const RTOS_TASK* t)
    {
    	/* printf("Get Name: ");
    	if( t->name!=NULL ){
    		printf("%s",t->name);
    	} else {
    		printf("ERRROR TASK HAS ALLREADY BEEN DELETED");
    	}
    	*/

    	return t->name;
    }

    INTERNAL_QUAL void rtos_task_set_name(RTOS_TASK* task, const char * name)
    {
        if(main_task->name) free(main_task->name);
        main_task->name = strndup(name, 255);
    }

    }
}
#undef INTERNAL_QUAL
