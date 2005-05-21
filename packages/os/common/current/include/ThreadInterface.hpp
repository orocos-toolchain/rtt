/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:51 CEST 2005  ThreadInterface.hpp 

                        ThreadInterface.hpp -  description
                           -------------------
    begin                : Sat May 21 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef OS_THREAD_INTERFACE_HPP
#define OS_THREAD_INTERFACE_HPP

#include "os/RunnableInterface.hpp"

namespace ORO_OS
{
    /**
     * A thread which is being run (or runs a RunnableInterface).
     * The periodicity is the time between the starting
     * of two runs or zero for non periodic threads.
     */
    class ThreadInterface
    {
    public:
        ThreadInterface();
        virtual ~ThreadInterface();

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
         * Get the periodicity in Seconds. Return zero if non periodic.
         */
        virtual Seconds getPeriod() const = 0 ;

        /**
         * Get the periodicity in nanoseconds. Return zero if non periodic.
         */
        virtual nsecs getPeriodNS() const = 0 ;

        /**
         * Returns whether the thread is running
         */
        virtual inline bool isRunning() const = 0 ;

        /**
         * Read the name of this task
         */
        virtual const char* getName() const = 0 ;

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
        virtual bool isHardRealtime() const = 0;

        /**
         * The priority of this Thread.
         */
        virtual int getPriority() const = 0;

        /**
         * The unique thread number.
         */
        unsigned int threadNumber() const { return threadnb; }
    protected:
        /**
         * Threads are given an unique number,
         * which follows thread creation order.
         * @see ORO_OS::threads
         */
        int threadnb;
    };
}

#endif
