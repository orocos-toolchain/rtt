/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  EventSimple.cxx 

                        EventSimple.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

#include "corelib/EventSimple.hpp"
#include "corelib/CompletionProcessor.hpp"

namespace ORO_CoreLib
{
    using namespace detail;

        EventSimple::EventSimple(EventType t)
            : EventStrategyMethods(t), myList( Listener::None ), myComp( Completer::None )
        {
            cp = CompletionProcessor::Instance();
        }

        EventSimple::~EventSimple()
        {}

        void EventSimple::fire()
        {
            strategy->notify(this);
        }

        void EventSimple::complete()
        {
            strategy->complete(this, myList);
        }

        void EventSimple::notifySyn()
        {
            myList->handleEvent(this);
        }

        void EventSimple::notifyAsyn()
        {
            cp->queue( myList, this );
        }

        void EventSimple::completeSyn( EventListenerInterface* eli )
        {
            myComp->completeEvent();
        }

        void EventSimple::completeAsyn( EventListenerInterface* eli )
        {
            cp->queue( myComp );
        }
}
