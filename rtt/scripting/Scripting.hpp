/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  Scripting.hpp

                        Scripting.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#ifndef ORO_SCRIPTING_HPP_
#define ORO_SCRIPTING_HPP_

#include "../ServiceRequester.hpp"
#include "ProgramInterface.hpp"
#include "StateMachine.hpp"
#include "../OperationCaller.hpp"
#include <string>

namespace RTT
{

    /**
     * The method interface of the scripting plugin.
     */
    class RTT_SCRIPTING_API Scripting
        : public ServiceRequester
    {
    public:
        Scripting(TaskContext* owner);
        ~Scripting();
        typedef scripting::ProgramInterface::Status ProgramStatus;
        typedef scripting::StateMachine::Status StateMachineStatus;

        OperationCaller<bool (const std::string& )> unloadProgram;

        OperationCaller<ProgramStatus::ProgramStatus (const std::string& )> getProgramStatus;

        OperationCaller<std::string (const std::string& )> getProgramStatusStr;

        OperationCaller<std::vector<std::string>(void)> getProgramList;

        OperationCaller<bool ( const std::string&  )> unloadStateMachine;

        OperationCaller<StateMachineStatus::StateMachineStatus (const std::string& )> getStateMachineStatus;

        OperationCaller<std::string (const std::string& )> getStateMachineStatusStr;

        OperationCaller<std::vector<std::string> ()> getStateMachineList;

        OperationCaller<int (const std::string&  )> execute;

        OperationCaller<bool ( const std::string&)> loadPrograms;

        OperationCaller<bool ( const std::string& )> loadProgramText;

        OperationCaller<int (const std::string& )> getProgramLine;

        OperationCaller<std::string (const std::string&  )> getProgramText;

        OperationCaller<bool ( const std::string& )> loadStateMachines;

        OperationCaller<bool ( const std::string& code )> loadStateMachineText;

        OperationCaller<std::string (const std::string&  )> getStateMachineText;

        OperationCaller<int (const std::string&  )> getStateMachineLine;

        OperationCaller<bool (const std::string& )> startProgram;

        OperationCaller<bool (const std::string& )> stopProgram;

        OperationCaller<bool (const std::string& )> pauseProgram;

        OperationCaller<bool (const std::string& )> stepProgram;

        OperationCaller<bool (const std::string& )> hasProgram;

        OperationCaller<bool (const std::string& )> isProgramRunning;

        OperationCaller<bool (const std::string& )> isProgramPaused;

        OperationCaller<bool (const std::string& )> inProgramError;

        OperationCaller<bool (const std::string& )> activateStateMachine;

        OperationCaller<bool (const std::string& )> deactivateStateMachine;

        OperationCaller<bool (const std::string& )> startStateMachine;

        OperationCaller<bool (const std::string& )> pauseStateMachine;

        OperationCaller<bool (const std::string& )> stopStateMachine;

        OperationCaller<bool (const std::string& )> resetStateMachine;

        OperationCaller<bool (const std::string& , const std::string& )> requestStateMachineState;

        OperationCaller<bool (const std::string& )> hasStateMachine;

        OperationCaller<bool (const std::string& )> isStateMachineActive;

        OperationCaller<bool (const std::string& )> isStateMachineRunning;

        OperationCaller<bool (const std::string& )> inStateMachineError;

        OperationCaller<bool (const std::string& )> isStateMachinePaused;

        OperationCaller<std::string (const std::string& )> getStateMachineState;

        OperationCaller<bool (const std::string& , const std::string& )> inStateMachineState;
    };

}

#endif /* ORO_SCRIPTING_HPP_ */
