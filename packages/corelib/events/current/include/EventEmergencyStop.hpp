/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  EventEmergencyStop.hpp 

                        EventEmergencyStop.hpp -  description
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
 
#ifndef EVENTEMERGENCYSTOP_HPP
#define EVENTEMERGENCYSTOP_HPP

#include"Event.hpp"

namespace ORO_CoreLib
{
    /**
     * This event can be used as a cascading event.
     * It is itself a Listener and a Completer, so that it
     * fires itself when the methods of the
     * EventListenerInterface and EventCompleterInterface are called
     */
    class EventEmergencyStop 
        : public Event, 
          public EventListenerInterface,
          public EventCompleterInterface
    {
    public:
        EventEmergencyStop(EventType t, const std::string name="") : Event(t,name) {}
        virtual void handleEvent( CallbackInterface* cb)
        {
            fire();
            cb->complete();
        }

        virtual void completeEvent()
        {
            complete(this);
        }
    };
}

#endif
