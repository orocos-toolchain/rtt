/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:19 CET 2008  ParserExecutionAccess.cpp

                        ParserExecutionAccess.cpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
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



#include "ParserExecutionAccess.hpp"
#include "../Logger.hpp"
#include "../TaskContext.hpp"
#include "../internal/TaskObject.hpp"
#include "../Method.hpp"
#include "../internal/TaskObject.hpp"

namespace RTT {
    using namespace detail;
    using namespace std;

    ParserExecutionAccess::ParserExecutionAccess( TaskContext* parent )
        : ExecutionAccess(parent)
    {
        OperationInterface* obj = parent->getObject("engine");

        if (!obj)
            obj = new TaskObject("engine","Access to the Execution Engine. \
Use this object in order to address programs or state machines which may or may not be loaded.");


        // Query Methods for programs
        obj->methods()->addMethod( method( "hasProgram", &ExecutionAccess::hasProgram, this),
                                            "Is a program loaded?", "Name", "The Name of the loaded Program"  );
        obj->methods()->addMethod( method( "isProgramRunning", &ExecutionAccess::isProgramRunning, this),
                                            "Is a program running ?", "Name", "The Name of the Loaded Program"  );
        obj->methods()->addMethod( method( "isProgramPaused", &ExecutionAccess::isProgramPaused, this),
                                            "Is a program paused ?", "Name", "The Name of the Loaded Program"  );
        obj->methods()->addMethod( method( "inProgramError", &ExecutionAccess::inProgramError, this),
                                            "Is a program in error ?", "Name", "The Name of the Loaded Program"  );

        // Query Methods for state machines
        obj->methods()->addMethod( method( "hasStateMachine", &ExecutionAccess::hasStateMachine, this),
                                            "Is a state machine loaded?", "Name", "The Name of the loaded State Machine"  );
        obj->methods()->addMethod( method( "isStateMachineActive", &ExecutionAccess::isStateMachineActive, this),
                                            "Is a state machine active ?", "Name", "The Name of the Loaded StateMachine"  );
        obj->methods()->addMethod( method( "isStateMachineRunning", &ExecutionAccess::isStateMachineRunning, this),
                                            "Is a state machine running ?", "Name", "The Name of the Loaded StateMachine"  );
        obj->methods()->addMethod( method( "isStateMachinePaused", &ExecutionAccess::isStateMachinePaused, this),
                                            "Is a state machine paused ?", "Name", "The Name of the Loaded StateMachine"  );
        obj->methods()->addMethod( method( "inStateMachineError", &ExecutionAccess::inStateMachineError, this),
                                            "Is a state machine in error ?", "Name", "The Name of the Loaded StateMachine"  );
        obj->methods()->addMethod( method( "inStateMachineState", &ExecutionAccess::inStateMachineState, this),
                                         "Is a state machine in a given state ?",
                                         "Name", "The Name of the Loaded StateMachine",
                                         "State", "The name of the state in which it could be.");
        obj->methods()->addMethod( method( "getStateMachineState", &ExecutionAccess::getStateMachineState, this),
                                         "Get the current state name of a state machine.",
                                         "Name", "The Name of the Loaded StateMachine");

        // Methods for programs
        obj->methods()->addMethod( method( "startProgram", &ExecutionAccess::startProgram, this),
                    "Start a program", "Name", "The Name of the Loaded Program"  );
        obj->methods()->addMethod( method( "stopProgram", &ExecutionAccess::stopProgram ,this),
                                           "Stop a program", "Name", "The Name of the Started Program" );

        obj->methods()->addMethod( method( "stepProgram", &ExecutionAccess::stepProgram ,this),
                    "Step a single program instruction", "Name", "The Name of the Paused Program"  );
        obj->methods()->addMethod( method( "pauseProgram", &ExecutionAccess::pauseProgram ,this),
                    "Pause a program", "Name", "The Name of the Started Program"  );

        // Methods for state machines
        // Activate/deactivate:
        obj->methods()->addMethod( method( "activateStateMachine", &ExecutionAccess::activateStateMachine ,this),
                    "Activate a StateMachine", "Name", "The Name of the Loaded StateMachine"  );
        obj->methods()->addMethod( method( "deactivateStateMachine", &ExecutionAccess::deactivateStateMachine ,this),
                    "Deactivate a StateMachine", "Name", "The Name of the Stopped StateMachine"  );

        // start/stop/pause:
        obj->methods()->addMethod( method( "startStateMachine", &ExecutionAccess::startStateMachine ,this),
                    "Start a StateMachine", "Name", "The Name of the Activated/Paused StateMachine"  );
        obj->methods()->addMethod( method( "pauseStateMachine", &ExecutionAccess::pauseStateMachine ,this),
                    "Pause a StateMachine", "Name", "The Name of a Started StateMachine"  );
        obj->methods()->addMethod( method( "stopStateMachine", &ExecutionAccess::stopStateMachine ,this),
                    "Stop a StateMachine", "Name", "The Name of the Started/Paused StateMachine"  );
        obj->methods()->addMethod( method( "resetStateMachine", &ExecutionAccess::resetStateMachine ,this),
                    "Reset a StateMachine", "Name", "The Name of the Stopped StateMachine" );

        // request states
        obj->methods()->addMethod( method( "requestStateMachineState", &ExecutionAccess::requestStateMachineState ,this),
                                           "Request a State change",
                                           "Name", "The Name of the StateMachine",
                                           "StateName", "The Name of the State to change to" );
        parent->addObject( obj );
    }

    ParserExecutionAccess::~ParserExecutionAccess()
    {
    }
}
