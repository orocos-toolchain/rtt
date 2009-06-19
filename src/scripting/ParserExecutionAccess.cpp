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
#include "Logger.hpp"
#include "TaskContext.hpp"
#include "TaskObject.hpp"
#include "Method.hpp"
#include "Command.hpp"

namespace RTT
{
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

        // Commands for programs
        obj->commands()->addCommand( command( "startProgram", &ExecutionAccess::startProgram ,
                    &ExecutionAccess::true_gen , this, tc->engine()->commands()),
                    "Start a program", "Name", "The Name of the Loaded Program"  );
        obj->commands()->addCommand( command( "stopProgram", &ExecutionAccess::stopProgram ,
                                                    &ExecutionAccess::true_gen , this, tc->engine()->commands()),
                                           "Stop a program", "Name", "The Name of the Started Program" );

        obj->commands()->addCommand( command( "stepProgram", &ExecutionAccess::stepProgram ,
                    &ExecutionAccess::true_gen , this, tc->engine()->commands()),
                    "Step a single program instruction", "Name", "The Name of the Paused Program"  );
        obj->commands()->addCommand( command( "pauseProgram", &ExecutionAccess::pauseProgram ,
                    &ExecutionAccess::true_gen , this, tc->engine()->commands()),
                    "Pause a program", "Name", "The Name of the Started Program"  );

        // Commands for state machines
        // Activate/deactivate:
        obj->commands()->addCommand( command( "activateStateMachine", &ExecutionAccess::activateStateMachine ,
                    &ExecutionAccess::true_gen , this, tc->engine()->commands()),
                    "Activate a StateMachine", "Name", "The Name of the Loaded StateMachine"  );
        obj->commands()->addCommand( command( "deactivateStateMachine", &ExecutionAccess::deactivateStateMachine ,
                    &ExecutionAccess::true_gen , this, tc->engine()->commands()),
                    "Deactivate a StateMachine", "Name", "The Name of the Stopped StateMachine"  );

        // start/stop/pause:
        obj->commands()->addCommand( command( "startStateMachine", &ExecutionAccess::startStateMachine ,
                    &ExecutionAccess::true_gen , this, tc->engine()->commands()),
                    "Start a StateMachine", "Name", "The Name of the Activated/Paused StateMachine"  );
        obj->commands()->addCommand( command( "pauseStateMachine", &ExecutionAccess::pauseStateMachine ,
                    &ExecutionAccess::true_gen , this, tc->engine()->commands()),
                    "Pause a StateMachine", "Name", "The Name of a Started StateMachine"  );
        obj->commands()->addCommand( command( "stopStateMachine", &ExecutionAccess::stopStateMachine ,
                    &ExecutionAccess::true_gen , this, tc->engine()->commands()),
                    "Stop a StateMachine", "Name", "The Name of the Started/Paused StateMachine"  );
        obj->commands()->addCommand( command( "resetStateMachine", &ExecutionAccess::resetStateMachine ,
                    &ExecutionAccess::true_gen , this, tc->engine()->commands()),
                    "Reset a StateMachine", "Name", "The Name of the Stopped StateMachine" );

        // request states
        obj->commands()->addCommand( command( "requestStateMachineState", &ExecutionAccess::requestStateMachineState ,
                                                    &ExecutionAccess::inStateMachineState , this, tc->engine()->commands()),
                                           "Request a State change",
                                           "Name", "The Name of the StateMachine",
                                           "StateName", "The Name of the State to change to" );
        parent->addObject( obj );
    }

    ParserExecutionAccess::~ParserExecutionAccess()
    {
    }
}
