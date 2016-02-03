/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  StateMachineService.cpp

                        StateMachineService.cpp -  description
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



#include "StateMachineService.hpp"

#include "../Attribute.hpp"
#include "../FactoryExceptions.hpp"
#include "../TaskContext.hpp"
#include "../OperationCaller.hpp"
#include "SendHandleAlias.hpp"

namespace RTT
{

    using namespace detail;

        void StateMachineService::createOperationFactory() {
            // Add the state specific methods :
            // Special trick : we store the 'this' pointer in a DataSource, such that when
            // the created commands are copied, they also get the new this pointer.
            // This requires template specialisations on the TemplateFactory level.
            DataSource<StateMachinePtr>* ptr = _this.get();

            // I had to make activate() a command because the entry {}
            // may contain commands upon which the state machine is
            // not strictly active (entry executed and no transition
            // in progress) when activate() returns, hence activate()
            // takes time and is thus a method. This is however in
            // violation with the concept of 'initialisation of the
            // SM' which may contain non-rt safe code. When activate() is
            // called directly upon the SM in C++, it _is_ a method, but
            // with the same deficiencies.
            addOperationDS("trace", &StateMachine::trace,ptr).doc("Trace the execution of this StateMachine. *Not* Real-Time.");
            addOperationDS("activate", &StateMachine::activate,ptr).doc("Activate this StateMachine to initial state and enter request Mode.");
            addOperationDS("deactivate", &StateMachine::deactivate,ptr).doc("Deactivate this StateMachine");
            addOperationDS("start", &StateMachine::automatic,ptr).doc("Start this StateMachine, enter automatic Mode.");
            addOperationDS("automatic", &StateMachine::automatic,ptr).doc("Start this StateMachine, enter automatic Mode.");
            addOperationDS("pause", &StateMachine::pause,ptr).doc("Pause this StateMachine, enter paused Mode.");
            addOperationDS("step", &StateMachine::step,ptr).doc(
                                 "Step this StateMachine. When paused, step a single instruction or transition evaluation. \n"
                                 "When in reactive mode, evaluate transitions and go to a next state, or if none, run handle.");
            addOperationDS("reset", &StateMachine::reset,ptr).doc("Reset this StateMachine to the initial state");
            addOperationDS("stop", &StateMachine::stop,ptr).doc("Stop this StateMachine to the final state and enter request Mode.");
            addOperationDS("reactive", &StateMachine::reactive,ptr).doc("Enter reactive mode (see requestState() and step() ).\n OperationCaller is done if ready for requestState() or step() method.");
            addOperationDS("requestState", &StateMachine::requestState,ptr).doc("Request to go to a particular state. Will succeed if there exists a valid transition from this state to the requested state.").arg("State", "The state to make the transition to.");

            addOperationDS("inState", &StateMachine::inState,ptr).doc("Is the StateMachine in a given state ?").arg("State", "State Name");
            addOperationDS("inError", &StateMachine::inError,ptr).doc("Is this StateMachine in error ?");
            addOperationDS("getState", &StateMachine::getCurrentStateName,ptr).doc("The name of the current state. An empty string if not active.");
            addOperationDS("isActive", &StateMachine::isActive,ptr).doc("Is this StateMachine activated (possibly in transition) ?");
            addOperationDS("isRunning", &StateMachine::isAutomatic,ptr).doc("Is this StateMachine running in automatic mode ?");
            addOperationDS("isReactive", &StateMachine::isReactive,ptr).doc("Is this StateMachine ready and waiting for requests or events ?");
            addOperationDS("isPaused", &StateMachine::isPaused,ptr).doc("Is this StateMachine paused ?");
            addOperationDS("inInitialState", &StateMachine::inInitialState,ptr).doc("Is this StateMachine in the initial state ?");
            addOperationDS("inFinalState", &StateMachine::inFinalState,ptr).doc("Is this StateMachine in the final state ?");
            addOperationDS("inTransition", &StateMachine::inTransition,ptr).doc("Is this StateMachine executing a entry|handle|exit program ?");
        }

        StateMachineServicePtr StateMachineService::copy(ParsedStateMachinePtr newsc, std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate )
        {
            // if this gets copied, all created methods will use the new instance of StateMachineService to
            // call the member functions. Further more, all future methods for the copy will also call the new instance
            // while future methods for the original will still call the original.
            StateMachineServicePtr tmp( new StateMachineService( newsc, this->mtc ) );
            replacements[ _this.get() ] = tmp->_this.get(); // put 'newsc' in map

            if (instantiate) {
                // Remove any remaining SendHandleAlias attributes, since they are not allowed for an instantiate...
                // See SendHandleAlias::copy() for more details.
                for ( ConfigurationInterface::map_t::iterator it = values.begin(); it != values.end(); ) {
                    if (dynamic_cast<SendHandleAlias*>(*it)) {
                        it = values.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            ConfigurationInterface* dummy = ConfigurationInterface::copy( replacements, instantiate );
            tmp->loadValues( dummy->getValues());
            delete dummy;

            return tmp;
        }

        StateMachineService::StateMachineService(ParsedStateMachinePtr statem, TaskContext* tc)
            : Service( statem->getName() ),
              _this( new ValueDataSource<StateMachinePtr>( statem ) ),
              statemachine(statem),
              mtc(tc)
        {
            this->createOperationFactory();
            this->setOwner( tc );
        }

    StateMachineService::~StateMachineService()
    {
        // When the this Service is deleted, make sure the program does not reference us.
        if ( statemachine ) {
            statemachine->setService( StateMachineServicePtr() );
        }
    }
    //ExecutionEngine* StateMachineService::engine() const { return mtc->engine(); }

}

