/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:43:13 CEST 2002  PeriodicThread.hpp 

                       PeriodicThread.hpp -  description
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


#ifndef PERIODIC_THREAD_HPP
#define PERIODIC_THREAD_HPP

#include "os/cpp.hpp"
#include "os/fosi.h"
#include "Task.hpp"
#include "PeriodicThreadInterface.hpp"
#include "RunnableInterface.hpp"

#include "EventInterfaces.hpp"

/**
  *
  *        An active component is a threaded component
  *
  *        The periodicity is the time between the starting
  *        of two runs.
  */

namespace ORO_OS
{

    class PeriodicThread
                : public PeriodicThreadInterface,
                private Task
    {

    public:

        /**
         * Constructs a PeriodicThread with a priority and an optional
         * RunnableInterface which it must execute.
         *
         * @param priority 
         *        The tasks priority
         * @param r
         *        The RunnableInterface it will run when present
         */
        PeriodicThread(int priority, const std::string& name="", RunnableInterface* r = 0 );

            virtual ~PeriodicThread();
            /**
             * Start the thread
             */
            bool start();
            /**
             * Stop the thread
             */
            bool stop();
            /**
             * Set the periodicity of this thread
             * (seconds, nanoseconds)
             */
            int setPeriod( secs s, nsecs ns );
            /**
             * Get the periodicity of this thread
             * (seconds, nanoseconds)
             */
            void getPeriod( secs& s, nsecs& ns ) const;
            /**
             * Get the periodicity in seconds
             */
            Seconds getPeriod() const;
            /**
             * Returns whether the thread is running
             */
            inline bool isRunning() const;

            /**
             * Set the name of this task
             */
            void taskNameSet( const char* );
            /**
             * Read the name of this task
             */
            const char* taskNameGet() const;
            /**
             * Exit the thread 
             * @pre  this is only called from within the thread
             * @post the thread does no longer exist
             */
            void terminate();

            virtual int run();

        bool makeHardRealtime() {return true;}
        bool makeSoftRealtime() {return false;}
        bool isHardRealtime()   {return true;}
        protected:
            /**
             * Set the periodicity of this thread
             */
            int setPeriod( TIME_SPEC p );
            /**
             * This method will be executed in each period once,
             * when isRunning() == 1
             */
            virtual void step();

            virtual bool initialize();

            virtual void finalize();

            virtual void setToStop();

            /**
             * Wait for the full period getPeriod()
             */
            void periodWait();
            /**
             * Wait only for the remaining period, being
             * getPeriod() - (time_now - start_time_of_this_period)
             */
            void periodWaitRemaining();

        private:

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

            bool stopped;

        protected:
	    const static unsigned int TASKNAME_SIZE = 64;
            char taskName[ TASKNAME_SIZE ];

        /**
         * The Finalizer was invented because setToStop() wanted
         * to have finalize() called after step() completed.
         * This is only possible from another thread.
         * So we use the Completion processor to stop the task
         */
        class Finalizer : public EventCompleterInterface
            {
                    PeriodicThread* parent;

                public:
                    Finalizer( PeriodicThread* ct ) : parent( ct )
                    {}

                    void completeEvent()
                    {
                        parent->stop();
                    }
            }

            finalizer;


            bool prepareForExit;

            /**
             * The possible Runnable to run in this Component
             */
            RunnableInterface* runComp;
    };

}

#endif
