/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 13:45:11 CEST 2002  EventInterface.h 

                       EventInterface.h -  description
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


#ifndef EVENTINTERFACES_HPP
#define EVENTINTERFACES_HPP

#include "NameServer.hpp"
#include "NameServerRegistrator.hpp"
#include "EventListenerInterface.hpp"
#include "EventCompleterInterface.hpp"
#include "CallbackInterface.hpp"

#include <vector>

namespace ORO_CoreLib
{

    /**
     * EventOperationInterface describes that an Event can be fire()`d
     */
    class EventOperationInterface
        : protected NameServerRegistrator<EventOperationInterface*>
    {
    public:
        /**
         * Create a not nameserved EventOperationInterface instance.
         */
        EventOperationInterface() {}

        /**
         * Create a nameserved EventOperationInterface instance.
         * 
         * @param name
         *        The name for this instance
         */
        EventOperationInterface(const std::string& name)
            : NameServerRegistrator<EventOperationInterface*>(nameserver, name, this) {}

        /**
         * Destroy the EventOperationInterface instance and remove
         * it from the nameserver if needed.
         */
        virtual ~EventOperationInterface() {}

        /**
         * Activate an Event, signal to all subscribers, according to 
         * the Events policy that it has happened.
         */
        virtual void fire() = 0;

        /**
         * This NameServer keeps track of all EventOperationInterface
         * instances.
         *
         * @see NameServer
         */
        static NameServer<EventOperationInterface*> nameserver;
    };

    /**
     * An Event with a single Listener
     */
    class EventSingleListenerInterface
    {
    public:
        /**
         * Set the unique EventListenerInterface instance which will be
         * notified when this Event fires.
         *
         * @param eli
         *        The unique listener for this event.
         * @return true if the operation succeeds, false otherwise
         */
         bool listenerSet(EventListenerInterface* eli)
        {
            if (myList == 0) { myList = eli; return true;}
            return false;
        }

        /**
         * Remove the unique EventListenerInterface instance from being
         * notified when this Event fires.
         *
         * @param eli
         *        The unique listener to be removed from this event.
         * @return true if the operation succeeds, false otherwise
         */
         bool listenerRemove(EventListenerInterface* eli)
        {
            if (myList == eli) { myList = 0; return true;}
            return false;
        }

    protected:
        EventListenerInterface* myList;
    };

    /**
     * An Event with a single Completer.
     * A Completer is always optionally.
     */
    class EventSingleCompleterInterface
    {
    public:
        /**
         * Set the unique EventCompleterInterface instance which will be
         * notified when this Event completes.
         *
         * @param eci
         *        The unique completer for this event.
         * @return true if the operation succeeds, false otherwise
         */
         bool completerSet(EventCompleterInterface* eci)
        {
            if (myComp == 0) { myComp = eci; return true;}
            return false;
        }

        /**
         * Remove the unique EventCompleterInterface instance from being
         * notified when this Event fires.
         *
         * @param eci
         *        The unique completer to be removed from this event.
         * @return true if the operation succeeds, false otherwise
         */
         bool completerRemove(EventCompleterInterface* eci)
        {
            if (myComp == eci) { myComp = 0; return true;}
            return false;
        }

    protected:
        EventCompleterInterface* myComp;
    };

    /**
     * A class for defining an Event with many listeners.
     */
    class EventMultiListenerInterface
    {
    public:
        /**
         * Add a listener for this Event.
         *
         * @param eli
         *        The listener to be added to this event.
         */
        void listenerAdd(EventListenerInterface* eli)
        {
            myList.push_back(eli);
        }

        /**
         * Remove a listener from this Event.
         *
         * @param eli
         *        The listener to be removed from this event.
         */
        void listenerRemove(EventListenerInterface* eli)
        {
            std::vector<EventListenerInterface*>::iterator itl= std::find(myList.begin(), myList.end(), eli);
            if (itl != myList.end() )
                {
                    myList.erase(itl);
                }
        }

    protected:
        std::vector<EventListenerInterface*> myList;
    };

    /**
     * A class for defining an Event with many completers
     */
    class EventMultiCompleterInterface
    {
    public:
        /**
         * Add a completer for this Event.
         *
         * @param eci
         *        The completer to be added to this event.
         */
        void completerAdd(EventCompleterInterface* eci)
        {
            myComp.push_back(eci);
        }

