/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  fosi_internal_interface.h

                        fosi_internal_interface.h -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

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



/**
 * @file fosi_internal_interface.hpp
 * This file has all the (periodic) thread specific interfaces.
 * If a function does not apply to your OS, it may be left empty.
 *
 */

#ifndef OS_FOSI_INTERNAL_INTERFACE_HPP
#define OS_FOSI_INTERNAL_INTERFACE_HPP

#include "ThreadInterface.hpp"
#include "fosi.h"

namespace RTT {
    namespace os {
            /**
             * Initialise the main thread. Anything necessary to 
             * setup the real-time environment on the current RTOS
             * may be done here. This is the first function that
             * the RTT will call in the FOSI layer, when the MainThread
             * object is constructed.
             * Typically, the main thread is assigned a priority,
             * put in a scheduler and memory is locked into RAM.
             */
            int rtos_task_create_main(RTOS_TASK* main_task);

            /**
             * Cleanup the main thread.
             * This undoes the actions taken by rtos_task_create_main().
             * This is the last function that the RTT will call in the FOSI
             * layer, when the MainThread object is destructed.
             */
            int rtos_task_delete_main(RTOS_TASK* main_task);

            /**
             * Create a thread.
             * Spawn a new thread with the given arguments. When rtos_task_create returns
             * successfully, the \a task structure must be initialised fully and be valid.
             *
             * @param task An uninitialised, by the user allocated struct which will be
             * used to store the task data in.
             * @param priority The priority of the thread, to be interpreted by the underlying RTOS.
             * @param cpu_affinity The cpu affinity of the thread (@see rtos_task_set_cpu_affinity).
             * @param name The name for the thread, or null if none is given.
             * @param sched_type The scheduler type, one of ORO_SCHED_OTHER, ORO_SCHED_RT or a value
             * defined by your RTOS.
             * @param stack_size The stack size of this thread, or zero when the default stack size
             * may be used.
             * @param start_routine The entry function that the thread will execute.
             * @param obj The argument to pass to that thread.
             * @return zero on success, -1 if it could not be created.
             */
            int rtos_task_create(RTOS_TASK* task,
                                 int priority,
                                 unsigned cpu_affinity,
                                 const char * name,
                                 int sched_type,
                                 size_t stack_size,
                                 void * (*start_routine)(void *),
                                 ThreadInterface* obj);

            /**
             * Yields the current thread. This function may be left empty.
             * @param task The task handle of the current thread.
             */
            void rtos_task_yield(RTOS_TASK* task);

            /**
             * Returns the process ID the OS gave to the task \a task.
             * @param task The task handle of the thread to query
             * @return 0 typically denotes the PID could not be obtained,
             * but any interpretation is left to the user of the Operating System.
             */
            unsigned int rtos_task_get_pid(const RTOS_TASK* task);

            /**
             * Set the scheduler of a given task \a t to a the type \a sched_type.
             * In some RTOS's the scheduler can't be changed or the change can
             * only be done for the calling thread.
             *
             * @param t The RTOS task to change.
             * @param sched_type A scheduler typed known by your RTOS.
             * @retval 0 when the scheduler of t was changed successfully
             * @retval -1 if scheduler for \a t could not be changed or sched_type
             * is invalid
             * @retval -2 if sched_type is valid but can only be changed for the
             * current thread which is not \a t. In this case, the calling code
             * should find a way to let the thread associated with \a t make the
             * call to rtos_task_set_scheduler().
             */
            int rtos_task_set_scheduler(RTOS_TASK* t, int sched_type);

            /**
             * Returns the current scheduler set for task \a t.
             *
             * @param t The RTOS task to query.
             * @return The scheduler type defined by your RTOS in which t is running.
             */
            int rtos_task_get_scheduler(const RTOS_TASK* t);

            /**
             * This function is to inform the RTOS that a thread is switching between
             * periodic or non-periodic execution. This may temporarily suspend the
             * thread. For on-line periodic changes, use rtos_task_set_period, which
             * is guaranteed to keep the thread running.
             *
             * @param mytask The RTOS task to change.
             * @param nanosecs Set to zero if you wish to switch to non periodic
             * execution, set to a positive value (the period in nano seconds)
             * if you wish to switch to periodic execution.
             */
            void rtos_task_make_periodic(RTOS_TASK* mytask, NANO_TIME nanosecs );

