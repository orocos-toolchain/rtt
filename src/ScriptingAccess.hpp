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
#include <string>
#include "ProgramInterface.hpp"
#include "DispatchInterface.hpp"

namespace RTT
{
    class TaskContext;

    /**
     * This interface allows to load program scripts
     * and state machines and allows execution of code.
     * It can be found as the \a scripting object of a TaskContext.
     */
    class ScriptingAccess
    {
    protected:
        TaskContext* mparent;
    public:
        ScriptingAccess( TaskContext* parent )
            : mparent(parent)
        {}
        
        virtual ~ScriptingAccess();

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
        virtual DispatchInterface::shared_ptr getCommand( int ticket );

        /**
         * List of executed functions.
         */
        typedef std::vector< ProgramInterfacePtr > Functions;

        /** 
         * Load and execute a set of functions into the Program Processor for execution
         * 
         * @param filename The file to load
         * @param do_throw Set to yes to cause the parse errors to be thrown
         * instead of being printed to the Logger interface.
         * 
         * @return A list of functions loaded in the ProgramProcessor.
         */
        virtual Functions loadFunctions(std::string filename, bool do_throw = false );

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
        virtual Functions loadFunctions(std::string code, std::string filename, bool do_throw = false );

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
        virtual bool loadPrograms( std::string filename, bool do_throw = false );

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
        virtual bool loadPrograms( std::string code, std::string filename, bool do_throw = false );

        /** 
         * Unload a program from the Program Processor.
         * 
         * @param name The name of the loaded program.
         * @param do_throw Set to yes to cause the unloading errors to be thrown
         * instead of being printed to the Logger interface.
         * 
         * @return true if it could be unloaded
         */
        virtual bool unloadProgram( std::string name, bool do_throw = false );

        /** 
         * Get a list of the loaded programs.
         * 
         * 
         * @return A list of program names.
         */
        virtual std::vector<std::string> getPrograms() const;

        /** 
         * Check if a program was loaded.
         * 
         * @param name The name of the program.
         * 
         * @return True if so.
         */
        virtual bool hasProgram(std::string name) const;

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
        virtual int getProgramLine(std::string name) const;

        /** 
         * Get the original script description of a loaded program.
         * 
         * @param name The name of the program.
         * 
         * @return A string containing only that program.
         */
        virtual std::string getProgramText(std::string name ) const;

        /** 
         * Get the status of the program in a human readable form.
         * 
         * @param name The name of the program.
         * 
         * @return The ProgramInterface::Status as a string.
         */
        virtual std::string getProgramStatus(std::string name ) const;

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
        virtual bool loadStateMachines( std::string filename, bool do_throw = false  );

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
        virtual bool loadStateMachines( std::string code, std::string filename, bool do_throw = false  );

        /** 
         * Unload a state machine from the StateMachineProcessor.
         * 
         * @param name The name of the loaded program.
         * @param do_throw Set to yes to cause the unloading errors to be thrown
         * instead of being printed to the Logger interface.
         * 
         * @return true if it could be unloaded
         */
        virtual bool unloadStateMachine( std::string name, bool do_throw = false );

        /** 
         * Check if a state machine was loaded.
         * 
         * @param name The name of the state machine.
         * 
         * @return True if so.
         */
        virtual bool hasStateMachine(std::string name) const;

        /** 
         * Get a list of the loaded state machines.
         * 
         * 
         * @return A list of state machine names.
         */
        virtual std::vector<std::string> getStateMachines() const;

        /** 
         * Get the original script description of a loaded state machine.
         * 
         * @param name The name of the state machine.
         * 
         * @return A string containing only that state machine.
         */
        virtual std::string getStateMachineText(std::string name ) const;

        /** 
         * Get the status of the state machine in a human readable form.
         * 
         * @param name The name of the state machine.
         * 
         * @return The State MachineInterface::Status as a string.
         */
        virtual std::string getStateMachineStatus(std::string name ) const;

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
        virtual int getStateMachineLine(std::string name ) const;

        /** 
         * Get the current state of an activated state machine.
         * 
         * @param name The name of the state machine
         * 
         * @return The current state.
         */
        virtual std::string getCurrentState(std::string name ) const;
    };
}

#endif
