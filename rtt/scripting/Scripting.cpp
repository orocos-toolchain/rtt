/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  Scripting.cpp

                        Scripting.cpp -  description
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


#include "Scripting.hpp"

namespace RTT
{
    /*
      unloadProgram
      getProgramStatus
      getProgramStatusStr
      getProgramList
      unloadStateMachine
      getStateMachineStatus
      getStateMachineStatusStr
      getStateMachineList
      getStateMachine
      getStateMachine
      execute
      runScript
      eval
      loadPrograms
      loadProgramText
      unloadProgram
      getProgramLine
      getProgramText
      loadStateMachines
      loadStateMachineText
      unloadStateMachine
      getStateMachineText
      getStateMachineLine
      getCurrentState
      startProgram
      stopProgram
      pauseProgram
      stepProgram
      hasProgram
      isProgramRunning
      isProgramPaused
      inProgramError
      activateStateMachine
      deactivateStateMachine
      startStateMachine
      pauseStateMachine
      stopStateMachine
      resetStateMachine
      requestStateMachineState
      hasStateMachine
      isStateMachineActive
      isStateMachineRunning
      inStateMachineError
      isStateMachinePaused
      getStateMachineState
      inStateMachineState
    */
    Scripting::Scripting(TaskContext* owner)
        : ServiceRequester("scripting", owner),
          unloadProgram("unloadProgram"),
          getProgramStatus("getProgramStatus"),
          getProgramStatusStr("getProgramStatusStr"),
          getProgramList("getProgramList"),
          unloadStateMachine("unloadStateMachine"),
          getStateMachineStatus("getStateMachineStatus"),
          getStateMachineStatusStr("getStateMachineStatusStr"),
          getStateMachineList("getStateMachineList"),
          execute("execute"),
          eval("eval"),
          runScript("runScript"),
          loadPrograms("loadPrograms"),
          loadProgramText("loadProgramText"),
          getProgramLine("getProgramLine"),
          getProgramText("getProgramText"),
          loadStateMachines("loadStateMachines"),
          loadStateMachineText("loadStateMachineText"),
          getStateMachineText("getStateMachineText"),
          getStateMachineLine("getStateMachineLine"),
          startProgram("startProgram"),
          stopProgram("stopProgram"),
          pauseProgram("pauseProgram"),
          stepProgram("stepProgram"),
          hasProgram("hasProgram"),
          isProgramRunning("isProgramRunning"),
          isProgramPaused("isProgramPaused"),
          inProgramError("inProgramError"),
          activateStateMachine("activateStateMachine"),
          deactivateStateMachine("deactivateStateMachine"),
          startStateMachine("startStateMachine"),
          pauseStateMachine("pauseStateMachine"),
          stopStateMachine("stopStateMachine"),
          resetStateMachine("resetStateMachine"),
          requestStateMachineState("requestStateMachineState"),
          hasStateMachine("hasStateMachine"),
          isStateMachineActive("isStateMachineActive"),
          isStateMachineRunning("isStateMachineRunning"),
          inStateMachineError("inStateMachineError"),
          isStateMachinePaused("isStateMachinePaused"),
          getStateMachineState("getStateMachineState"),
          inStateMachineState("inStateMachineState")
    {
        addOperationCaller(unloadProgram);
        addOperationCaller(getProgramStatus);
        addOperationCaller(getProgramStatusStr);
        addOperationCaller(getProgramList);
        addOperationCaller(unloadStateMachine);
        addOperationCaller(getStateMachineStatus);
        addOperationCaller(getStateMachineStatusStr);
        addOperationCaller(getStateMachineList);
        addOperationCaller(execute);
        addOperationCaller(eval);
        addOperationCaller(runScript);
        addOperationCaller(loadPrograms);
        addOperationCaller(loadProgramText);
        addOperationCaller(getProgramLine);
        addOperationCaller(getProgramText);
        addOperationCaller(loadStateMachines);
        addOperationCaller(loadStateMachineText);
        addOperationCaller(getStateMachineText);
        addOperationCaller(getStateMachineLine);
        addOperationCaller(startProgram);
        addOperationCaller(stopProgram);
        addOperationCaller(pauseProgram);
        addOperationCaller(stepProgram);
        addOperationCaller(hasProgram);
        addOperationCaller(isProgramRunning);
        addOperationCaller(isProgramPaused);
        addOperationCaller(inProgramError);
        addOperationCaller(activateStateMachine);
        addOperationCaller(deactivateStateMachine);
        addOperationCaller(startStateMachine);
        addOperationCaller(pauseStateMachine);
        addOperationCaller(stopStateMachine);
        addOperationCaller(resetStateMachine);
        addOperationCaller(requestStateMachineState);
        addOperationCaller(hasStateMachine);
        addOperationCaller(isStateMachineActive);
        addOperationCaller(isStateMachineRunning);
        addOperationCaller(inStateMachineError);
        addOperationCaller(isStateMachinePaused);
        addOperationCaller(getStateMachineState);
        addOperationCaller(inStateMachineState);
    }

    Scripting::~Scripting()
    {
    }

}
