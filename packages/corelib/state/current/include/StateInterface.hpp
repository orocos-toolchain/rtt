/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  StateInterface.hpp 

                        StateInterface.hpp -  description
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
 
 
#ifndef STATEINTERFACE_HPP
#define STATEINTERFACE_HPP

#ifdef __GNU__
#pragma interface
#endif

namespace ORO_CoreLib
{
    /**
     * An object which implements StateInterface can be used to handle
     * state specific actions. OnEntry() and onExit() will be called when
     * the state is entered of left. handle() will be called each time
     * (thus also the first time) the state is requested.
     *
     * Thus when we are in state A and want to switch to state B, the
     * following happens : 
     * call A->onExit();
     * call B->onEntry();
     * call B->handle();
     * 
     * If another request for state B is done, the next call is:
     * call B->handle();
     */
    class StateInterface
    {

        public:

        virtual ~StateInterface()
        {}
        /**
         * This method contains the functionality which must be executed
         * on state entry.
         */
        virtual void onEntry() = 0;

        /**
         * This method contains the functionality which must be executed
         * each time the state is requested.
         */
        virtual void handle() = 0;

        /**
         * This method contains the functionality which must be executed
         * each time the state is left.
         */
        virtual void onExit() = 0;
    };
}



#endif
