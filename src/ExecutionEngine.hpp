/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ExecutionEngine.hpp

                        ExecutionEngine.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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


#ifndef ORO_EXECUTION_ENGINE_HPP
#define ORO_EXECUTION_ENGINE_HPP

#include "base/RunnableInterface.hpp"
#include "base/ActivityInterface.hpp"
#include <vector>

#include "../rtt-config.h"
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
#include "internal/EventProcessor.hpp"
#endif
#ifdef OROPKG_EXECUTION_ENGINE_COMMANDS
#include "internal/CommandProcessor.hpp"
#endif
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
#include "scripting/ProgramProcessor.hpp"
#endif
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
#include "scripting/StateMachineProcessor.hpp"
#endif

namespace RTT
{

    /**
     * An execution engine serialises (executes one after the other)
     * the execution of all commands, programs, state machines and
     * incomming events for a task.  Any function executing in the
     * same execution engine is guaranteed to be thread-safe with
     * respect to other functions executing in the same execution
     * engine.
     *
     * The ExecutionEngine bundles a internal::CommandProcessor, scripting::ProgramProcessor,
     * scripting::StateMachineProcessor and EventProcessor.
     *
     * @par Changing the Execution Policy
     * One can subclass this class in order to change the run-time
     * behaviour. Use base::TaskCore::setExecutionEngine in order to
     * install a new ExecutionEngine in a component. All Members of
     * this class are protected and thus accessible in a subclass.
     * @ingroup Processor
     */
    class RTT_API ExecutionEngine
        : public base::RunnableInterface
    {
    protected:
        enum EngineState { Stopped, Activating, Active, Running };
        /**
         * The parent or 'owner' of this ExecutionEngine, may be null.
         */
        base::TaskCore*     taskc;

        /**
         * The ExecutionEngine keeps a state of its own which is synchronised
         * with the parent and child base::TaskCore states.
         */
        EngineState     estate;

        /**
         * We store the Processors as base::RunnableInterface pointers,
         * and dynamic_cast them back to the correct type.
         */
        base::RunnableInterface* cproc;
        base::RunnableInterface* pproc;
        base::RunnableInterface* smproc;
        base::RunnableInterface* eproc;

        /**
         * All tasks which execute in this ExecutionEngine.
         */
        std::vector<base::TaskCore*> children;

        /**
         * Install new Processors.
         */
        void setup();

        /**
         * Call all necessary hook functions of parent and children
         * TaskContexts.
         */
        bool startContexts();
    public:
        /**
         * Create an execution engine with a internal::CommandProcessor, scripting::ProgramProcessor
         * and StateMachineProcessor.
         * @param owner The base::TaskCore in which this execution engine executes.
         * It may be null, in that case no base::TaskCore owns this execution engine.
         */
        ExecutionEngine( base::TaskCore* owner = 0);

        ~ExecutionEngine();

        /**
         * Run the Execution Engine's processors, but not the updateHook()
         * functions of the TaskCores.
         */
        bool activate();

        /**
         * Run the Execution Engine's processors \b and the updateHook()
         * functions of the TaskCores.
         */
        bool start();

        /**
         * Stop the processors and all TaskCores.
         */
        bool stop();

        virtual bool initialize();

        /**
         * Executes (in that order) programs, state machines, commands,
         * events and the base::TaskCore's update() function.
         */
        virtual void step();

        virtual bool breakLoop();

        virtual void finalize();

        virtual void setActivity(base::ActivityInterface* t);

        virtual bool hasWork();

        /**
         * The base::TaskCore which created this ExecutionEngine.
         */
        base::TaskCore* getParent();

        /**
         * Add a base::TaskCore to execute.
         */
        virtual void addChild(base::TaskCore* tc);

        /**
         * Remove a base::TaskCore from execution.
         */
        virtual void removeChild(base::TaskCore* tc);

        /**
         * Returns the owner of this execution engine.
         */
        base::TaskCore* getTaskCore() const { return taskc; }

        /**
         * Return the internal::CommandProcessor of this engine.
         */
        internal::CommandProcessor* commands() const;

        /**
         * Return the scripting::ProgramProcessor of this engine.
         */
        scripting::ProgramProcessor* programs() const;

        /**
         * Return the scripting::StateMachineProcessor of this engine.
         */
        scripting::StateMachineProcessor* states() const;

        /**
         * Return the internal::EventProcessor of this engine.
         */
        internal::EventProcessor* events() const;

        /**
         * Install a new CommandProcessor.
         * @param c becomes owned by this object and is returned in commands().
         */
        virtual void setCommandProcessor(internal::CommandProcessor* c);

        /**
         * Install a new ProgramProcessor.
         * @param p becomes owned by this object and is returned in programs().
         */
        virtual void setProgramProcessor(scripting::ProgramProcessor* p);

        /**
         * Install a new StateMachineProcessor.
         * @param s becomes owned by this object and is returned in states().
         */
        virtual void setStateMachineProcessor(scripting::StateMachineProcessor* s);

        /**
         * Install a new EventProcessor.
         * @param e becomes owned by this object and is returned in events().
         */
        virtual void setEventProcessor(internal::EventProcessor* e);

    };

}
#endif
