/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  StateMachineTask.cpp

                        StateMachineTask.cpp -  description
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



#include "StateMachineTask.hpp"

#include "../Attribute.hpp"
#include "../internal/FactoryExceptions.hpp"
#include "../ExecutionEngine.hpp"
#include "../Method.hpp"

namespace RTT
{

    using namespace detail;

        void StateMachineTask::createMethodFactory() {
            // Add the state specific methods :
            // Special trick : we store the 'this' pointer in a DataSource, such that when
            // the created commands are copied, they also get the new this pointer.
            // This requires template specialisations on the TemplateFactory level.
            DataSource<StateMachineWPtr>* ptr = _this.get();

            // I had to make activate() a command because the entry {}
            // may contain commands upon which the state machine is
            // not strictly active (entry executed and no transition
            // in progress) when activate() returns, hence activate()
            // takes time and is thus a method. This is however in
            // violation with the concept of 'initialisation of the
            // SM' which may contain non-rt safe code. When activate() is
            // called directly upon the SM in C++, it _is_ a method, but
            // with the same deficiencies.
            methods()->addMethodDS(ptr, method_ds("activate",
                                               &StateMachine::activate),
                                    "Activate this StateMachine to initial state and enter request Mode.");
            methods()->addMethodDS(ptr, method_ds("deactivate",
                                               &StateMachine::deactivate),
                                    "Deactivate this StateMachine");
            methods()->addMethodDS(ptr, method_ds("start",
                                               &StateMachine::automatic),
                                    "Start this StateMachine, enter automatic Mode.");
            methods()->addMethodDS(ptr, method_ds("automatic",
                                               &StateMachine::automatic),
                                    "Start this StateMachine, enter automatic Mode.");
            methods()->addMethodDS(ptr, method_ds("pause",
                      &StateMachine::pause),
                                 "Pause this StateMachine, enter paused Mode.");
            methods()->addMethodDS(ptr, method_ds("step",
                      &StateMachine::step),
                                 "Step this StateMachine. When paused, step a single instruction or transition evaluation. \n"
                                 "When in reactive mode, evaluate transitions and go to a next state, or if none, run handle.");
            methods()->addMethodDS(ptr, method_ds("reset",
                      &StateMachine::reset),
                                 "Reset this StateMachine to the initial state");
            methods()->addMethodDS(ptr, method_ds("stop",
                      &StateMachine::stop),
                                 "Stop this StateMachine to the final state and enter request Mode.");
            methods()->addMethodDS(ptr, method_ds("reactive",
                      &StateMachine::reactive),
                                 "Enter reactive mode (see requestState() and step() ).\n Method is done if ready for requestState() or step() method.");
            methods()->addMethodDS(ptr, method_ds("requestState",
                      &StateMachine::requestState),
                                 "Request to go to a particular state. Will succeed if there exists a valid transition from this state to the requested state.",
                                 "State", "The state to make the transition to.");

            methods()->addMethodDS(ptr, method_ds("inState", &StateMachine::inState), "Is the StateMachine in a given state ?", "State", "State Name");
            methods()->addMethodDS(ptr, method_ds("inError", &StateMachine::inError), "Is this StateMachine in error ?");
            methods()->addMethodDS(ptr, method_ds("getState", &StateMachine::getCurrentStateName), "The name of the current state. An empty string if not active.");
            methods()->addMethodDS(ptr, method_ds("isActive", &StateMachine::isActive), "Is this StateMachine activated (possibly in transition) ?");
            methods()->addMethodDS(ptr, method_ds("isRunning", &StateMachine::isAutomatic), "Is this StateMachine running in automatic mode ?");
            methods()->addMethodDS(ptr, method_ds("isReactive", &StateMachine::isReactive), "Is this StateMachine ready and waiting for requests or events ?");
            methods()->addMethodDS(ptr, method_ds("isPaused", &StateMachine::isPaused), "Is this StateMachine paused ?");
            methods()->addMethodDS(ptr, method_ds("inInitialState", &StateMachine::inInitialState), "Is this StateMachine in the initial state ?");
            methods()->addMethodDS(ptr, method_ds("inFinalState", &StateMachine::inFinalState), "Is this StateMachine in the final state ?");
            methods()->addMethodDS(ptr, method_ds("inTransition", &StateMachine::inTransition), "Is this StateMachine executing a entry|handle|exit program ?");
        }

        StateMachineTask* StateMachineTask::copy(ParsedStateMachinePtr newsc, std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate )
        {
            // if this gets copied, all created methods will use the new instance of StateMachineTask to
            // call the member functions. Further more, all future methods for the copy will also call the new instance
            // while future methods for the original will still call the original.
            StateMachineTask* tmp = new StateMachineTask( newsc, this->mengine );
            replacements[ _this.get() ] = tmp->_this.get(); // put 'newsc' in map

            AttributeRepository* dummy = this->attributes()->copy( replacements, instantiate );
            *(tmp->attributes()) = *dummy;
            delete dummy;

            return tmp;
        }

        StateMachineTask::StateMachineTask(ParsedStateMachinePtr statem, ExecutionEngine* ee)
            : TaskObject( statem->getName() ),
              _this( new ValueDataSource<StateMachineWPtr>( statem ) ),
              statemachine(statem),
              mengine(ee)
        {
            this->createMethodFactory();
            this->setEngine( ee );
        }

    StateMachineTask::~StateMachineTask()
    {
        // When the this TaskObject is deleted, make sure the program does not reference us.
        ParsedStateMachinePtr prog = statemachine.lock();
        if ( prog ) {
            prog->setTaskObject(0);
        }
    }
}

