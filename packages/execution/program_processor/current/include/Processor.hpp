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
         * Pause the execution of a program.
         */
        bool pauseProgram(const std::string& name);

        /**
         * Load a new State Machine.
         */
        bool loadStateMachine( StateMachine* sc );

        /**
         * Unload a deactivated State Machine.
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
         */
        bool deleteStateMachine(const std::string& name);

        /**
         * Put a StateMachine in the default, state-at-a-time, mode.
         */
        bool steppedStateMachine(const std::string& name);

        /**
         * Put a StateMachine in the traverse-as-many-states-as-possible mode
         * (until no transition succeeds).
         */
        bool continuousStateMachine(const std::string& name);

        ProgramStatus::status getProgramStatus(const std::string& name) const;
        StateMachineStatus::status getStateMachineStatus(const std::string& name) const;

        /**
         * Load a new Program.
         */
        bool loadProgram( ProgramInterface* pi ) ;

        /**
         * Start executing a Program.
         */
        bool startProgram(const std::string& name);

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
         */
        bool deleteProgram(const std::string& name);

        /**
         * Return true if a Program is successfully running.
         */
        bool isProgramRunning( const std::string& name) const;
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
         * Use this method to run the Processor in blocked (non periodic) mode.
         * @see breakloop(), resumeloop()
         */
        virtual void loop();

        /**
         * Instruct the Processor to resume ( awake ) its loop and check pending programs
         * and commands.
         */
        void resumeloop();

        /**
         * Call this method to let the endless \a loop() return.
         */
        void breakloop();

        virtual bool initialize();
        virtual void step();
        virtual void finalize();

        /**
         * Process a given command.
         * @return The command number. You can check if the given command was processed
         * by calling Processor::isProcessed( command number ).
         */
        int process(CommandInterface* c);
        bool isProcessed( int i );

        bool runFunction(ProgramInterface* f);
        bool isFunctionFinished( ProgramInterface* f );

        std::vector<std::string> getProgramList();
        std::vector<std::string> getStateMachineList();

        ProgramInterface* getProgram(const std::string& name) const;

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
        std::vector<ProgramInterface*>::iterator f_it;

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
        ORO_OS::MutexRecursive* statemonitor;
        ORO_OS::Mutex* progmonitor;

        ORO_OS::Semaphore* queuesem;
        bool doloop;
    };

}

#endif
