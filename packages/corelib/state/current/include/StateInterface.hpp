/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  StateInterface.hpp 

                        StateInterface.hpp -  description
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
 
#ifndef STATEINTERFACE_HPP
#define STATEINTERFACE_HPP

#ifdef __GNU__
#pragma interface
#endif

namespace ORO_CoreLib
{
    /**
     * @brief An object which implements StateInterface can be used to handle
     * state specific actions. 
     *
     * OnEntry() and onExit() will be called when
     * the state is entered of left. handle() will be called each time
     * (thus also the first time) the state is requested.
     *
     * Thus when we are in state A and want to switch to state B, the
     * following happens : 
     * @verbatim
     * call A->onExit();
     * call B->onEntry();
     * call B->handle();
     * @endverbatim
     *
     * If another request for state B is done, the next call is:
     * @verbatim
     * call B->handle();
     * @endverbatim
     */
    class StateInterface
    {
    public:

        virtual ~StateInterface()
        {}
        /**
         * @brief This method contains the functionality which must be executed
         * on state entry.
         */
        virtual void onEntry() = 0;

        /**
         * @brief This method contains the functionality which must be executed
         * each time the state is requested.
         */
        virtual void handle() = 0;

        /**
         * @brief This method contains the functionality which must be executed
         * each time the state is left.
         */
        virtual void onExit() = 0;
    };
}



#endif
