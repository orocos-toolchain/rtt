/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  StateEventInterface.hpp 

                        StateEventInterface.hpp -  description
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
