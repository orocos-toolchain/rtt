/***************************************************************************
  tag: Peter Soetens  Mon Jun 10 14:43:13 CEST 2002  PeriodicThread.hpp 

                        PeriodicThread.hpp -  description
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

#ifndef ORO_PERIODIC_THREAD_HPP
#define ORO_PERIODIC_THREAD_HPP

// Our own package config headers.
#include "../rtt-config.h"
#include "fosi.h"

#include "RunnableInterface.hpp"
#include "ThreadInterface.hpp"

#ifdef OROPKG_OS_THREAD_SCOPE
# include "dev/DigitalOutInterface.hpp"
#endif

#include <string>

namespace RTT
{ namespace OS {
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
        : public OS::ThreadInterface 
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
        PeriodicThread(int priority,const std::string & name, double period, OS::RunnableInterface* r=0);
    
        /**
         * Create a Thread with a given scheduler type, priority and a name.
         *
         * @param scheduler The scheduler, one of ORO_SCHED_RT or ORO_SCHED_OTHER.
         * @param priority The priority of the thread, with zero being the highest, 99 being the lowest.
         * @param name     The name of the Thread.
         * @param period   The period in seconds (eg 0.001) of the thread.
         * @param r        The optional RunnableInterface instance to run. If not present,
         *                 the thread's own virtual functions are executed.
         */
        PeriodicThread(int scheduler, int priority,const std::string & name, double period, OS::RunnableInterface* r=0);
    
        virtual ~PeriodicThread();

        virtual bool run( OS::RunnableInterface* r);

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
         * Read the name of this task
         */
        virtual const char* getName() const;

        /**
         * Get the RTOS_TASK pointer
         * FIXME should this be a const?
         */
        virtual RTOS_TASK * getTask(){return &rtos_task;};

        virtual bool setScheduler(int sched_type);
        virtual int getScheduler() const;

        /**
         * Set the periodicity of this thread
         */
        bool setPeriod(  TIME_SPEC p );

        virtual bool setPriority(int priority);

        virtual int getPriority() const;

        virtual void yield();

        void setMaxOverrun( int m );
        int getMaxOverrun() const;
        /**
         * Exit and destroy the thread 
         * @pre  this is only called from within the destructor.
         * @post the thread does no longer exist.
         */
        void terminate();

        void emergencyStop();
     protected:

        virtual void continuousStepping(bool yes_no);
        /**
         * Use this from within step() to stop this thread.
         * This function will call finalize() as well, thus it
         * is advised to call this function as the last statement
         * in your step().
         */
        virtual bool setToStop();

        virtual void step();
    
        virtual bool initialize();

        virtual void finalize();
     private:

        void setup(int _priority, const std::string& name);

        /**
         * Do configuration actions when the thread is stopped.
         */
        void configure();

        /**
         * The task struct.
         * @todo If we make this and other structs a pointer, the FOSI
         * can become a library and there is a larger binary compatibility.
         * A 'd' pointer approach would also be an improvement.
         */
        RTOS_TASK rtos_task;

        /**
         * Desired scheduler type.
         */
        int msched_type;

        /**
         * When set to 1, the thread will run, when set to 0
         * the thread will stop
         */
        bool running;

        bool prepareForExit;

        bool wait_for_step;

        rt_sem_t sem;
        rt_sem_t confDone;
        /**
         * The possible Runnable to run in this Component
         */
        OS::RunnableInterface* runComp;

        int maxOverRun;

        // Only used for passing on the period
        NANO_TIME period;

#ifdef OROPKG_OS_THREAD_SCOPE
	// Pointer to Threadscope device
        DigitalOutInterface * d;
#endif    
    };

}}

#endif
