/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  StateEvent.hpp 

                        StateEvent.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
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
 
 
#ifndef STATEEVENT_HPP
#define STATEEVENT_HPP

#include "StateEventInterfaces.hpp"


namespace ORO_CoreLib
{

    /**
     * A State which fires events on each state transition.
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
