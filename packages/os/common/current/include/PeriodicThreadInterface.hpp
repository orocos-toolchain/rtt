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



#ifndef COMPONENT_ACTIVE_INTERFACE_HPP
#define COMPONENT_ACTIVE_INTERFACE_HPP

#include "os/fosi.h"
#include "os/RunnableInterface.hpp"

namespace ORO_OS
{
    /**
      *
      *        An active component is a Component which is being run periodically.
      *
      *        The periodicity is the time between the starting
      *        of two runs.
      */
    class PeriodicThreadInterface :
                protected RunnableInterface
    {

        public:
            virtual ~PeriodicThreadInterface()
            {}

            typedef double Seconds;
            typedef long secs;
            typedef long msecs;
            typedef long long nsecs;

            /**
             * Start the component.
             *
             * @post    initialize() is called first
             * @post    The Component is running
             * @return  true if the function did succeed.
             *       false otherwise.
             */
            virtual bool start() = 0;

            /**
             * Stop the component.
             *
             * @post    The Component is no longer being run
             * @post    finalize() is called when the Component is stopped.
             * @return  true if the function did succeed.
             *          false otherwise.
             */
            virtual bool stop() = 0;

            /**
             * Set the periodicity of this component
             * in seconds
             *
             * @pre     The Component is not running
             */
            virtual int periodSet( Seconds s ) = 0;

            /**
             * Set the periodicity of this component
             * (seconds, nanoseconds).
             *
             * @pre     The Component is not running
             */
            virtual int periodSet( secs s, nsecs ns ) = 0;

            /**
             * Get the periodicity of this component
             * (seconds,nanoseconds).
             *
             */
            virtual void periodGet( secs& s, nsecs& ns ) const = 0 ;

            /**
             * Get the periodicity in Seconds
             */
            virtual Seconds periodGet() const = 0 ;

            /**
             * Returns whether the component is running
             */
            virtual inline bool isRunning() const = 0 ;

            /**
             * Set the name of this task
             */
            virtual void taskNameSet( const char* ) = 0;

            /**
             * Read the name of this task
             */
            virtual const char* taskNameGet() const = 0 ;

            /**
             * This method terminates the PeriodicThread in a safe
             * way. After the last step(), finalize will be called
             * and the component cannot be restarted.
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
        
        friend class RunnableInterface;

            /**
             * Signal stopping to the Component. This can be called
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
