/***************************************************************************
  tag: Peter Soetens  Mon Jun 10 14:43:13 CEST 2002  PeriodicThread.hpp 

                        PeriodicThread.hpp -  description
                           -------------------
    begin                : Mon June 10 2002
    copyright            : (C) 2002 Peter Soetens
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

#ifndef PERIODIC_THREAD_HPP
#define PERIODIC_THREAD_HPP

// Our own package config headers.
#include "pkgconf/os.h"
#include <os/fosi.h>

#include <os/RunnableInterface.hpp>
#include <os/ThreadInterface.hpp>

#include <string>

namespace ORO_CoreLib {
    class Handle;
}

namespace ORO_OS
{
    /**
     * A PeriodicThread executes its step() method (or a RunnableInterface ) periodically.
     *
     * It has a fixed priority, a name and a (changable) periodicity. The common methods
     * to use are start() and stop().
     *
     * Step() Overruns are detected and the threshold to 'emergency stop' the thread can be
     * set by \a setMaxOverrun(). Overruns must be accumulated 'on average' to trigger this behavior:
     * one not overrunning step() compensates for one overrunning step().
     */
    class PeriodicThread 
        : public ORO_OS::ThreadInterface 
    {
        friend void* periodicThread( void* t );

    public:

        /**
         * Create a Thread with a given priority and a name.
         *
         * @param priority The priority of the thread, with zero being the highest, 99 being the lowest.
         * @param name     The name of the Thread.
         * @param period   The period in seconds (eg 0.001) of the thread.
         * @param r        The optional RunnableInterface instance to run. If not present,
         *                 the thread's own virtual functions are executed.
         */
        PeriodicThread(int priority, std::string name, double period, ORO_OS::RunnableInterface* r=0);
    
        virtual ~PeriodicThread();

        virtual bool run( ORO_OS::RunnableInterface* r);

        /**
         * Start the thread
         */
        virtual bool start();
        /**
         * Stop the thread
         */
        virtual bool stop();
        /**
         * Set the periodicity of this thread
         * in seconds.
         */
        bool setPeriod( Seconds s );
        /**
         * Set the periodicity of this thread
         * (seconds, nanoseconds)
         */
        bool setPeriod( secs s, nsecs ns );
        /**
         * Get the periodicity of this thread
         * (seconds, nanoseconds)
         */
        void getPeriod( secs& s, nsecs& ns ) const;

        /**
         * Get the periodicity of this thread in seconds.
         */
        virtual Seconds getPeriod() const;

        /**
         * Get the periodicity of this thread in nano-seconds.
         */
        virtual nsecs getPeriodNS() const;

        /**
         * Returns whether the thread is running
         */
        virtual bool isRunning() const;

        /**
         * Set the name of this task
         */
        void setName(const char*);
        /**
         * Read the name of this task
         */
        virtual const char* getName() const;

        virtual bool makeHardRealtime();
        virtual bool makeSoftRealtime();
        virtual bool isHardRealtime() const;

        /**
         * Set the scheduler of the thread to \a sched.
         * @param sched One of SCHED_OTHER, SCHED_RR or SCHED_FIFO, or any other known scheduler type.
         * @pre this->isRunning() == true && this->isHardRealtime() == false
         * @return true if the preconditions were met, false otherwise.
         */
        bool setScheduler( int sched );

        /**
         * Set the periodicity of this thread
         */
        bool setPeriod(  TIME_SPEC p );

        virtual int getPriority() const;

        virtual int getPosixPriority() const;

        void setMaxOverrun( int m );
        int getMaxOverrun() const;
     protected:
        /**
         * Exit the thread 
         * @pre  this is only called from within the thread
         * @post the thread does no longer exist
         */
        void terminate();

        virtual void continuousStepping(bool yes_no);
        virtual bool setToStop();

        virtual void step();
    
        virtual bool initialize();

        virtual void finalize();

      void emergencyStop();
     private:
        /**
         * Do configuration actions when the thread is stopped.
         */
        void configure();

        /**
         * Called from within the thread to reconfigure the linux scheduler.
         */
        void reconfigScheduler();

        /**
         * Periodicity of the thread in ns.
         */
        NANO_TIME period;

        /**
         * When set to 1, the thread will run, when set to 0
         * the thread will stop
         */
        bool running;

        /**
         * True when the thread should go realtime.
         */
        bool goRealtime;

        /**
         * The realtime task
         */
        RTOS_TASK* rtos_task;

        /**
         * The thread struct
         */
        RTOS_THREAD thread;

        int priority;

        std::string taskName;

        bool prepareForExit;

        rt_sem_t sem;
        rt_sem_t confDone;
        /**
         * The possible Runnable to run in this Component
         */
        ORO_OS::RunnableInterface* runComp;

        ORO_CoreLib::Handle* h;
        // We cannot include Event.hpp inhere,
        // thus we use a void* + static_cast in cxx file.
        void* stopEvent;

        bool wait_for_step;

        /**
         * The linux scheduler of this thread. One of SCHED_OTHER, SCHED_RR or SCHED_FIFO.
         */
        int sched_type;

      int maxOverRun;
    };

}

#endif
