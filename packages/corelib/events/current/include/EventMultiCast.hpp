/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  EventMultiCast.hpp 

                        EventMultiCast.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef EVENTMULTICAST_HPP
#define EVENTMULTICAST_HPP

#include "EventInterfaces.hpp"
#include "EventStrategy.hpp"

namespace ORO_CoreLib
{
    using namespace detail;

    class CompletionProcessor;

    /** 
     * Defines an Event for multicasting the completion of a unique
     * Listener to all subscribed Completers.
     */
    class EventMultiCast
        :public EventMultiCastInterface,
         public EventOperationInterface,
         public CallbackInterface,
         protected EventStrategyMethods
    {
    public:
        EventMultiCast(EventType t);

        virtual ~EventMultiCast();

        void fire();
        
        void complete();
    protected:

        virtual void notifySyn();

        virtual void notifyAsyn();

        virtual void completeSyn( EventListenerInterface* eli );

        virtual void completeAsyn( EventListenerInterface* eli );

        CompletionProcessor* cp;

        EventListenerInterface* myList;
    };
}

#endif
