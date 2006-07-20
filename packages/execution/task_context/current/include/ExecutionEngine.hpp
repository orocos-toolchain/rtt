/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ExecutionEngine.hpp 

                        ExecutionEngine.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#ifndef ORO_EXECUTION_ENGINE_HPP
#define ORO_EXECUTION_ENGINE_HPP

#include "RunnableInterface.hpp"
#include "ActivityInterface.hpp"
#include "CommandProcessor.hpp"
#include "ProgramProcessor.hpp"
#include "StateMachineProcessor.hpp"

namespace OS {
    class Semaphore;
}
namespace RTT
{
    class TaskCore;

    /**
     * An execution engine serialises (executes one after the other)
     * the execution of all commands, programs, state machines and
     * incomming events for a task.  Any function executing in the
     * same execution engine is guaranteed to be thread-safe with
     * respect to other functions executing in the same execution
     * engine.
     *
     * The ExecutionEngine bundles a CommandProcessor, ProgramProcessor,
     * StateMachineProcessor and EventProcessor.
     */
    class ExecutionEngine
        : public RunnableInterface
    {
    protected:
        OS::Semaphore* work_sem;
        OS::Semaphore* loop_sem;
        TaskCore*     taskc;
        ExecutionEngine* mainee;

        CommandProcessor* cproc;
        ProgramProcessor* pproc;
        StateMachineProcessor* smproc;
        EventProcessor* eproc;

        std::vector<ExecutionEngine*> children;

        bool eerun;
    public:
        /**
         * Create an execution engine with a CommandProcessor, ProgramProcessor 
         * and StateMachineProcessor. The EventProcessor is the event processor
         * of this task. If you provide another ExecutionEngine as argument,
         * this execution engine delegates all requests to that execution engine
         * and does itself nothing.
         * @param owner The TaskCore in which this execution engine executes.
         * @param parent The parent which will execute this ExecutionEngine. This execution engine
         * will act as a slave. When the parent executes, this execution engine
         * is executed as well. If none given, one must attach an ActivityInterface
         * implementation in order to run this engine.
         */
        ExecutionEngine( TaskCore* owner, ExecutionEngine* parent = 0 );
        
        ~ExecutionEngine();

        virtual bool initialize();

        virtual void step();
        virtual void loop();
        virtual bool breakLoop();

        virtual void finalize();

        virtual void setActivity(ActivityInterface* t);

        /**
         * In case a parent execution engine was given, return it, otherwise,
         * return \a this.
         */
        ExecutionEngine* getParent();

        /**
         * Returns the owner of this execution engine.
         */
        TaskCore* getTaskCore() { return taskc; }

        /**
         * Inform this execution engine that it gets a new parent.
         */
        void reparent(ExecutionEngine* new_parent);

        /** 
         * Return the CommandProcessor of this engine.
         */
        CommandProcessor* commands() const {
            return mainee ? mainee->getCommandProcessor() : cproc;
        }

        /** 
         * Return the ProgramProcessor of this engine.
         */
        ProgramProcessor* programs() const {
            return mainee ? mainee->getProgramProcessor() : pproc;
        }

        /** 
         * Return the StateMachineProcessor of this engine.
         */
        StateMachineProcessor* states() const {
            return mainee ? mainee->getStateMachineProcessor() : smproc;
        }

        /** 
         * Return the EventProcessor of this engine.
         */
        EventProcessor* events() const;

        CommandProcessor* getCommandProcessor() const {
            return mainee ? mainee->getCommandProcessor() : cproc;
        }

        ProgramProcessor* getProgramProcessor() const {
            return mainee ? mainee->getProgramProcessor() : pproc;
        }

        StateMachineProcessor* getStateMachineProcessor() const {
            return mainee ? mainee->getStateMachineProcessor() : smproc;
        }

        EventProcessor* getEventProcessor() const;

    };

}
#endif
