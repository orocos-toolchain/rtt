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
#include "execution/ProcessorInterface.hpp"
#include "execution/StateContextTree.hpp"
#include <os/Mutex.hpp>
#include <corelib/AtomicQueue.hpp>

#include <list>

#include <pkgconf/execution_program_processor.h>

namespace ORO_Execution
{
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
     */
    class Processor
        : public ProcessorInterface,
          public ORO_CoreLib::RunnableInterface
    {
    public:
        /**
         * The status of a Program.
         */
        struct ProgramStatus {
            enum status { unloaded, stopped, running, stepmode, error };
        };
        /**
         * The status of a StateContext.
         */
        struct StateContextStatus {
            enum status { unloaded, inactive, active, running, stopped, paused, todelete, error };
        };

        /**
         * Constructs a new Processor
         * @param queue_size The size of the command queue.
         *
         */
        Processor(int queue_size = ORONUM_EXECUTION_PROC_QUEUE_SIZE);

        virtual ~Processor();

        virtual bool pauseProgram(const std::string& name);

        virtual bool loadStateContext( StateContextTree* sc );
        virtual bool unloadStateContext( const std::string& name );
        virtual bool activateStateContext(const std::string& name);
        virtual bool deactivateStateContext(const std::string& name);
        virtual bool startStateContext(const std::string& name);
        virtual bool pauseStateContext(const std::string& name);
        virtual bool stopStateContext(const std::string& name);
        virtual bool resetStateContext(const std::string& name);
        virtual bool deleteStateContext(const std::string& name);
        virtual bool steppedStateContext(const std::string& name);
        virtual bool continuousStateContext(const std::string& name);

        virtual ProgramStatus::status getProgramStatus(const std::string& name) const;
        virtual StateContextStatus::status getStateContextStatus(const std::string& name) const;

        virtual bool loadProgram( ProgramInterface* pi ) ;
        virtual bool startProgram(const std::string& name);
        virtual bool stopProgram(const std::string& name);
        virtual bool stepProgram(const std::string& name);
        virtual bool deleteProgram(const std::string& name);

        virtual bool isProgramRunning( const std::string& name) const;
        virtual bool isStateContextRunning( const std::string& name) const;
        virtual bool isStateContextStepped(const std::string& name) const;

        virtual bool initialize();
        virtual void step();
        virtual void finalize();

        virtual bool process(CommandInterface* c);

        std::vector<std::string> getProgramList();
        std::vector<std::string> getStateContextList();

        ProgramInterface* getProgram(const std::string& name) const;

        /**
         * Should the Processor accept or reject commands in \a process().
         * @param true_false true to accept, false to reject.
         */
        void acceptCommands( bool true_false) { accept=true_false; }

        class ProgramInfo;
        class StateInfo;
    private:
        void recursiveLoadStateContext( StateContextTree* sc );
        void recursiveCheckLoadStateContext( StateContextTree* sc );
        void recursiveUnloadStateContext( StateContextTree* sc );
        void recursiveCheckUnloadStateContext(const StateInfo& si );

        typedef std::list<ProgramInfo>::iterator program_iter;
        typedef std::list<StateInfo>::iterator state_iter;
        typedef std::list<ProgramInfo>::const_iterator cprogram_iter;
        typedef std::list<StateInfo>::const_iterator cstate_iter;
        std::list<ProgramInfo>* programs;
        std::list<StateInfo>*   states;

        bool accept;
        ORO_CoreLib::AtomicQueue<CommandInterface*> a_queue;

        /**
         * Guard state list
         */
        ORO_OS::MutexRecursive statemonitor;
        ORO_OS::Mutex progmonitor;
    };

}

#endif
