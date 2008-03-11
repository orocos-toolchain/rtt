/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:25 CET 2008  ExecutionAccess.cpp 

                        ExecutionAccess.cpp -  description
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
 
 
#include "ExecutionAccess.hpp"
#include "TaskContext.hpp"
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

