/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:44:18 CEST 2002  ComponentActive.hpp 

                       ComponentActive.hpp -  description
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



#ifndef COMPONENTTHREADED_H
#define COMPONENTTHREADED_H

#include "os/cpp.hpp"
#include <os/fosi.h>
#include "ComponentActiveInterface.hpp"
#include "RunnableInterface.hpp"

namespace ORO_OS
{

    /**
      *
      *        An active component is a threaded component
      *
      *        The periodicity is the time between the starting
      *        of two runs.
      */

    class ComponentThreaded : public ComponentActiveInterface
    {
            friend void* ComponentThread( void* t );
            RunnableInterface* runner;

        public:
            ComponentThreaded(int priority, const string& name="", RunnableInterface* r = 0 );

            virtual ~ComponentThreaded();
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
            int periodSet( secs s, nsecs ns );
            void periodGet( secs& s, nsecs& ns ) const;
            /**
             * Get the periodicity in seconds
             */
            double periodGet() const;
            /**
             * Returns whether the thread is running
             */
            inline bool isRunning() const;

            inline int setToTerminate();
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

        bool makeHardRealtime() {return true;}
        bool makeSoftRealtime() {return false;}
        bool isHardRealtime()   {return true;}

        protected:
            /**
             * This method will be executed in each period once,
             * when isRunning() == 1
             */
            virtual void step();

            virtual bool initialize();

            virtual void finalize();

            virtual void setToStop();
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
            int exitNow;
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
            char taskName[ 32 ];
    };

};

#endif
