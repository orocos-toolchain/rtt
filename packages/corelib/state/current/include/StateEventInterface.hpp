/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  StateEventInterface.hpp 

                        StateEventInterface.hpp -  description
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
 
 
#ifndef STATEEVENTINTERFACE_HPP
#define STATEEVENTINTERFACE_HPP

#include "EventInterfaces.hpp"
#include "StateInterface.hpp"

namespace ORO_CoreLib
{

    /**
     * A StateEventInterface object extends the StateInterface with
     * Events that will be fired on each state transition. As a consequence,
     * anyone can tie its own handles to the state machine.
     */
    class StateEventInterface
        : public StateInterface
    {
        /**
         * Returns the Event which will be fired on state entry.
         */
        virtual EventInterface* entryEventGet() = 0;

        /**
         * Returns the Event which will be fired on state handling.
         */
        virtual EventInterface* handleEventGet() = 0;

        /**
         * Returns the Event which will be fired on state exit.
         */
        virtual EventInterface* exitEventGet() = 0;
    };
}


#endif
