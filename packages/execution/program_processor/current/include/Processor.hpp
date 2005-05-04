/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:19:04 CEST 2002  Processor.hpp

                        Processor.hpp -  description
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

#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "corelib/RunnableInterface.hpp"
#include "corelib/CommandInterface.hpp"

#include <list>
#include <string>
#include <vector>

#include <pkgconf/execution_program_processor.h>

namespace ORO_OS
{
    class Semaphore;
    class Mutex;
    class MutexRecursive;
}

namespace ORO_CoreLib
{
    template< class T>
    class AtomicQueue;
}

namespace ORO_Execution
{
    class StateMachine;
    class ProgramInterface;
    class FunctionGraph;

    using ORO_CoreLib::CommandInterface;

    class program_load_exception
    {
        std::string merror;
    public:
        program_load_exception( const std::string& error )
            : merror( error )
        {
        }
        const std::string what() const
        {
            return merror;
        }
    };

    class program_unload_exception
    {
        std::string merror;
    public:
        program_unload_exception( const std::string& error )
            : merror( error )
        {
        }
        const std::string what() const
        {
            return merror;
        }
    };

    /**
     * @brief This class implements a controllable program processor.
     * It executes Realtime Programs and State Machines and can 
     * accept and execute external commands when running.
     *
     * @todo Make a more efficient implementation instead of iterating
     * over the programs and state lists. Also provide object based
     * control methods instead of string based.
     */
    class Processor
        : public ORO_CoreLib::RunnableInterface
    {
    public:
        /**
         * The status of a Program.
         */
        struct ProgramStatus {
            enum status { unloaded, stopped, running, stepmode, error };
        };
        /**
         * The status of a StateMachine.
         */
        struct StateMachineStatus {
            enum status { unloaded, inactive, activating, active, running, stopping, stopped, resetting, deactivating, paused, error };
        };

        /**
         * Constructs a new Processor
         * @param queue_size The size of the command queue.
         *
         */
        Processor(int queue_size = ORONUM_EXECUTION_PROC_QUEUE_SIZE);

        virtual ~Processor();
        /**
         * Load a new State Machine.
         * @throw program_load_exception if a state machine with the same name already exists.
         */
        bool loadStateMachine( StateMachine* sc );

        /**
         * Unload a deactivated State Machine.
         * @throw program_unload_exception if the state machine or one of its children is
         * still active.
         */
        bool unloadStateMachine( const std::string& name );
        /**
         * Activate a State Machine, enter the initial state.
         */
        bool activateStateMachine(const std::string& name);
        /**
         * Completely deactivate a state machine ( executes exit of current state and stops ).
         */
        bool deactivateStateMachine(const std::string& name);
        /**
         * Start evaluating a state machine.
         */
        bool startStateMachine(const std::string& name);
        /**
         * Pause execution of a state machine.
         */
        bool pauseStateMachine(const std::string& name);
        /**
         * Stop evaluating a state context and go to the final state.
         */
        bool stopStateMachine(const std::string& name);
        /**
         * Execute one statement of a state machine program or
         * evaluate one transition.
         */
        bool stepStateMachine(const std::string& name);
        /**
         * Reset to the initial state
         */
        bool resetStateMachine(const std::string& name);
        /**
         * Discard a state context.
         * @throw program_unload_exception if the state machine or one of its children is
         * still active or does not exist.
         */
        bool deleteStateMachine(const std::string& name);

        /**
         * Put a StateMachine in the default, state-at-a-time, mode.
         */
        bool steppedModeStateMachine(const std::string& name);

        /**
         * Put a StateMachine in the request Mode, where the user
         * request to go to a given state.
         */
        bool requestModeStateMachine(const std::string& name);

        /**
         * Request to go to a specified \a state in StateMachine \a name.
         * Works only if the StateMachineStatus is 'active'.
         * @return false if name not found, or StateMachineStatus not active or transition to state is not possible.
         *         true otherwise, and transition is in progress.
         */
        bool requestStateStateMachine( const std::string& name, const std::string& state);

        /**
         * Put a StateMachine in the traverse-as-many-states-as-possible mode
         * (until no transition succeeds).
         */
        bool continuousModeStateMachine(const std::string& name);

        /**
         * Return true if the StateMachine is successfully running.
         */
        bool isStateMachineRunning( const std::string& name) const;

        /**
         * Check if a StateMachine is in Stepped mode.
         * @return true if it is so, false if it is continuous.
         */
        bool isStateMachineStepped(const std::string& name) const;

        /**
         * Return the status of a StateMachine.
         */
        StateMachineStatus::status getStateMachineStatus(const std::string& name) const;

        /**
         * Load a new Program.
         * @return false if a program with the same name already exists.
         * @throw program_load_exception if a state machine with the same name already exists.
         */
        bool loadProgram( ProgramInterface* pi ) ;

        /**
         * Start executing a Program.
         */
        bool startProgram(const std::string& name);

        /**
         * Pause the execution of a program.
         */
        bool pauseProgram(const std::string& name);

        /**
         * Stop execution and reset logic to the beginning of the associated program.
         */
        bool stopProgram(const std::string& name);

        /**
         * Execute one statement of a program.
         */
        bool stepProgram(const std::string& name);

        /**
         * Completely discard a loaded Program.
         * @throw program_unload_exception if the program is
         * not stopped or does not exist.
         * @return false if a program with the same name already exists.
         */
        bool deleteProgram(const std::string& name);

        /**
         * Return true if a Program is successfully running.
         */
        bool isProgramRunning( const std::string& name) const;

        /**
         * Return the status of a Program.
         */
        ProgramStatus::status getProgramStatus(const std::string& name) const;

        /**
         * Use this method to run the Processor in blocked (non periodic) mode.
         * @see breakLoop(), resumeLoop()
         */
        virtual void loop();

        /**
         * Instruct the Processor to resume ( awake ) its loop and check pending programs
         * and commands.
         */
        void resumeLoop();

        virtual bool breakLoop();

        virtual bool initialize();
        virtual void step();
        virtual void finalize();

        /**
         * Queue and execute (process) a given command. The command is executed in step() or loop() 
         * directly after all other queued CommandInterface objects. The constructor parameter \a queue_size 
         * limits how many commands can be queued in between step()s or loop().
         * @return The command number. You can check if the given command was processed
         * by calling Processor::isProcessed( command id ). The command number is reset to one
         * each time the Processor is (re-)started. 
         * @return 0 when the Processor is not running or does not accept commands.
         * @see isProcessed, acceptCommands
         */
        int process(CommandInterface* c);

        /**
         * Check if a given command id has been processed.
         */
        bool isProcessed( int id ) const;

        /**
         * Run a given function in step() or loop(). The function may only
         * be destroyed after isFunctionFinished() returns true or the 
         * Processor is stopped. The number of functions the Processor can
         * run in parallel is set in its constructor argument \a queue_size. In
         * addition to that number, a queue of the same size is allocated for
         * pending functions to execute.
         * @return false if the Processor is not running or the 'pending' queue is full.
         * @see isFunctionFinished, acceptCommands
         */
        bool runFunction(ProgramInterface* f);
        /**
         * Check if the execution of a function has finished.
         * @return true if so.
         */
        bool isFunctionFinished( ProgramInterface* f ) const;

        /**
         * Get a list of all loaded Programs.
         */
        std::vector<std::string> getProgramList() const;

        /**
         * Get a list of all loaded StateMachines and their children.
         */
        std::vector<std::string> getStateMachineList() const;

        /**
         * Get a pointer to a loaded Program.
         */
        const ProgramInterface* getProgram(const std::string& name) const;

        /**
         * Get a pointer to a loaded StateMachine.
         */
        const StateMachine* getStateMachine(const std::string& name) const;

        /**
         * Should the Processor accept or reject commands in \a process().
         * @param true_false true to accept, false to reject.
         */
        void acceptCommands( bool true_false) { accept=true_false; }

        class ProgramInfo;
        class StateInfo;
    private:
        void recursiveLoadStateMachine( StateMachine* sc );
        void recursiveCheckLoadStateMachine( StateMachine* sc );
        void recursiveUnloadStateMachine( StateMachine* sc );
        void recursiveCheckUnloadStateMachine(const StateInfo& si );

        typedef std::map<std::string,ProgramInfo> ProgMap;
        ProgMap* programs;
        typedef std::map<std::string,StateInfo> StateMap;
        StateMap*   states;
        typedef ProgMap::iterator program_iter;
        typedef StateMap::iterator state_iter;
        typedef ProgMap::const_iterator cprogram_iter;
        typedef StateMap::const_iterator cstate_iter;

        std::vector<ProgramInterface*> funcs;

        bool accept;
        ORO_CoreLib::AtomicQueue<CommandInterface*>* a_queue;
        ORO_CoreLib::AtomicQueue<ProgramInterface*>* f_queue;

        /**
         * Counting how much commands we processed.
         */
        int coms_processed;

        /**
         * Guard state list
         */
        ORO_OS::MutexRecursive* loadmonitor;

        ORO_OS::Semaphore* queuesem;
        bool doloop;
        bool loopmode;
    };

}

#endif
