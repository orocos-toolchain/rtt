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
#include <vector>

#include <pkgconf/execution_task_context.h>
#ifdef OROPKG_EXECUTION_ENGINE_EVENTS
#include "rtt/EventProcessor.hpp"
#endif
#ifdef OROPKG_EXECUTION_ENGINE_COMMANDS
#include "rtt/CommandProcessor.hpp"
#endif
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
#include "rtt/ProgramProcessor.hpp"
#endif
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
#include "rtt/StateMachineProcessor.hpp"
#endif

namespace RTT
{
    class TaskCore;
    namespace OS {
        class Semaphore;
    }

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

        /**
         * We store them as RunnableInterface pointers,
         * and static_cast them back to the correct type.
         */
        RunnableInterface* cproc;
        RunnableInterface* pproc;
        RunnableInterface* smproc;
        RunnableInterface* eproc;

        std::vector<ExecutionEngine*> children;

        bool eerun;

        /**
         * This function checks if a mainee is present, if not,
         * new Processors are created, if there is, the Processors
         * are destroyed.
         */
        void setup();
    public:
        /**
         * Create an execution engine with a CommandProcessor, ProgramProcessor 
         * and StateMachineProcessor. If you provide another ExecutionEngine as argument,
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

        /**
         * Executes (in that order) programs, state machines, commands,
         * events and the TaskCore's update() function.
         */
        virtual void step();
        /**
         * Identical to step(), but blocks for commands and events.
         */
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
         * Returns the semaphore which is signaled when events or
         * commands arrive. The user may signal this semaphore to
         * force the execution of a step when the ExecutionEngine
         * is in a non periodic activity.
         * @return null if the ExecutionEngine is periodic, a Semaphore
         * otherwise.
         */
        OS::Semaphore* getSemaphore() const;

        /**
         * Returns the owner of this execution engine.
         */
        TaskCore* getTaskCore() const { return taskc; }

        /**
         * Inform this execution engine that it gets a new parent.
         */
        void reparent(ExecutionEngine* new_parent);

        /** 
         * Return the CommandProcessor of this engine.
         */
        CommandProcessor* commands() const;

        /** 
         * Return the ProgramProcessor of this engine.
         */
        ProgramProcessor* programs() const;

        /** 
         * Return the StateMachineProcessor of this engine.
         */
        StateMachineProcessor* states() const;

        /** 
         * Return the EventProcessor of this engine.
         */
        EventProcessor* events() const;

        void setCommandProcessor(CommandProcessor* c);

        void setProgramProcessor(ProgramProcessor* p);

        void setStateMachineProcessor(StateMachineProcessor* s);

        void setEventProcessor(EventProcessor* e);

    };

}
#endif
