/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  EventBarrier.cxx 

                        EventBarrier.cxx -  description
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

#include "corelib/EventBarrier.hpp"
#include "corelib/CompletionProcessor.hpp"

namespace ORO_CoreLib
{
    using namespace detail;

    EventBarrier::EventBarrier(EventType t)
        : EventStrategyMethods(t), myComp( Completer::None ), count(0)
    {
        callBacks.reserve(128);
        cp = CompletionProcessor::Instance();
    }

    EventBarrier::~EventBarrier()
    {}

    void EventBarrier::fire()
    {
        count = 0;
        strategy->notify(this);
    }

    void EventBarrier::complete(EventListenerInterface* eli)
    {
        strategy->complete(this,eli);
    }

    void EventBarrier::notifySyn()
    {
        for(unsigned int i=0; i< myList.size(); ++i)
            myList[i]->handleEvent( &callBacks[i] );
    }

    void EventBarrier::notifyAsyn()
    {
        for(unsigned int i=0; i< myList.size(); ++i)
            cp->queue(myList[i], &callBacks[i] );
    }

    void EventBarrier::completeSyn( EventListenerInterface* eli )
    {
        // @todo : instead of counting, keep track of all Listeners.
        ++count;
        if ( count == myList.size() )
            myComp->completeEvent();
    }

    void EventBarrier::completeAsyn( EventListenerInterface* eli )
    {
        ++count;
        if ( count == myList.size() )
            cp->queue(myComp);
    }
}
