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

#include "os/PeriodicThread.hpp"
#include "EventInterfaces.hpp"
#include "EventPeriodic.hpp"
#include "os/Mutex.hpp"
#include "os/MutexLock.hpp"

#include <list>

namespace ORO_CoreLib
{
    using ORO_OS::MutexLock;

    /**
     * This PeriodicThread is the base class for the ZeroTimeThread
     * and the ZeroLatencyThread.
     *
     * You can not instantiate it.
     *
     * @see PeriodicThread
     */
    class TaskExecution
        : public ORO_OS::PeriodicThread
    {
        /**
         * Friends which can call our constructor
         */
        friend class ZeroTimeThread;
        friend class ZeroLatencyThread;
        friend class NonRealTimeThread;

        /**
         * A structure to keep track of ownership
         * of events.
         */
        struct EventItem
        {
            EventItem(EventPeriodic* ev, bool _owner = false) : event(ev), owner(_owner) {}
            EventPeriodic* event;
            bool owner;

            struct Locator : public std::binary_function<EventItem, EventPeriodic*, bool>
            {
                bool operator()(const EventItem& p, const EventPeriodic* ev) const
                {
                    return p.event == ev;
                }
            };
        };

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
         *        The periodicity of this thread in seconds (e.g. 0.001 = 1000Hz )
         */
        TaskExecution(int priority, const std::string& name, double periodicity);

        virtual void step();

        /**
         * Add a EventListenerInterface which is handled each n nanoseconds
         * 
         * @param t The task to handle each n nanoseconds
         * @param n handle every n nanoseconds
         */
        bool taskAdd( EventListenerInterface* t, const nsecs n );

        /**
         * Remove a EventListenerInterface from handleing
         *
         * @post <t> is no longer handled by this thread
         */
        void taskRemove( EventListenerInterface* t );
        
        /**
         * Internal method for keeping track of EventPeriodic
         * ownership.
         */
        void doEventAdd( EventPeriodic* ev, bool myEvent);

        /**
         * A list containing all the EventPeriodic instances
         *  we must fire
         *
         * @see EventInterface
         */ 
        std::list<EventItem> clocks;

        ORO_OS::Mutex lock;
    };
} // namespace ORO_CoreLib

#endif
