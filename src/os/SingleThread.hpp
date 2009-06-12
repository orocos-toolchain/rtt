/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:53 CEST 2004  SingleThread.hpp

                        SingleThread.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
    copyright            : (C) 2004 Peter Soetens
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


#ifndef SINGLE_THREAD_HPP
#define SINGLE_THREAD_HPP

// Our own package config headers.
#include "../rtt-config.h"
#include "fosi.h"

#include "RunnableInterface.hpp"
#include "ThreadInterface.hpp"
#include "Mutex.hpp"

#include <string>

namespace RTT
{
    class DigitalOutInterface;

    namespace OS {
    /**
     * This Thread abstraction class represents a single-shot thread
     * which can be started many times.  The first invocation of
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
     * When a RunnableInterface object is given, the above methods
     * initialize(), loop(), breakLoop() and finalize() are called
     * on that object instead of on the SingleThread's virtual functions.
     *
     * @see RunnableInterface
     */
    class SingleThread
        : public OS::ThreadInterface
    {
        friend void* singleThread_f( void* t );
    public:
        /**
         * Create a single-shot Thread with priority \a priority, a \a name and optionally,
         * an object to execute.
         * @param priority The priority of the thread, in the ORO_SCHED_RT domain.
         * @param name     The name of the Thread.
         * @param r        The optional RunnableInterface instance to run. If not present,
         *                 the thread's own virtual functions are executed.
         */
        SingleThread(int priority, const std::string& name, OS::RunnableInterface* r=0);

        /**
         * Create a single-shot Thread with priority \a priority, a \a name and optionally,
         * an object to execute.
         * @param scheduler The scheduler, one of ORO_SCHED_RT or ORO_SCHED_OTHER.
         * @param priority The priority of the thread, in the \a scheduler domain.
         * @param name     The name of the Thread.
         * @param r        The optional RunnableInterface instance to run. If not present,
         *                 the thread's own virtual functions are executed.
         */
        SingleThread(int scheduler, int priority, const std::string& name, OS::RunnableInterface* r=0);

        virtual ~SingleThread();

        virtual bool run( OS::RunnableInterface* r);

        /**
         * Start the thread
         * @post if !this->isRunning(), then execute
         * initialize() and execute loop() in the thread.
         * @post if this->isRunning(), then schedule a start of loop()
         * in the thread.
         * @post this->isRunning is true
         *
         * @return true if successfull.
         */
        virtual bool start();

        /**
         * Stop the thread. The return value of stop, is the
         * same as the return value of RunnableInterface::breakLoop().
         * @pre this->isRunning()
         * @post if breakLoop() returns \a true, then execute
         * finalize() and this->isRunning() == false
         * @post if breakLoop() returns \a false (the default),
         * then do nothing.
         *
         * @return true if successfull.
         */
        virtual bool stop();

        /**
         * Returns whether the thread is running
         */
        virtual bool isRunning() const;

        /**
         * Returns whether the thread is active
         */
        virtual bool isActive() const;

        /**
         * Read the name of this task
         */
        virtual const char* getName() const;
        /**
         * Get the RTOS_TASK pointer
         * FIXME should this be a const?
         */
        virtual RTOS_TASK * getTask(){return &(this->rtos_task);};

        virtual bool setScheduler(int sched_type);
        virtual int getScheduler() const;

        virtual Seconds getPeriod() const { return 0.0; }
        virtual bool setPeriod(Seconds period) { return false; }
        virtual nsecs getPeriodNS() const { return 0; }

        virtual bool setPriority(int priority);

        virtual int getPriority() const;

        virtual void yield();

    protected:

        virtual bool breakLoop();

        virtual void loop();

        virtual bool initialize();

        virtual void finalize();

    private:
        void setup(int _priority, const std::string& name);

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
         * Signal flag between thread and start()/stop()
         */
        bool runloop;

        /**
         * The realtime task structure created by this object.
         */
        RTOS_TASK rtos_task;

        /**
         * The semaphore used for starting the thread.
         */
        rt_sem_t sem;

        /**
         * The semaphore used for communicating between
         * the thread and the constructor/destructor.
         */
        rt_sem_t confDone;

        /**
         * The possible Runnable to run in this Component
         */
        OS::RunnableInterface* runComp;

        /**
         * Used to implement synchronising breakLoop().
         */
        MutexRecursive breaker;

#ifdef OROPKG_OS_THREAD_SCOPE
	// Pointer to Threadscope device
        DigitalOutInterface * d;
#endif
    };

}}

#endif
