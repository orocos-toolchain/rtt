/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  StateEvent.hpp 

                        StateEvent.hpp -  description
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
 
#ifndef STATEEVENT_HPP
#define STATEEVENT_HPP

#include "StateEventInterface.hpp"


namespace ORO_CoreLib
{

    /**
     * @brief A State which fires events on each state transition.
     *
     * The functionality must be provided in the EventListenerInterface
     * and EventCompleterInterface instances registered to the
     * Events.
     */
    class StateEvent
        : public StateEventInterface
    {

    public:

        /**
         * Constructs a StateEvent with all events of the SYNASYN EventType.
         */
        StateEvent()
            : entryEvent( Event::SYNASYN ),
              handleEvent( Event::SYNASYN ),
              exitEvent( Event::SYNASYN ) {}


        void onEntry()
        {
            entryEvent.fire();
        }

        void handle()
        {
            handleEvent.fire();
        }

        void onExit()
        {
            exitEvent.fire();
        }


        EventInterface* entryEventGet()
        {
            return & entryEvent;
        }

        EventInterface* handleEventGet()
        {
            return & handleEvent;
        }

        EventInterface* exitEventGet()
        {
            return & exitEvent;
        }

    protected:
        /**
         * The Event being fired on entry.
         */
        Event entryEvent;
        /**
         * The Event being fired on handling.
         */
        Event handleEvent;
        /**
         * The Event being fired on exit.
         */
        Event exitEvent;
    };
}



#endif
