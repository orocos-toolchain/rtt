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
 
#ifndef EXECUTION_STATEINTERFACE_HPP
#define EXECUTION_STATEINTERFACE_HPP

#ifdef __GNU__
#pragma interface
#endif

#include <string>
#include "ProgramInterface.hpp"

namespace ORO_Execution
{
    /**
     * A State contains an entry, handle and exit program.
     *
     * OnEntry() and onExit() will be called when
     * the state is entered of left. handle() will be called each time
     * (thus also the first time) the state is requested.
     *
     * Thus when we are in state A and want to switch to state B, the
     * following happens : 
     * @verbatim
     * in A :
     * if ( transition to B allowed )
     *   call A->onExit();
     *   call B->onEntry();
     *   call B->handle();
     *   return true;
     * else 
     *   return false;
     * @endverbatim
     *
     * If another request for state B is done, the next call is:
     * @verbatim
     * call B->handle();
     * @endverbatim
     * Error recovery can be handled inside these functions, if even that fails,
     * the functions return false and the state machine containing this state is considered
     * in error.
     */
    class StateInterface
    {
    public:

        virtual ~StateInterface()
        {}

        /**
         * @brief Get the name of this state.
         */
        virtual const std::string& getName() const = 0;

        /**
         * Get the entry program of this State.
         */
        virtual ProgramInterface* getEntryProgram() const = 0;

        /**
         * Get the exit program of this State.
         */
        virtual ProgramInterface* getHandleProgram() const = 0;

        /**
         * Get the exit program of this State.
         */
        virtual ProgramInterface* getExitProgram() const = 0;

        /**
         * Get the beginning definition of this State.
         */
        virtual int getEntryPoint() const = 0;
    };
}



#endif
