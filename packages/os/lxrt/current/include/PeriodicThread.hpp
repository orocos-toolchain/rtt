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
#include "pkgconf/os_lxrt.h"

#include <os/fosi.h>

#include <os/RunnableInterface.hpp>
#include <os/PeriodicThreadInterface.hpp>

#include <string>

namespace ORO_CoreLib {
    class Handle;
}

namespace ORO_OS
{
    /**
     * @brief A PeriodicThread executes its step() method (or a RunnableInterface ) periodically.
     *
     * It has a fixed priority, a name and a periodicity.
     */
    class PeriodicThread 
        : public PeriodicThreadInterface 
    {
        friend void* ComponentThread( void* t );

    public:

        /**
         * Create a Thread with a given priority and a name.
         *
         * @param priority The priority of the thread, with zero being the highest.
         * @param name     The name of the Thread.
         * @param period   The period in seconds (eg 0.001) of the thread.
         * @param r        The optional RunnableInterface instance to run. If not present,
         *                 the thread's own RunnableInterface functions are executed.
         */
        PeriodicThread(int priority, const std::string& name, double period=0.01, RunnableInterface* r=0);
    
        virtual ~PeriodicThread();
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
        virtual int periodSet( Seconds s );
        /**
         * Set the periodicity of this thread
         * (seconds, nanoseconds)
         */
        virtual int periodSet( secs s, nsecs ns );
        /**
         * Get the periodicity of this thread
         * (seconds, nanoseconds)
         */
        virtual void periodGet( secs& s, nsecs& ns ) const;
        /**
         * Get the periodicity in seconds
         */
        virtual double periodGet() const;
        /**
         * Returns whether the thread is running
         */
        virtual bool isRunning() const;

        /**
         * Set the name of this task
         */
        virtual void taskNameSet(const char*);
        /**
         * Read the name of this task
         */
        virtual const char* taskNameGet() const;
        /**
         * Exit the thread 
         * @pre  this is only called from within the thread
         * @post the thread does no longer exist
         */
        virtual void terminate();

        virtual void step();
    
        virtual bool initialize();

        virtual void finalize();

        bool makeHardRealtime() 
        { 
            if ( !running ) 
                {
                    goRealtime = true; 
                    rt_sem_signal(sem);
                    rt_sem_wait(confDone);
                }
            return goRealtime; 
        }
        bool makeSoftRealtime()
        { 
            if ( !running ) 
                {
                    goRealtime = false; 
                    rt_sem_signal(sem);
                    rt_sem_wait(confDone);
                }
            return !goRealtime; 
        }
        bool isHardRealtime()   { return rt_is_hard_real_time(rt_task); }

    protected:
        virtual bool setToStop();

        /**
         * Do configuration actions when the thread is stopped.
         */
        void configure();
        /**
         * Set the periodicity of this thread
         */
        int periodSet(  TIME_SPEC p );

        /**
         * Wait for the full period periodGet()
         */
        void periodWait();
        /**
         * Wait only for the remaining period, being
         * periodGet() - (time_now - start_time_of_this_period)
         */
        void periodWaitRemaining();

        /**
         * Periodicity of the thread in ns.
         */
        RTIME period;

        /**
         * When set to 1, the thread will run, when set to 0
         * the thread will stop
         */
        bool running;

        /**
         * Signals if rt_task is stopped or not
         */
        bool stopped;

        /**
         * True when the thread should go realtime.
         */
        bool goRealtime;

        /**
         * The realtime task
         */
        RT_TASK* rt_task;

        /**
         * The userspace thread carying the rt_task.
         */
        pthread_t thread;

        int priority;

        static const int TASKNAMESIZE = 64;

        char taskName[TASKNAMESIZE];

        bool prepareForExit;

        SEM* sem;
        SEM* confDone;
        /**
         * The possible Runnable to run in this Component
         */
        RunnableInterface* runComp;

        ORO_CoreLib::Handle* h;
    };

}

#endif
