/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 13:49:58 CEST 2002  Event.h 

                       Event.hpp -  description
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


#ifndef EVENT_HPP
#define EVENT_HPP

#include "os/fosi.h"
#include "EventInterfaces.hpp"
#include "EventStrategy.hpp"

#include "EventListenerInterface.hpp"
#include "EventCompleterInterface.hpp"

#include <os/MutexLock.hpp>

namespace ORO_CoreLib
{
    // Forward decls to avoid header mess
    class CompletionProcessor;
    using namespace detail;
    /**
     * @brief A most generic event.  
     *
     * If an event is fired, all listeners are notified.
     *
     * @detail We used the strategy pattern and detail namespace to encapsulate the
     * choice of event upon construction. This might seem 'ugly' but is 
     * not wrong in OO craftmansship. The reason for not implementing the
     * functionalities in subclasses is that we wanted flexibility upon creation
     * of the Event (during runtime) but fix it once created.
     * Other implementation suggestions welcome.
     *
     */
    class Event
        : public EventOperationInterface,
          public EventInterface,
          public EventStrategyMethods,
          public EventBoundToListenerInterface,
          protected std::map<EventListenerInterface*, EventCompleterInterface*>
    {
    public :
        /**
         * Construct a new event
         *
         * @param t
         *        The EventType denoting if the Event is fired synchronously or
         *        asynchronously and completed synchronously or asynchronously.
         */
        Event( EventType t );

        /**
         * Construct a new, fully nameserved Event
         *
         * @param t
         *        The EventType denoting if the Event is fired synchronously or
         *        asynchronously and completed synchronously or asynchronously.
         * @param name
         *        The name to be used for nameserving in the EventInterface
         *        and EventOperationInterface.
         */
        Event( EventType t, const std::string& name );

        /**
         * Construct a new, partial nameserved Event
         *
         * @param t
         *        The EventType denoting if the Event is fired synchronously or
         *        asynchronously and completed synchronously or asynchronously.
         * @param opName
         *        The name to be used for nameserving in the 
         *        EventOperationInterface. When opName == string(""), it will not
         *        be nameserved.
         * @param regName
         *        The name to be used for nameserving in the 
         *        EventInterface. When regName == string(""), it will not be
         *        nameserved.
         */
        Event( EventType t, const std::string& opName, const std::string& regName );
        
        /**
         * Destroy an event
         */
        virtual ~Event();

        virtual void fire();

        virtual void addHandler( EventListenerInterface * eli, EventCompleterInterface * eci );

        virtual void removeHandler( EventListenerInterface * eli, EventCompleterInterface* eci );

        protected:

        /**
         * Initializes the Event.
         */
        void init();

        virtual void notifySyn();

        virtual void notifyAsyn();

        virtual void completeSyn( EventListenerInterface* eli );

        virtual void completeAsyn( EventListenerInterface* eli );

        virtual void complete( EventListenerInterface * eli );

    private:

        /**
         * The default constructor should not be used
         */
        Event();

        /**
         * Our Future Completion Processor for queueing completion events
         * and executing them asynchronously
         */
        CompletionProcessor* cp;

        /**
         * Protects access of event method calls
         */
        ORO_OS::Mutex evLock;

        std::vector<EventCallbackStub> evCallback;

    };
}

#endif
