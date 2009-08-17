/***************************************************************************
  tag: FMTC  do nov 2 13:06:06 CET 2006  ExecutionAccess.hpp

                        ExecutionAccess.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_EXECUTION_ACCESS_HPP
#define ORO_EXECUTION_ACCESS_HPP

#include <string>
#include "rtt-config.h"
#include "../rtt-fwd.hpp"

namespace RTT
{ namespace interface {

    /**
     * @brief The ExecutionAccess provides access to programs
     * and state machines loaded in the execution engine.
     * It can be found as the \a engine object of a TaskContext.
     */
    class RTT_API ExecutionAccess
    {
    protected:
        TaskContext* tc;

    public:
        ExecutionAccess( TaskContext* parent );

        virtual ~ExecutionAccess();

        /**
         * @name Script Program Commands
         * @{
         */
        /**
         * Start a Program.
         * @param name The name of the Program.
         */
        bool startProgram(const std::string& name);

        /**
         * Stops the execution of a program.
         * @param name The name of the Program.
         */
        bool stopProgram(const std::string& name);

        /**
         * Pauses the execution of a running program.
         * @param name The name of the Program.
         */
        bool pauseProgram(const std::string& name);

        /**
         * Steps a single instruction of a paused program.
         * @param name The name of the Program.
         */
        bool stepProgram(const std::string& name);
        /**
         *@}
         */

        /**
         * @name Script Program Methods
         * @{
         */
        /**
         * Check if a program is loaded
         *
         * @param name The name of the program.
         *
         * @return True if so.
         */
        virtual bool hasProgram(const std::string& name) const;

        /**
         * Inspect if a loaded program is in the running state
         *
         * @param name The name of the Program.
         *
         * @return true if so.
         */
        bool isProgramRunning(const std::string& name) const;

        /**
         * Inspect if a loaded program is in the paused state
         *
         * @param name The name of the Program.
         *
         * @return true if so.
         */
        bool isProgramPaused(const std::string& name) const;

        /**
         * Inspect if a loaded program is in the error state
         *
         * @param name The name of the Program.
         *
         * @return true if so.
         */
        bool inProgramError(const std::string& name) const;
        /**
         *@}
         */

        /**
         * @name Script State Machine Commands
         * @{
         */
        /**
         * Activate a previously loaded StateMachine.
         * @param name The name of the StateMachine.
         */
        bool activateStateMachine(const std::string& name);

        /**
         * Deactivate a stopped StateMachine.
         * @param name The name of the StateMachine.
         */
        bool deactivateStateMachine(const std::string& name);

        /**
         * Start a previously activated StateMachine.
         * @param name The name of the StateMachine.
         */
        bool startStateMachine(const std::string& name);

        /**
         * Pause a previously activated StateMachine.
         * @param name The name of the StateMachine.
         */
        bool pauseStateMachine(const std::string& name);

        /**
         * Stops the execution of a scripting::StateMachine and enters the Final_State.
         * @param name The name of the StateMachine.
         */
        bool stopStateMachine(const std::string& name);

        /**
         * Resets the execution of a scripting::StateMachine and enters the Initial_State.
         * @param name The name of the StateMachine.
         */
        bool resetStateMachine(const std::string& name);

        /**
         * Request a state change in a state machine.
         * @param name The name of the StateMachine.
         * @param state The state to enter.
         * @return true if the state change request was accepted.
         */
        bool requestStateMachineState(const std::string& name, const std::string& state);
        /**
         *@}
         */

        /**
         * @name Script State Machine Methods
         * @{
         */
        /**
         * Check if a state machine is loaded.
         *
         * @param name The name of the state machine.
         *
         * @return True if so.
         */
        virtual bool hasStateMachine(const std::string& name) const;

        /**
         * Returns true if the state machine has been activated.
         * @param name The name of the StateMachine.
         */
        bool isStateMachineActive(const std::string& name) const;

        /**
         * Returns true if the state machine was successfully started.
         * @param name The name of the StateMachine.
         */
        bool isStateMachineRunning(const std::string& name) const;

        /**
         * Returns true if the state machine is in error.
         * @param name The name of the StateMachine.
         */
        bool inStateMachineError(const std::string& name) const;

        /**
         * Returns true if the state machine is paused.
         * @param name The name of the StateMachine.
         */
        bool isStateMachinePaused(const std::string& name) const;

        /**
         * Get the current state of a state machine.
         * @param name The name of the StateMachine.
         */
        const std::string& getStateMachineState(const std::string& name) const;

        /**
         * Check if a state machine is in a given state
         * @param name The name of the StateMachine.
         * @param state The state to enter.
         * @return true if so.
         */
        bool inStateMachineState(const std::string& name, const std::string& state) const;
        /**
         *@}
         */

        bool true_gen() const;
    };

}}

#endif
