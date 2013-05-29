/***************************************************************************
  tag: Peter Soetens  Mon Jun 10 14:43:13 CEST 2002  Thread.hpp

                        Thread.hpp -  description
                           -------------------
    begin                : Mon June 10 2002
    copyright            : (C) 2002 Peter Soetens
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

#ifndef ORO_THREAD_HPP
#define ORO_THREAD_HPP

// Our own package config headers.
#include "../rtt-config.h"
#include "fosi.h"

#include "ThreadInterface.hpp"
#include "Mutex.hpp"

#include <string>

namespace RTT
{

    namespace os
    {
        /**
         * A Thread object executes user code in its own thread.
         *
         * If the underlying OS supports it, both period and priority can be changed
         * after creation. When the period is 0, the loop() function is executed, when
         * the period is greater than 0, the step() function is executed. Note: if the
         * user does not implement loop(), it will call step() by default.
         *
         * The main functions to implement are initialize(), step()/loop() and finalize().
         * initialize() is called when start() is called, and before the first step()/loop() and finalize() is called
         * when stop() is called after the last step()/loop() returns.
         *
         * @section Periodic behavior
         *
         * When a period is set, step() is executed from the moment start() is called,
         * and afterwards according to it's period. When stop() is called, it waits
         * for the step() function to return and then stops the periodic execution.
         *
         * Step() overruns are detected and the threshold to 'emergency stop' the thread can be
         * set by \a setMaxOverrun(). Overruns must be accumulated 'on average' to trigger this behavior:
         * one not overrunning step() compensates for one overrunning step().
         *
         * @section Non periodic behaviour
         *
         * The first invocation of
         * start() invokes the initialize() function and runs the loop()
         * method in the thread.  When loop() returns the thread waits for
         * another start() to execute loop() again. In that case,
         * initialize() is not executed.
         *
         * When stop() is called
         * and the thread is still executing loop() and breakLoop()
         * returns \a true (\b not the default), the stop() function
         * succeeds and the finalize() method is called by stop().
         * If the thread was not executing
         * loop(), stop will always call finalize() and return success.
         *
         * The user must provide an
         * implementation of breakLoop() returning \a true to make stop()
         * work while loop() is being executed. stop() will fail and not execute
         * finalize() if the thread executes loop() and breakLoop() is not
         * reimplemented to return true.
         *
         * @section Periodicities, priorities, schedulers and stack sizes
         *
         * These four parameters are the parameters that users wish to set
         * for every thread created. All but stack size can be modified
         * after creation, and if the OS permits it, modified even when
         * the thread is using user code. All these parameters are passed
         * to the underlying OS unmodified. In case an incorrect combination
         * is made, the change may be rejected or adjusted to a close or
         * safe value.
         * @see setPeriod, setScheduler, setPriority, setStackSize
         */
        class RTT_API Thread: public ThreadInterface
        {
            friend void* thread_function(void* t);

        public:

            /**
             * Create a Thread with a given scheduler type, priority and a name.
             *
             * @param scheduler The scheduler, one of ORO_SCHED_RT or ORO_SCHED_OTHER.
             * @param priority The priority of the thread, this is interpreted by your RTOS.
             * @param period   The period in seconds (eg 0.001) of the thread, or zero if not periodic.
             * @param cpu_affinity The cpu affinity of the thread, this is interpreted by your RTOS.
             * @param name     The name of the Thread. May be used by your OS to identify the thread.
             *                 the thread's own virtual functions are executed.
             */
            Thread(int scheduler, int priority, double period, unsigned cpu_affinity,
                   const std::string & name);

            virtual ~Thread();

            /**
             * Sets the stack size of the threads to be created.
             * This value is suggestive and may be altered or ignored by your
             * operating system.
             * Use zero to use the system's default.
             * @param ssize the size of the stack in bytes
             */
            static void setStackSize(unsigned int ssize);

            /**
             * Sets the lock timeout for a thread which does not have a period
             * The default is 1 second 
             * @param timeout_in_s the timeout is seconds
             */
            static void setLockTimeoutNoPeriod(double timeout_in_s);

            /**
             * Set the lock timeout for a thread which has a period
             * by a factor of the period
             * The default is factor 10
             * @param factor Factor of the period 
             */
            static void setLockTimeoutPeriodFactor(double factor);


            virtual bool start();

            virtual bool stop();

            /** Sets the timeout for stop(), in seconds
             *
             * @see getStopTimeout
             */
            void setStopTimeout(Seconds s);

            /** Returns the desired timeout for stop(), in seconds
             *
             * If not set with setStopTimeout, the timeout is deduced from the
             * global values lock_timeout_period_factor and
             * lock_timeout_no_period_in_s
             */
            Seconds getStopTimeout() const;

            bool setPeriod(Seconds s);

            /**
             * Set the periodicity of this thread
             * (seconds, nanoseconds)
             */
            bool setPeriod(secs s, nsecs ns);

            /**
             * Set the periodicity of this thread
             */
            bool setPeriod(TIME_SPEC p);

            /**
             * Get the periodicity of this thread
             * (seconds, nanoseconds)
             */
            void getPeriod(secs& s, nsecs& ns) const;

            virtual Seconds getPeriod() const;

            virtual nsecs getPeriodNS() const;

            virtual bool isPeriodic() const;

            virtual bool isRunning() const;

            virtual bool isActive() const;

            virtual const char* getName() const;

            virtual RTOS_TASK * getTask()
            {
                return &rtos_task;
            }

            virtual const RTOS_TASK * getTask() const
            {
                return &rtos_task;
            }

            virtual bool setScheduler(int sched_type);

            virtual int getScheduler() const;

            virtual bool setPriority(int priority);

            virtual int getPriority() const;

            virtual unsigned int getPid() const;
            /**
             * Set cpu affinity for this thread
             * @cpu_affinity The cpu affinity of the thread (@see rtos_task_set_cpu_affinity).
             * @return true if the mask has been applied
             */
            virtual bool setCpuAffinity(unsigned cpu_affinity);

            /**
             * @return the cpu affinity (@see rtos_task_set_cpu_affinity)
             */
            virtual unsigned getCpuAffinity() const;

            virtual void yield();

            virtual void setMaxOverrun(int m);

            virtual int getMaxOverrun() const;

            virtual void setWaitPeriodPolicy(int p);

        protected:
            /**
             * Exit and destroy the thread
             * @pre  this is only called from within the destructor.
             * @post the thread does no longer exist.
             */
            void terminate();

            void emergencyStop();

            /**
             * @see base::RunnableInterface::step()
             */
            virtual void step();

            /**
             * @see base::RunnableInterface::loop()
             */
            virtual void loop();

            /**
             * @see base::RunnableInterface::breakLoop()
             */
            virtual bool breakLoop();

            /**
             * @see base::RunnableInterface::initialize()
             */
            virtual bool initialize();

            /**
             * @see base::RunnableInterface::finalize()
             */
            virtual void finalize();
        private:
            Thread(const Thread&);

            void setup(int _priority, unsigned cpu_affinity, const std::string& name);

            /**
             * Do configuration actions when the thread is stopped.
             */
            void configure();

            static unsigned int default_stack_size;

            /**
             *  configuration of the lock timeout in seconds
             */
            static double lock_timeout_no_period_in_s;

            /**
             * configuration of the lock timeout for periodic tasks in seconds
             */
            static double lock_timeout_period_factor;

            /**
             * Desired scheduler type.
             */
            int msched_type;

            /**
             * When set to 1, the thread will run, when set to 0
             * the thread will stop ( isActive() )
             */
            bool active;

            /**
             * Signal the thread that it should exit.
             */
            bool prepareForExit;

            /**
             * Is true when in the loop (isRunning() )
             */
            bool inloop;

            /**
             * Indicates if step() or loop() should be executed.
             */
            bool running;

            /**
             * The realtime task structure created by this object.
             */
            RTOS_TASK rtos_task;

            /**
             * The semaphore used for starting the thread.
             */
            rt_sem_t sem;

            /**
             * Used to implement synchronising breakLoop().
             */
            MutexRecursive breaker;

            /**
             * The maximum times a periodic overrun may happen,
             * or -1 if unlimited.
             */
            int maxOverRun;

            /**
             * The period as it is passed to the operating system.
             */
            NANO_TIME period;

            /**
             * The timeout, in seconds, for stop()
             */
            double stopTimeout;

#ifdef OROPKG_OS_THREAD_SCOPE
            // Pointer to Threadscope device
            dev::DigitalOutInterface * d;
#endif
        };

    }
}

#endif
