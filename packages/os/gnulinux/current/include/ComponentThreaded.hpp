/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:42:38 CEST 2002  ComponentThreaded.hpp 

                       ComponentThreaded.hpp -  description
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



#ifndef COMPONENTTHREADED_HPP
#define COMPONENTTHREADED_HPP

// Our own package config headers.
#include "pkgconf/os.h"
#include "pkgconf/os_gnulinux.h"

// code headers
#include "os/fosi.h"
#include "os/ComponentActiveInterface.hpp"
#include "RunnableInterface.hpp"

#include <string>

namespace ORO_OS
{
    class Finalizer;

    /**
     *
     *        An active component is a threaded component
     *
     *        The periodicity is the time between the starting
     *        of two runs.
     */
    class ComponentThreaded : public ComponentActiveInterface
    {
        friend void *ComponentThread( void *t );

    public:
        /**
         * Create a Thread with a given priority, and optional : a name and an object to run.
         *
         * @param priority The priority of the thread, with zero being the highest.
         * @param name     The name of the Thread.
         * @param period   The period in seconds (eg 0.001) of the thread.
         * @param r        The optional RunnableInterface instance to run. If not present,
         *                 the thread's own RunnableInterface functions are executed.
         */
        ComponentThreaded(int priority, const std::string& name="", double period=0.01, RunnableInterface* r = 0 );

        /**
         * Destructor. Stops the thread safely (if needed).
         */
        virtual ~ComponentThreaded();

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
            virtual int periodSet( const secs s, const nsecs ns );
            /**
             * Get the periodicity of this thread
             * (seconds, nanoseconds)
             */
            virtual void periodGet( secs& s, nsecs& ns ) const;
            /**
             * Get the periodicity in seconds
             */
            virtual Seconds periodGet() const;
            /**
             * Returns whether the thread is running
             */
            virtual bool isRunning() const {  return running; };

            /**
             * Set the name of this task
             */
            virtual void taskNameSet( const char* );
            /**
             * Read the name of this task
             */
            virtual const char* taskNameGet() const;

            /**
             * Used to terminate the thread from within destructor
             */
            void terminate();

        bool makeHardRealtime() {return false;}
        bool makeSoftRealtime() {return true;}
        bool isHardRealtime()   {return false;}
        protected:
            /**
             * Set the periodicity of this thread
             */
            int periodSet( TIME_SPEC p );

            virtual void step();

            virtual bool initialize();

            virtual void finalize();

            RunnableInterface* runner;

        private:
            /**
             * return the attributes of this thread
             */
            pthread_attr_t* threadAttributeGet();
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
             * Finish a running thread
             * @pre  this is only called outside the thread
             * @post the thread does no longer exist
             */
            void finishThread();
            /**
              * The thread for this object.
              */
            RTOS_TASK thread;
            /**
             * Periodicity of the thread
             */
            TIME_SPEC period;
            /**
             * The time when the current run was started
             */
            NANO_TIME periodMark;
            /**
             * When set to 1, the thread will run, when set to 0
             * the thread will stop
             */
            bool running;
            /**
             * Flag to signal the thread to exit
             */
            bool timeToQuit;
            /**
             * Flag indicating the thread is destroyed
             */
            int destroyed;
            /**
             * Thread attributes
             * (posix but portable ? )
             */
            pthread_attr_t threadAttribute;

    protected:
	    static const unsigned int TASKNAME_SIZE = 64;
    
        char taskName[ TASKNAME_SIZE ];

        /**
         * Can be called by the thread to stop itself
         * asynchronously within step(). This means this function
         * returns.
         */
        virtual bool setToStop();

        Finalizer* finalizer;
    };
}

#endif
