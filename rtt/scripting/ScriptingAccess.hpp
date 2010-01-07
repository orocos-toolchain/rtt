/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:57 CEST 2006  ScriptingAccess.hpp

                        ScriptingAccess.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
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


#ifndef ORO_EXECUTION_SCRIPTING_ACCESS
#define ORO_EXECUTION_SCRIPTING_ACCESS

#include <vector>
#include <map>
#include <string>
#include "../rtt-config.h"
#include "../base/ProgramInterface.hpp"
#include "StateMachine.hpp"
#include "../base/DispatchInterface.hpp"
#include "../internal/TaskObject.hpp"

namespace RTT
{ namespace scripting {

    /**
     * This interface allows to load program scripts
     * and state machines and allows execution of code.
     * It keeps track of all loaded scripts of the parent component.
     * It can be found as the \a scripting object of a TaskContext.
     */
    class RTT_API ScriptingAccess
        : public internal::TaskObject
    {
    protected:
        TaskContext* mparent;
    public:
        ScriptingAccess( TaskContext* parent );

        virtual ~ScriptingAccess();

        typedef base::ProgramInterface::Status ProgramStatus;
        typedef scripting::StateMachine::Status StateMachineStatus;

        /**
         * Load a new Program. The Processor takes full ownership and will
         * delete it upon destruction.
         * @throw program_load_exception if a program with the same name already exists.
         */
        bool loadProgram( base::ProgramInterfacePtr pi );

        /**
         * Completely discard a loaded Program.
         * @throw program_unload_exception if the program is
         * not stopped or does not exist.
         * @deprecated by unloadProgram
         */
        bool deleteProgram(const std::string& name);

        /**
         * Unload a program from this processor.
         * @throw program_unload_exception if the program is
         * not stopped or does not exist.
         */
        bool unloadProgram(const std::string& name);

        /**
         * Stop and unload all program scripts.
         */
        void clear();

        /**
         * Return the status of a Program.
         */
        ProgramStatus::ProgramStatus getProgramStatus(const std::string& name) const;

        /**
         * Return the status of a Program as a human readable string.
         */
        std::string getProgramStatusStr(const std::string& name) const;


        /**
         * Get a list of all loaded Programs.
         */
        std::vector<std::string> getProgramList() const;

        /**
         * Get a pointer to a loaded Program.
         */
        const base::ProgramInterfacePtr getProgram(const std::string& name) const;

        base::ProgramInterfacePtr getProgram(const std::string& name);

    public:
        /**
         * Load a new State Machine and all its children.
         * @throw program_load_exception if a state machine with the same name already exists.
         */
        bool loadStateMachine( StateMachinePtr sc );

        /**
         * Unload a deactivated State Machine and all its children.
         * @throw program_unload_exception if the state machine or one of its children is
         * still active.
         */
        bool unloadStateMachine( const std::string& name );

        /**
         * Delete a deactivated State Machine and all its children.
         * @throw program_unload_exception if the state machine or one of its children is
         * still active.
         * @deprecated by unloadStateMachine
         */
        bool deleteStateMachine( const std::string& name );

        /**
         * Return the status of a StateMachine.
         */
        StateMachineStatus::StateMachineStatus getStateMachineStatus(const std::string& name) const;

        /**
         * Return the status of a StateMachine as a human readable string
         */
        std::string getStateMachineStatusStr(const std::string& name) const;

        /**
         * Get a list of all loaded StateMachines and their children.
         */
        std::vector<std::string> getStateMachineList() const;

        /**
         * Get a pointer to a loaded StateMachine.
         */
        const StateMachinePtr getStateMachine(const std::string& name) const;

        /**
         * Get a pointer to a loaded StateMachine.
         */
        StateMachinePtr getStateMachine(const std::string& name);

    protected:
        void recursiveLoadStateMachine( StateMachinePtr sc );
        bool recursiveCheckLoadStateMachine( StateMachinePtr sc );
        void recursiveUnloadStateMachine( StateMachinePtr sc );
        bool recursiveCheckUnloadStateMachine( StateMachinePtr si );

        typedef std::map<std::string,StateMachinePtr> StateMap;
        StateMap   states;
        typedef StateMap::const_iterator StateMapIt;

        typedef std::map<std::string,base::ProgramInterfacePtr> ProgMap;
        ProgMap programs;
        typedef ProgMap::const_iterator ProgMapIt;

    public:
        /**
         * Parse and execute a statement.
         * @param code A single statement to execute.
         * @return A ticket number.
         * @retval -1 if the code could not be parsed.
         * @retval 0 if the code was an expression and could be executed.
         * @retval >0 if the code was a command and is being executed.
         * @see getCommand
         */
        virtual int execute(const std::string& code );

        /**
         * Get the associated command with a given ticket number.
         *
         * @param ticket The ticket number returned by execute().
         *
         * @return An object which represents the command.
         */
        virtual base::DispatchInterface::shared_ptr getCommand( int ticket );

        /**
         * List of executed functions.
         */
        typedef std::vector< base::ProgramInterfacePtr > Functions;

        /**
         * Load and execute a set of functions into the Program Processor for execution
         *
         * @param filename The file to load
         * @param do_throw Set to yes to cause the parse errors to be thrown
         * instead of being printed to the Logger interface.
         *
         * @return A list of functions loaded in the ProgramProcessor.
         */
        virtual Functions loadFunctions(const std::string& filename, bool do_throw );

        /**
         * Load and execute  a set of functions into the Program Processor for execution
         *
         * @param code A string containing the function definitions.
         * @param filename The file name to use in the error messages.
         * @param do_throw Set to yes to cause the parse errors to be thrown
         * instead of being printed to the Logger interface.
         *
         * @return A list of functions loaded in the ProgramProcessor.
         */
        virtual Functions loadFunctions(const std::string& code, const std::string& filename, bool do_throw );

        /**
         * Load a set of programs into the Program Processor for execution.
         * The programs are just loaded, not started yet.
         *
         * @param filename The file to load
         * @param do_throw Set to yes to cause the parse errors to be thrown
         * instead of being printed to the Logger interface.
         *
         * @return true if all state machines could be loaded in the ProgramProcessor.
         */
        virtual bool loadPrograms( const std::string& filename, bool do_throw );

        /**
         * Load a set of programs into the Program Processor for execution.
         * The programs are just loaded, not started yet.
         *
         * @param code A string containing the function definitions.
         * @param filename The file name to use in the error messages.
         * @param do_throw Set to yes to cause the parse errors to be thrown
         * instead of being printed to the Logger interface.
         *
         * @return true if all state machines could be loaded in the ProgramProcessor.
         */
        virtual bool loadPrograms( const std::string& code, const std::string& filename, bool do_throw  );

        /**
         * Unload a program from the Program Processor.
         *
         * @param name The name of the loaded program.
         * @param do_throw Set to yes to cause the unloading errors to be thrown
         * instead of being printed to the Logger interface.
         *
         * @return true if it could be unloaded
         */
        virtual bool unloadProgram( const std::string& name, bool do_throw );

        /**
         * Get the current line of execution of a loaded program script.
         *
         * @param name The name of the program.
         *
         * @return The line number.
         * @retval -1 if the program does not exist.
         * @retval 0 if the program is not running.
         * @retval n the line number if the program is executing.
         */
        virtual int getProgramLine(const std::string& name) const;

        /**
         * Get the original script description of a loaded program.
         *
         * @param name The name of the program.
         *
         * @return A string containing only that program.
         */
        virtual std::string getProgramText(const std::string& name ) const;

        /**
         * Load a set of state machines into the StateMachineProcessor for execution.
         * The state machines are just loaded, not started yet.
         *
         * @param filename The file to load
         * @param do_throw Set to yes to cause the parse errors to be thrown
         * instead of being printed to the Logger interface.
         *
         * @return true if all state machines could be loaded in the StateMachineProcessor.
         */
        virtual bool loadStateMachines( const std::string& filename, bool do_throw );

        /**
         * Load a set of state machines into the StateMachineProcessor for execution.
         * The state machines are just loaded, not started yet.
         *
         * @param code A string containing the function definitions.
         * @param filename The file name to use in the error messages.
         * @param do_throw Set to yes to cause the parse errors to be thrown
         * instead of being printed to the Logger interface.
         *
         * @return true if all state machines could be loaded in the StateMachineProcessor.
         */
        virtual bool loadStateMachines( const std::string& code, const std::string& filename, bool do_throw );

        /**
         * Unload a state machine from the StateMachineProcessor.
         *
         * @param name The name of the loaded program.
         * @param do_throw Set to yes to cause the unloading errors to be thrown
         * instead of being printed to the Logger interface.
         *
         * @return true if it could be unloaded
         */
        virtual bool unloadStateMachine( const std::string& name, bool do_throw );

        /**
         * Get the original script description of a loaded state machine.
         *
         * @param name The name of the state machine.
         *
         * @return A string containing only that state machine.
         */
        virtual std::string getStateMachineText(const std::string& name ) const;

        /**
         * Get the current line of execution of a loaded state machine script.
         *
         * @param name The name of the state machine.
         *
         * @return The line number.
         * @retval -1 if the state machine does not exist.
         * @retval 0 if the state machine is not running.
         * @retval n the line number if the state machine is executing.
         */
        virtual int getStateMachineLine(const std::string& name ) const;

        /**
         * Get the current state of an activated state machine.
         *
         * @param name The name of the state machine
         *
         * @return The current state.
         */
        virtual std::string getCurrentState(const std::string& name ) const;

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
    protected:
        StatementProcessor* sproc;
        bool doExecute(const std::string& code);

        bool doLoadPrograms( const std::string& filename );
        bool doLoadProgramText( const std::string& code );
        bool doUnloadProgram( const std::string& name );

        bool doLoadStateMachines( const std::string& filename );
        bool doLoadStateMachineText( const std::string& code );
        bool doUnloadStateMachine( const std::string& name );

        void createInterface(void);
    };
}}

#endif