        /**
         * Remove a completer from this Event.
         *
         * @param eci
         *        The completer to be removed from this event.
         */
         void completerRemove(EventCompleterInterface* eci)
        {
            std::vector<EventCompleterInterface*>::iterator itl= std::find(myComp.begin(), myComp.end(), eci);
            if (itl != myComp.end() )
                {
                    myComp.erase(itl);
                }
        }

    protected:
        std::vector<EventCompleterInterface*> myComp;
    };

    /**
     * An Interface for Completion on a per Listener basis.
     * The Completion is bound to a specific Listener.
     */
    struct EventBoundToListenerInterface
    {
        /**
         * Call the Completer associated with a Listener.
         *
         * @param eli
         *        The Listener which handling must be completed by a Completer.
         */
        virtual void complete(EventListenerInterface* eli) = 0;
    };

    /**
     * A class for defining an Event with multiple (EventListenerInterface,
     * EventCompleterInterface) Event handling pairs.
     */
    class HandlerRegistrationInterface
    {
    public:
        /**
         * Add an event handler (listener,completer) pair to this Event.
         *
         * @param eli
         *        The listener to be added to this event.
         * @param eci
         *        The completer to be added to this event. It will be called
         *        when <eli> comletes the Event.
         */
        virtual void addHandler(EventListenerInterface* eli, EventCompleterInterface* eci) = 0;
        
        /**
         * Remove a (listener,completer) pair from this Event.
         *
         * @param eli
         *        The listener to be removed from this event.
         * @param eci
         *        The completer to be removed from this event. It must be the
         *        same completer as given in the addHandler() method.
         */
        virtual void removeHandler(EventListenerInterface* eli, EventCompleterInterface* eci) = 0;

    };

    /**
     * An EventCallbackStub is handed over to the EventListenerInterface
     * handleEvent() method to allow completion of a Listener when
     * its callback calling (or not calling) must be tracked by the Event.
     * It keeps track of which Listener has to be completed involving
     * which Event.
     */
    class EventCallbackStub
        : public CallbackInterface
    {
        EventBoundToListenerInterface* mpi;
        EventListenerInterface*  li;
    public:
        EventCallbackStub(EventBoundToListenerInterface* empi, EventListenerInterface* eli)
            : mpi(empi), li(eli) {}

        virtual void complete()
        {
            mpi->complete( li );
        }
        
        EventBoundToListenerInterface* event() const
        {
            return mpi;
        }

        EventListenerInterface* listener() const
        {
            return li;
        }

        /**
         * A Functor which helps finding the EventCallbackStub which contains a certain
         * EventListenerInterface instance.
         */
        struct FindStub : public std::binary_function<EventCallbackStub,EventListenerInterface*, bool>
        {
            bool operator()( const EventCallbackStub& ecb, const EventListenerInterface* eli) const
            {
                return ecb.listener() == eli;
            }
        };
    };

    /**
     * A most generic event with multiple listeners and completers.
     * The EventInterface extends the HandlerRegistrationInterface with
     * a NameServer. One can thus find an Event and register as
     * Listener,Completer through the nameserver.
     */
    class EventInterface 
        : public HandlerRegistrationInterface,
          private NameServerRegistrator<EventInterface*>
    {
    public:

        /**
         * Create a nameless EventInterface.
         */
        EventInterface()
        {}

        /**
         * Create an EventInterface which is nameserved.
         *
         * @param name
         *        The name which should be used for nameserving
         */
        EventInterface( const std::string& name )
            : NameServerRegistrator<EventInterface*>( nameserver, name, this )
        {}

        /**
         * Destroy the EventInterface. If it was nameserved,
         * it will be removed from the nameserver.
         */
        virtual ~EventInterface()
        {}

        /**
         * The NameServer for EventInterface instances.
         */
        static NameServer<EventInterface*> nameserver;
    };

    /**
     * An EventSimpleInterface instance has one Listener
     * and one Completer. It will notify the Listener when fired
     * and the Completer when completion is required by the
     * Listener.
     */
    struct EventSimpleInterface
        : public EventSingleListenerInterface,
          public EventSingleCompleterInterface
    {
    };

    /**
     * Once fired, an EventMultiCastInterface signals the completion
     * of the unique Listener to all subscribed Completers.
     */
    struct EventMultiCastInterface
        : public EventSingleListenerInterface,
          public EventMultiCompleterInterface
    {
    };

    /**
     * Once fired, an EventBarrierInterface waits for completion
     * of all subscribed Listeners before notifying the unique Completer.
     */
    struct EventBarrierInterface
        : public EventMultiListenerInterface,
          public EventSingleCompleterInterface
    {
    };


}

#endif
