/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  EventSimple.hpp 

                        EventSimple.hpp -  description
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
 
#ifndef EVENTSIMPLE_HPP
#define EVENTSIMPLE_HPP

#include "EventInterfaces.hpp"
#include "EventStrategy.hpp"

namespace ORO_CoreLib
{
    using namespace detail;

    class CompletionProcessor;

    /**
     * Defines an Event in its most simple form. It has one Listener
     * and one Completer.
     */
    class EventSimple
        : public EventSimpleInterface,
          public EventOperationInterface,
          public CallbackInterface,
          protected EventStrategyMethods
    {
    public:
        EventSimple(EventType t);

        virtual ~EventSimple();

        void fire();

        void complete();

    protected:

        virtual void notifySyn();

        virtual void notifyAsyn();

        virtual void completeSyn( EventListenerInterface* eli );

        virtual void completeAsyn( EventListenerInterface* eli );

        EventListenerInterface* myList;
        EventCompleterInterface* myComp;
        CompletionProcessor* cp;
    };
}

#endif
