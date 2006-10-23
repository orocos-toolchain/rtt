#include "ExecutionAccess.hpp"
#include "OperationInterface.hpp"
#include "Command.hpp"
#include "Method.hpp"
#include "NA.hpp"

namespace RTT
{
    using namespace detail;
    using namespace boost;
    using namespace std;

    ExecutionAccess::ExecutionAccess( TaskContext* p )
        : tc(p)
    {
    }

    ExecutionAccess::~ExecutionAccess()
    {
    }

    bool ExecutionAccess::startProgram(const std::string& name)
    {
        ProgramInterfacePtr pi = tc->engine()->programs()->getProgram(name);
        if (pi)
            return pi->start();
        return false;
    }

    bool ExecutionAccess::hasProgram(const string& name) const {
#ifdef OROPKG_EXECUTION_ENGINE_PROGRAMS
        return tc->engine()->programs()->getProgram(name);
#else
        return false;
#endif
    }

    bool ExecutionAccess::isProgramRunning(const std::string& name) const
    {
        ProgramInterfacePtr pi = tc->engine()->programs()->getProgram(name);
        if (pi)
            return pi->isRunning();
        return false;
    }

    bool ExecutionAccess::isProgramPaused(const std::string& name) const
    {
        ProgramInterfacePtr pi = tc->engine()->programs()->getProgram(name);
        if (pi)
            return pi->isPaused();
        return false;
    }

    bool ExecutionAccess::inProgramError(const std::string& name) const
    {
        ProgramInterfacePtr pi = tc->engine()->programs()->getProgram(name);
        if (pi)
            return pi->inError();
        return false;
    }

    bool ExecutionAccess::stopProgram(const std::string& name)
    {
        ProgramInterfacePtr pi = tc->engine()->programs()->getProgram(name);
        if (pi)
            return pi->stop();
        return false;
    }

    bool ExecutionAccess::pauseProgram(const std::string& name)
    {
        ProgramInterfacePtr pi = tc->engine()->programs()->getProgram(name);
        if (pi)
            return pi->pause();
        return false;
    }

    bool ExecutionAccess::stepProgram(const std::string& name)
    {
        ProgramInterfacePtr pi = tc->engine()->programs()->getProgram(name);
        if (pi)
            return pi->step();
        return false;
    }

    bool ExecutionAccess::hasStateMachine(const std::string& name) const {
#ifdef OROPKG_EXECUTION_ENGINE_STATEMACHINES
        return tc->engine()->states()->getStateMachine(name);
#else
        return false;
#endif
    }

    bool ExecutionAccess::activateStateMachine(const std::string& name)
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->activate();
        return false;
    }

    bool ExecutionAccess::deactivateStateMachine(const std::string& name)
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->deactivate();
        return false;
    }

    bool ExecutionAccess::startStateMachine(const std::string& name)
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->start();
        return false;
    }

    bool ExecutionAccess::pauseStateMachine(const std::string& name)
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->pause();
        return false;
    }

    bool ExecutionAccess::stopStateMachine(const std::string& name)
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->stop();
        return false;
    }

    bool ExecutionAccess::isStateMachinePaused(const std::string& name) const
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->isPaused();
        return false;
    }

    bool ExecutionAccess::isStateMachineActive(const std::string& name) const
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->isActive();
        return false;
    }

    bool ExecutionAccess::isStateMachineRunning(const std::string& name) const
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->isAutomatic();
        return false;
    }

    bool ExecutionAccess::inStateMachineError(const std::string& name) const
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->inError();
        return false;
    }

    const std::string& ExecutionAccess::getStateMachineState(const std::string& name) const
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->getCurrentStateName();
        return NA<const std::string&>::na();
    }

    bool ExecutionAccess::requestStateMachineState(const std::string& name, const std::string& state)
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->requestState(state);
        return false;
    }

    bool ExecutionAccess::inStateMachineState(const std::string& name, const std::string& state) const
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->inState(state);
        return false;
    }

    bool ExecutionAccess::resetStateMachine(const std::string& name)
    {
        StateMachinePtr sm = tc->engine()->states()->getStateMachine(name);
        if (sm)
            return sm->reset();
        return false;
    }

    bool ExecutionAccess::true_gen() const {return true;}

}

