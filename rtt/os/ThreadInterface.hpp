/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:51 CEST 2005  ThreadInterface.hpp

                        ThreadInterface.hpp -  description
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


#ifndef OS_THREAD_INTERFACE_HPP
#define OS_THREAD_INTERFACE_HPP

#include "fosi.h"
#include "threads.hpp"
#include "Time.hpp"
#include "../rtt-config.h"

namespace RTT
{
    namespace os
    {
        /**
         * A thread which is being run.
         * The periodicity is the time between the starting
         * of two runs or zero for non periodic threads.
         */
        class RTT_API ThreadInterface
        {
        public:
            ThreadInterface();
            virtual ~ThreadInterface();

            /**
             * Start the Thread.
             *
             * @post    initialize() is called first
             * @post    The Thread is running
             * @return  true if the function did succeed.
             *       false otherwise.
             */
            virtual bool start() = 0;

            /**
             * Stop the Thread.
             *
             * @post    The Thread is no longer being run
             * @post    finalize() is called when the Thread is stopped.
             * @return  true if the function did succeed.
             *          false otherwise.
             */
            virtual bool stop() = 0;

            /**
             * Get the periodicity in Seconds. Return zero if non periodic.
             */
            virtual Seconds getPeriod() const = 0;

            /**
             * Set the periodicity in Seconds.
             * @param new_period A positive number expressing the period
             * @return true if it was accepted, false otherwise.
             */
            virtual bool setPeriod(Seconds new_period) = 0;

            /**
             * Get the periodicity in nanoseconds. Return zero if non periodic.
             */
            virtual nsecs getPeriodNS() const = 0;

            /**
             * Returns whether the thread is running.
             * A thread is running if it is executing its loop()
             * function or in the process of periodically calling
             * step().
             * Hence for periodic threads, isRunning() == isActive()
             * while for non-periodic threads, isRunning() is only
             * true if isActive() and it is executing loop().
             */
            virtual bool isRunning() const = 0;

            /**
             * Returns whether the thread is active.
             * A thread is active between the invocation of start()
             * and the invocation of stop().
             */
            virtual bool isActive() const = 0;

            /**
             * Read the name of this task
             */
            virtual const char* getName() const = 0;

            /**
             * Get the RTOS_TASK pointer.
             * @note Using this function leads to non-portable code.
             * It is here for users which wish to tweak OS specific
             * thread settings.
             */
            virtual RTOS_TASK * getTask() = 0;

            /**
             * const version of the above.
             */
            virtual const RTOS_TASK * getTask() const = 0;

            /**
             * Change the scheduler policy in which this
             * thread runs.
             * @param sched_type An OS-specific value which
             * selects a scheduler. Orocos requires that these
             * two values are available:
             * - ORO_SCHED_RT: Hint the OS that this thread should
             * be scheduled as a priority or real-time process.
             * - ORO_SCHED_OTHER: Hint the OS that this thread should
             * \b not be scheduled as a priority or real-time process.
             *
             * Your OS can in addition provide other \a sched_type's which
             * map more naturally to the schedulers present. If your
             * OS does not make a distinction between real-time and other,
             * both values may map to the same scheduler type.
             *
             * @return true if the change could be made.
             */
            virtual bool setScheduler(int sched_type) = 0;

            /**
             * Get the scheduler policy in which this
             * thread runs.
             * @return An OS-specific value which
             * represents the used scheduler.
             * @see setScheduler
             */
            virtual int getScheduler() const = 0;

            /**
             * Set the priority of this Thread.
             * @param priority The priority given upon construction of this
             * thread. It has to be interpreted in the current OS scheduler.
             * @see setScheduler
             */
            virtual bool setPriority(int priority) = 0;

            /**
             * The priority of this Thread.
             * @return The priority given upon construction of this
             * thread or set with setPriority. The returned number has to be interpreted
             * in the current OS scheduler.
             * @see setScheduler
             */
            virtual int getPriority() const = 0;

            /**
             * Returns the Process or Thread ID of this thread, as assigned by the
             * Operating System.
             * This function tries to return the Thread ID, if the OS supports it,
             * otherwise it returns the process ID. If that is not available either,
             * zero is returned.
             * On Linux systems for example, the Thread ID of the main thread is
             * equal to the Process ID. The Thread ID of every newly created thread
             * is different from the Process ID (and unique).
             */
            virtual unsigned int getPid() const = 0;

            /**
             * @return the cpu affinity
             */
            virtual unsigned getCpuAffinity() const = 0;

            virtual void setMaxOverrun(int m) = 0;

            virtual int getMaxOverrun() const = 0;

            /**
             * Set the wait policy of a periodic thread
             * @param The wait policy can be ORO_WAIT_ABS (absolute wait)
             * and ORO_WAIT_REL (relative wait, with respect to current time)
             */
            virtual void setWaitPeriodPolicy(int p) = 0;

            /**
             * Yields (put to the back of the scheduler queue) the calling thread.
             */
            virtual void yield() = 0;

            /**
             * The unique thread number (within the same process).
             */
            unsigned int threadNumber() const
            {
                return threadnb;
            }

            bool isSelf() const;
        protected:
            /**
             * Threads are given an unique number,
             * which follows thread creation order.
             * @see os::threads
             */
            int threadnb;
        };
    }
}

#endif
