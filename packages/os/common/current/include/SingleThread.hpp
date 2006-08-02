/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:53 CEST 2004  SingleThread.hpp 

                        SingleThread.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 

#ifndef SINGLE_THREAD_HPP
#define SINGLE_THREAD_HPP

// Our own package config headers.
#include "pkgconf/os.h"
#include "fosi.h"

#include "RunnableInterface.hpp"
#include "ThreadInterface.hpp"
#include "Mutex.hpp"

#include <string>

namespace RTT
{ namespace OS {
    /**
     * This Thread abstraction class represents a single-shot thread
     * which can be started many times.  The first invocation of
     * start() invokes the initialize() function and runs the loop()
     * method in the thread.  When loop() returns the thread waits for
     * another start() to execute loop() again. When stop() is called
     * when the thread is still executing loop() and breakLoop()
     * returns \a true (\b not the default), the stop() function
     * succeeds and the finalize() method is called by stop(). A next
     * invocation of start() will again call initialize() before
     * loop() is executed and so on. The user must provide an
     * implementation of breakLoop() returning \a true to make stop()
     * work while loop() is being executed. stop() will not execute
     * finalize() if the thread executes loop() and breakLoop() is not
     * reimplemented to return true. If the thread was not executing
     * loop(), stop will always call finalize() and return success. 
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
         */
        SingleThread(int priority, const std::string& name, OS::RunnableInterface* r=0);
    
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

        virtual bool makeHardRealtime();

        virtual bool makeSoftRealtime();

        virtual bool isHardRealtime() const;

        virtual Seconds getPeriod() const { return 0.0; }
        virtual nsecs getPeriodNS() const { return 0; }

    virtual int getPriority() const;

        void yield();

    protected:

        virtual bool breakLoop();

        virtual void loop();
    
        virtual bool initialize();

        virtual void finalize();

    private:
        /**
         * When set to 1, the thread will run, when set to 0
         * the thread will stop ( isActive() )
         */
        bool active;

        /**
         * True when the thread should go realtime.
         */
        bool goRealtime;

        /**
         * Signal the thread that it should exit.
         */
        bool prepareForExit;

        /**
         * Is true when in the loop (isRunning() )
         */
        bool inloop;

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
        Mutex breaker;
    };

}}

#endif
