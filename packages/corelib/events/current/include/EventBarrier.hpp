/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  EventBarrier.hpp 

                        EventBarrier.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
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
 
#ifndef EVENTBARRIER_HPP
#define EVENTBARRIER_HPP

#include "EventInterfaces.hpp"
#include "EventStrategy.hpp"

namespace ORO_CoreLib
{
    using namespace detail;

    class CompletionProcessor;

    /**
     * Defines an Event for synchronisation purposes. The Completer will
     * only be called when all Listeners completed the Event. In this
     * way, the Completers execution is on hold until all Listeners
     * have signalled completion.
     */ 
    class EventBarrier
        : public EventBarrierInterface,
          public EventOperationInterface,
          public EventBoundToListenerInterface,
          protected EventStrategyMethods
    {
    public:
        EventBarrier(EventType t);

        virtual ~EventBarrier();

        void fire();

        virtual void listenerAdd( EventListenerInterface* eli )
        {
            EventBarrierInterface::listenerAdd(eli);
            callBacks.push_back(EventCallbackStub(this,eli));
        }

        virtual void listenerRemove( EventListenerInterface* eli )
        {
            EventBarrierInterface::listenerRemove(eli);
            callBacks.erase( find_if( callBacks.begin(), callBacks.end(), bind2nd(EventCallbackStub::FindStub(),eli) ) );
        }

        void complete( EventListenerInterface* eli );

    protected:
        virtual void notifySyn();

        virtual void notifyAsyn();

        virtual void completeSyn( EventListenerInterface* eli );

        virtual void completeAsyn( EventListenerInterface* eli );

        CompletionProcessor* cp;

        EventCompleterInterface* myComp;
        
        unsigned int count;

        std::vector<EventCallbackStub> callBacks;
    };
}

#endif
