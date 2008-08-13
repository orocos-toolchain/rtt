/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:58 CEST 2002  StateInterface.hpp

                        StateInterface.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef EXECUTION_STATEINTERFACE_HPP
#define EXECUTION_STATEINTERFACE_HPP

#ifdef __GNU__
#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif
#endif

#include <string>
#include "ProgramInterface.hpp"
#include "DataSourceBase.hpp"

namespace RTT
{
    /**
     * A State contains an entry, run, handle and exit program.
     *
     * The entry and exit programs will be called when
     * the state is entered of left. The handle program will be called each time
     * the state is requested and no transition is made. The run program will
     * be called before any transition is evaluated.
     *
     * Thus when we are in state A and want to switch to state B, the
     * following happens :
     * @verbatim
     * in State A :
     *   call A->run();
     * if ( transition to B allowed )
     *   call A->onExit();
     *   call B->onEntry();
     *   return true;
     * else
     *   call A->handle();
     *   return false;
     * @endverbatim
     *
     * Error recovery can be handled inside these programs, if even that fails,
     * the programs return false and the state machine containing this state is considered
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
         * Get the run program of this State.
         */
        virtual ProgramInterface* getRunProgram() const = 0;

        /**
         * Get the handle program of this State.
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

        virtual StateInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacementdss ) const = 0;

    };
}



#endif
