/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ListenerRegistration.hpp 

                        ListenerRegistration.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 

#ifndef LISTENERREGISTRATION_HPP
#define LISTENERREGISTRATION_HPP

#include "EventListenerInterface.hpp"
#include "EventCompleterInterface.hpp"
#include "EventInterfaces.hpp"

namespace ORO_CoreLib
{

    /**
     * This class/interface defines how EventListener and EventCompleter
     * can be added or removed for notification
     *
     * @deprecated
     */
    class ListenerRegistration
    {

        public:

            /**
             * Construct an object which allows adding and removing listeners
             * to an event.
             * @param ei The event to encapsulate
             */
            ListenerRegistration( EventInterface* ei )
                    : target( ei )
            {}

            /**
             * Add an EventListener and a Completer which will be notified
             * when this event fires.
             * 
             * @pre  <EventListener> is effective
             * @post <EventListener> will be notified when this event fires.
             * @post <EventCompleter> will be notified when it is effective
             *       and the event is completed
             */
            virtual void addHandler( EventListenerInterface *eli, EventCompleterInterface *eci )
            {
                target->addHandler( eli, eci );
            }

            /**
             * Remove an EventListener from this Event
             *
             * @post <EventListener> is not notified when this event fires.
             */
            virtual void removeHandler( EventListenerInterface *eli, EventCompleterInterface* eci )
            {
                target->removeHandler( eli, eci );
            }

            virtual EventInterface* eventGet() const
            {
                return target;
            }

        private:
            /**
             * The event that we contain
             */
            EventInterface* target;

            // forbidden
            ListenerRegistration()
            {}

    }

    ;

}

#endif


