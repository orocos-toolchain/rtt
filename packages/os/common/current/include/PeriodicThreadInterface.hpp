/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:41:46 CEST 2002  PeriodicThreadInterface.hpp 

                       PeriodicThreadInterface.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/



#ifndef PERIODIC_THREAD_INTERFACE_HPP
#define PERIODIC_THREAD_INTERFACE_HPP

#include "os/RunnableInterface.hpp"

namespace ORO_OS
{
    /**
     * A thread which is being run (or runs a RunnableInterface) periodically.
     * The periodicity is the time between the starting
     * of two runs.
     */
    class PeriodicThreadInterface
    {
    public:
        virtual ~PeriodicThreadInterface()
        {}

        typedef double Seconds;
        typedef long secs;
        typedef long msecs;
        typedef long long nsecs;

        /**
         * Run the functionality of a RunnableInterface object.
         * @param r The object to run or zero to clear.
         * @return true if accepted, false if the thread is running.
         */
        virtual bool run( RunnableInterface* r) = 0;

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
         * Set the periodicity of this thread
         * in seconds
         *
         * @return false if the thread is running.
         */
        virtual bool setPeriod( Seconds s ) = 0;

        /**
         * Set the periodicity of this thread
         * (seconds, nanoseconds).
         *
         * @return false if the thread is running.
         */
        virtual bool setPeriod( secs s, nsecs ns ) = 0;

        /**
         * Get the periodicity of this thread
         * (seconds,nanoseconds).
         *
         */
        virtual void getPeriod( secs& s, nsecs& ns ) const = 0 ;

        /**
         * Get the periodicity in Seconds
         */
        virtual Seconds getPeriod() const = 0 ;

        /**
         * Returns whether the thread is running
         */
        virtual inline bool isRunning() const = 0 ;

        /**
         * Set the name of this task
         */
        virtual void setName( const char* ) = 0;

        /**
         * Read the name of this task
         */
        virtual const char* getName() const = 0 ;

        /**
         * This method terminates the PeriodicThread in a safe
         * way. After the last step(), finalize will be called
         * and the thread cannot be restarted.
         *
         * @post The PeriodicThread is no longer running
         * @post This instance should no longer be used
         */
        virtual void terminate() = 0;

        /**
         * Instructs the thread to enter hard realtime mode.
         *
         * @return true if the operation succeeded.
         */
        virtual bool makeHardRealtime() = 0;

        /**
         * Instructs the thread to leave hard realtime mode.
         *
         * @return true if the operation succeeded.
         */
        virtual bool makeSoftRealtime() = 0;

        /**
         * Returns whether the thread is hard realtime.
         *
         * @return true if the thread is hard realtime.
         */
        virtual bool isHardRealtime() = 0;

    protected:
        /**
         * Disable waiting for the next step with \a true.
         * This will cause the step() method to be called
         * continuously, until this is enabled again or
         * the thread is stopped.
         */
        virtual void continuousStepping(bool yes_no) = 0;


        /**
         * Signal stopping to the Thread. This can be called
         * from within the task to indicate that no more runs 
         * should follow.
         * 
         * @post The current step will stop when it finishes
         */
        virtual bool setToStop() = 0;

        virtual bool initialize() = 0;

        virtual void step() = 0;

        virtual void finalize() = 0;

    };
}

#endif
