/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  StateMachineTask.cxx 

                        StateMachineTask.cxx -  description
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
 
 

#include "execution/StateMachineTask.hpp"
#include <corelib/Attribute.hpp>
#include "execution/TemplateFactories.hpp"
#include "execution/TaskContext.hpp"
#include "execution/FactoryExceptions.hpp"

namespace ORO_Execution
{
    using namespace ORO_CoreLib;
    using namespace ORO_CoreLib::detail;

        CommandFactoryInterface* StateMachineTask::createCommandFactory() {
            // Add the state specific methods :
            // Special trick : we store the 'this' pointer in a DataSource, such that when
            // the created commands are copied, they also get the new this pointer.
            // This requires template specialisations on the TemplateFactory level.
            TemplateCommandFactory< DataSource<StateMachineWPtr> >* fact = newCommandFactory( static_cast< DataSource<StateMachineWPtr>* >( _this.get() ) );
            fact->add("activate",
                      command_ds(&StateMachine::activate, &StateMachine::isStrictlyActive,
                                 "Activate this StateMachine to initial state and enter request Mode."));
            fact->add("deactivate",
                      command_ds(&StateMachine::deactivate, &StateMachine::isActive,
                                 "Deactivate this StateMachine", true));
            fact->add("start",
                      command_ds(&StateMachine::automatic, &StateMachine::isAutomatic,
                                 "Start this StateMachine, enter automatic Mode."));
            fact->add("automatic",
                      command_ds(&StateMachine::automatic, &StateMachine::isAutomatic,
                                 "Start this StateMachine, enter automatic Mode."));
            fact->add("pause",
                      command_ds(&StateMachine::pause, &StateMachine::isPaused,
                                 "Pause this StateMachine, enter paused Mode."));
            fact->add("step",
                      command_ds(&StateMachine::step, &StateMachine::stepDone,
                                 "Step this StateMachine. When paused, step a single instruction or transition evaluation. \n"
                                 "When in reactive mode, evaluate transitions and go to a next state, or if none, run handle."));
            fact->add("reset",
                      command_ds(&StateMachine::reset, &StateMachine::inInitialState,
                                 "Reset this StateMachine to the initial state"));
            fact->add("stop",
                      command_ds(&StateMachine::stop, &StateMachine::inFinalState,
                                 "Stop this StateMachine to the final state and enter request Mode."));
            fact->add("reactive",
                      command_ds(&StateMachine::reactive, &StateMachine::isStrictlyActive,
                                 "Enter reactive mode (see requestState() and step() ).\n Command is done if ready for requestState() or step() command."));
            fact->add("requestState",
                      command_ds(&StateMachine::requestState, &StateMachine::stepDone,
                                 "Request to go to a particular state. Will succeed if there exists a valid transition from this state to the requested state.",
                                 "State", "The state to make the transition to."));
            return fact;
        }

        MethodFactoryInterface* StateMachineTask::createMethodFactory()
        {
            TemplateMethodFactory< DataSource<StateMachineWPtr> >* f = newMethodFactory(static_cast< DataSource<StateMachineWPtr>* >( _this.get() ));
            f->add("inState", data_ds(&StateMachine::inState, "Is the StateMachine in a given state ?", "State", "State Name") );
            f->add("inError", data_ds(&StateMachine::inError, "Is this StateMachine in error ?") );
            f->add("getState", data_ds(&StateMachine::getCurrentStateName, "The name of the current state. An empty string if not active.") );
            f->add("isActive", data_ds(&StateMachine::isActive, "Is this StateMachine activated (possibly in transition) ?") );
            f->add("isRunning", data_ds(&StateMachine::isAutomatic, "Is this StateMachine running in automatic mode ?") );
            f->add("isReactive", data_ds(&StateMachine::isReactive, "Is this StateMachine ready and waiting for requests or events ?") );
            f->add("isPaused", data_ds(&StateMachine::isPaused, "Is this StateMachine paused ?") );
            f->add("inInitialState", data_ds(&StateMachine::inInitialState, "Is this StateMachine in the initial state ?") );
            f->add("inFinalState", data_ds(&StateMachine::inFinalState, "Is this StateMachine in the final state ?") );
            f->add("inTransition", data_ds(&StateMachine::inTransition, "Is this StateMachine executing a entry|handle|exit program ?") );
            return f;
        }

        StateMachineTask* StateMachineTask::copy(StateMachinePtr newsc, std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate )
        {
            // if this gets copied, all created commands will use the new instance of StateMachineTask to
            // call the member functions. Further more, all future commands for the copy will also call the new instance
            // while future commands for the original will still call the original. 
            StateMachineTask* tmp = new StateMachineTask( newsc, this->getExecutionEngine() );
            replacements[ _this.get() ] = tmp->_this.get(); // put 'newsc' in map

            AttributeRepository* dummy = this->attributeRepository.copy( replacements, instantiate );
            tmp->attributeRepository = *dummy;
            delete dummy;

            return tmp;
        }

        StateMachineTask::StateMachineTask(StateMachinePtr statemachine, ExecutionEngine* ee)
            : TaskContext( statemachine->getName(), ee ),
              _this( new ValueDataSource<StateMachineWPtr>( statemachine ) ) // was: VariableDataSource.
        {
            this->commands()->registerObject("this", this->createCommandFactory() );
            this->methods()->registerObject("this", this->createMethodFactory() );
        }

    StateMachineTask::~StateMachineTask()
    {
    }
}

