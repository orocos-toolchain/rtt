#ifndef ORO_SCRIPTING_HPP_
#define ORO_SCRIPTING_HPP_

#include "../interface/ServiceRequester.hpp"
#include "../base/ProgramInterface.hpp"
#include "StateMachine.hpp"
#include "../Method.hpp"
#include <string>

namespace RTT
{

    /**
     * The method interface of the scripting plugin.
     */
    class RTT_API Scripting
        : public interface::ServiceRequester
    {
    public:
        Scripting(TaskContext* owner);
        ~Scripting();
        typedef base::ProgramInterface::Status ProgramStatus;
        typedef scripting::StateMachine::Status StateMachineStatus;

        Method<bool (const std::string& )> unloadProgram;

        Method<ProgramStatus::ProgramStatus (const std::string& )> getProgramStatus;

        Method<std::string (const std::string& )> getProgramStatusStr;

        Method<std::vector<std::string>(void)> getProgramList;

        Method<bool ( const std::string&  )> unloadStateMachine;

        Method<StateMachineStatus::StateMachineStatus (const std::string& )> getStateMachineStatus;

        Method<std::string (const std::string& )> getStateMachineStatusStr;

        Method<std::vector<std::string> ()> getStateMachineList;

        Method<int (const std::string&  )> execute;

        Method<bool ( const std::string&)> loadPrograms;

        Method<bool ( const std::string& )> loadProgramText;

        Method<int (const std::string& )> getProgramLine;

        Method<std::string (const std::string&  )> getProgramText;

        Method<bool ( const std::string& )> loadStateMachines;

        Method<bool ( const std::string& code )> loadStateMachineText;

        Method<std::string (const std::string&  )> getStateMachineText;

        Method<int (const std::string&  )> getStateMachineLine;

        Method<bool (const std::string& )> startProgram;

        Method<bool (const std::string& )> stopProgram;

        Method<bool (const std::string& )> pauseProgram;

        Method<bool (const std::string& )> stepProgram;

        Method<bool (const std::string& )> hasProgram;

        Method<bool (const std::string& )> isProgramRunning;

        Method<bool (const std::string& )> isProgramPaused;

        Method<bool (const std::string& )> inProgramError;

        Method<bool (const std::string& )> activateStateMachine;

        Method<bool (const std::string& )> deactivateStateMachine;

        Method<bool (const std::string& )> startStateMachine;

        Method<bool (const std::string& )> pauseStateMachine;

        Method<bool (const std::string& )> stopStateMachine;

        Method<bool (const std::string& )> resetStateMachine;

        Method<bool (const std::string& , const std::string& )> requestStateMachineState;

        Method<bool (const std::string& )> hasStateMachine;

        Method<bool (const std::string& )> isStateMachineActive;

        Method<bool (const std::string& )> isStateMachineRunning;

        Method<bool (const std::string& )> inStateMachineError;

        Method<bool (const std::string& )> isStateMachinePaused;

        Method<std::string (const std::string& )> getStateMachineState;

        Method<bool (const std::string& , const std::string& )> inStateMachineState;
    };

}

#endif /* ORO_SCRIPTING_HPP_ */
