/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  EventEmergencyStop.hpp 

                        EventEmergencyStop.hpp -  description
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