            /**
             * Change the period of a periodic RTOS task.
             * You must first use rtos_task_make_periodic() before calling
             * this function. This function may not suspend the thread.
             *
             * @param mytask The RTOS task to change the period.
             * @param nanosecs the new period. As a special exception, if this is equal to zero, it is
             * to inform the RTOS that we'll block this periodic thread for an
             * unspecified amount of time and that this should not trigger a watchdog,
             * or lead to 'catching up' or signalling overruns.
             */
            void rtos_task_set_period( RTOS_TASK* mytask, NANO_TIME nanosecs );

            /**
             * Set the wait policy of a thread.
             * @param task The RTOS task to change.
             * @param policy The wait policy.
             */
            void rtos_task_set_wait_period_policy( RTOS_TASK* task, int policy );

            /**
             * This function is called by a periodic thread which
             * wants to go to sleep and wake up the next period.
             * @param task This must be RTOS_TASK struct of the calling thread.
             * @return zero on success (or if the thread is non periodic),
             * non-zero to indicate an overrun.
             */
            int rtos_task_wait_period( RTOS_TASK* task );

            /**
             * This function must join the thread created with
             * rtos_task_create and then clean up the RTOS_TASK struct.
             * This function may not be called from the thread itself.
             * @param mytask The thread to join and cleanup.
             */
            void rtos_task_delete(RTOS_TASK* mytask);

            /**
             * This function checks (and corrects) if the given sched_type is valid
             * for this RTOS. If it's not, a sensible default is returned
             * in the argument
             * @param sched_type An integer denoting a scheduler type,
             * which will be updated if it's not a valid value.
             * @return 0 when it is valid, -1 when invalid and corrected.
             */
            int rtos_task_check_scheduler(int* sched_type);

            /**
             * This function checks (and corrects) a given priority
             * within a given scheduler type. Incorrect arguments are
             * re-set to a sensible default.
             * @param sched_type An integer denoting a scheduler type,
             * which will be updated if it's not a valid value.
             * @param priority An integer denoting the priority,
             * which will be updated if it's not a valid value (within that scheduler).
             * @return 0 when it is valid, -1 when invalid and corrected.
             */
            int rtos_task_check_priority(int* sched_type, int* priority);

            /**
             * Set the priority of a thread.
             * @param task The thread to change the priority of
             * @param priority The priority as interpreted by the underlying RTOS.
             * @return 0 if the priority could be set.
             */
            int rtos_task_set_priority(RTOS_TASK * task,int priority);

            /**
             * Return the priority of a thread.
             * @param task The thread to get the priority of
             * @return the priority, as recorded by the underlying RTOS.
             */
            int rtos_task_get_priority(const RTOS_TASK * task);

            /**
             * Set the cpu affinity of a thread.
             * @param task The thread to change the cpu affinity of
             * @param cpu_affinity is a bit mask with the cpu this thread should be bound.
             *        The bit 0 is the cpu 0, the bit 1, the cpu 1 and so on.
             *        For example ((1 << 0) | (1 << 1)) will bound to cpu 0 and 1.
             * @return 0 if the cpu affinity could be set.
             */
            int rtos_task_set_cpu_affinity(RTOS_TASK * task,unsigned cpu_affinity);

            /**
             * Return the cpu affinity of a thread.
             * @param task The thread to get the cpu affinity
             * @return the cpu affinity (@see rtos_task_set_cpu_affinity)
             */
            unsigned rtos_task_get_cpu_affinity(const RTOS_TASK * task);

            /**
             * Returns the name by which a task is known in the RTOS.
             * @param task The task to query.
             * @return It's name.
             */
            const char * rtos_task_get_name(const RTOS_TASK* task);

            /**
             * Set the name by which a task is known in the RTOS.
             * @param task The task to query.
             * @return It's name.
             */
            void rtos_task_set_name(RTOS_TASK* task, const char * name);

            /**
             * Returns 1 when \a task is the task struct of the thread
             * calling this function, 0 otherwise.
             * @param task the task struct to compare against.
             * @retval -1 if task is not a valid task struct or
             * this thread is not known to the rtos layer.
             */
            int rtos_task_is_self(const RTOS_TASK* task);
    }
}
#endif
