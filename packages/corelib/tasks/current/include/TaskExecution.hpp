/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:36 CEST 2002  TaskExecution.h 

                       TaskExecution.h -  description
                          -------------------
   begin                : Wed April 17 2002
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



#ifndef TASKEXECUTION_HPP
#define TASKEXECUTION_HPP

#include "os/ComponentThreaded.hpp"
#include "EventInterfaces.hpp"
#include "EventPeriodic.hpp"
#include "os/Mutex.hpp"
#include "os/MutexLock.hpp"

#include <list>

namespace ORO_CoreLib
{
    using ORO_OS::MutexLock;

    /**
     * This ComponentThreaded is the base class for the ZeroTimeThread
     * and the ZeroLatencyThread.
     *
     * You can not instantiate it.
     *
     * @see ComponentThreaded
     */
    class TaskExecution
        : public ORO_OS::ComponentThreaded
    {
        /**
         * Friends which can call our constructor
         */
        friend class ZeroTimeThread;
        friend class ZeroLatencyThread;
        friend class NonRealTimeThread;

    public:

        /**
         * Destructor
         */
        virtual ~TaskExecution();

        /**
         * Add an Event that will be fired every execution period
         */
        void eventAdd( EventPeriodic* );

        /**
         * Remove an Event from being fired every execution period
         */
        void eventRemove( EventPeriodic* );
    protected:
        /**
         * Constructor. To be called from the friend classes.
         *
         * @param priority 
         *        The priority of this thread
         * @param periodicity
         *        The periodicity of this thread
         */
        TaskExecution(int priority, const std::string& name, double periodicity);

        virtual void step();

        /**
         * Add a EventListenerInterface which is handled each n nanoseconds
         * 
         * @param t The task to handle each n nanoseconds
         * @param n handle every n nanoseconds
         */
        bool taskAdd( EventListenerInterface* t, const nsecs n )
        {
            { 
                // scoped mutexlock with the for loop
                std::list<EventPeriodic*>::iterator itl;
                MutexLock locker(lock);
                for (itl = clocks.begin(); itl != clocks.end(); ++itl)
                    if ( *itl && (*itl)->periodGet() == n )
                        {
                            (*itl)->addHandler(t, Completer::None );
                            return true;
                        }
            }
#if OROSEM_CORELIB_TASKS_DYNAMIC_REG
            /**
             * Create the event when not existing.
             */
            EventPeriodic* ep = new EventPeriodic( n );
            ep->addHandler(t, Completer::None);
            eventAdd(ep);
            return true;
#else
            return false;
#endif
        }

        /**
         * Remove a EventListenerInterface from handleing
         *
         * @post <t> is no longer handled by this thread
         */
        void taskRemove( EventListenerInterface* t )
        {
            // this is doing it the hard way.
            std::list<EventPeriodic*>::iterator itl;
            MutexLock locker(lock);
            for (itl = clocks.begin(); itl != clocks.end(); ++itl)
                if ( *itl )
                    (*itl)->removeHandler(t,0);
        }
        
        /**
         * A list containing all the EventPeriodic instances
         *  we must fire
         *
         * @see EventInterface
         */ 
        std::list<EventPeriodic*> clocks;

        ORO_OS::Mutex lock;
    };
} // namespace ORO_CoreLib

#endif
