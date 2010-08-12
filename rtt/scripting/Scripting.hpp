#ifndef ORO_SCRIPTING_HPP_
#define ORO_SCRIPTING_HPP_

#include "../interface/ServiceRequester.hpp"
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
        : public interface::ServiceRequester
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
