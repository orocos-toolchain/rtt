/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:25 CET 2008  TaskCore.cpp

                        TaskCore.cpp -  description
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



#include "TaskCore.hpp"
#include "../ExecutionEngine.hpp"
#include "ActivityInterface.hpp"
#include "Logger.hpp"

namespace RTT {
    using namespace detail;

    using namespace std;

    TaskCore::TaskCore(TaskState initial_state /*= Stopped*/ )
        :  ee( new ExecutionEngine(this) )
           ,mTaskState(initial_state)
           ,mInitialState(initial_state)
           ,mTargetState(initial_state)
    {
    }

    TaskCore::TaskCore( ExecutionEngine* parent, TaskState initial_state /*= Stopped*/  )
        :  ee( parent )
           ,mTaskState(initial_state)
           ,mInitialState(initial_state)
           ,mTargetState(initial_state)
    {
        parent->addChild( this );
    }


    TaskCore::~TaskCore()
    {
        if ( ee->getParent() == this ) {
            delete ee;
        } else {
            ee->removeChild(this);
        }
        // Note: calling cleanup() here has no use or even dangerous, as
        // cleanupHook() is a virtual function and the user code is already
        // destroyed. The user's subclass is responsible to make this state
        // transition in its destructor if required.
    }

    TaskCore::TaskState TaskCore::getTaskState() const {
        return mTaskState;
    }

    TaskCore::TaskState TaskCore::getTargetState() const {
        return mTargetState;
    }

    bool TaskCore::update()
    {
        if ( !this->engine()->getActivity() )
            return false;
        return this->engine()->getActivity()->execute();
    }

    bool TaskCore::trigger()
    {
        if ( !this->engine()->getActivity() )
            return false;
        return this->engine()->getActivity()->trigger();
    }

    bool TaskCore::configure() {
        if ( mTaskState == Stopped || mTaskState == PreOperational) {
            try {
                mTargetState = Stopped;
                if (configureHook() ) {
                    mTaskState = Stopped;
                    return true;
                } else {
                    mTargetState = mTaskState = PreOperational;
                    return false;
                }
            } catch(...) {
                exception();
            }
        }
        return false; // no configure when running.
    }

    bool TaskCore::cleanup() {
        if ( mTaskState == Stopped ) {
            try {
                mTargetState = PreOperational;
                cleanupHook();
                mTaskState = PreOperational;
                return true;
            } catch (...) {
                exception();
            }
        }
        return false; // no cleanup when running or not configured.
    }

    void TaskCore::fatal() {
        mTargetState = mTaskState = FatalError;
        if ( engine()->getActivity() )
            engine()->getActivity()->stop();
    }

    void TaskCore::error() {
        // detects error() from within start():
        if (mTargetState < Running)
            return;
        mTargetState = mTaskState = RunTimeError;
    }

    void TaskCore::exception() {
        //log(Error) <<"Exception happend in TaskCore."<<endlog();
        TaskState copy = mTaskState;
        mTargetState = mTaskState = Exception;
        try {
            if ( copy >= Running ) {
                stopHook();
            }
            if ( copy >= Stopped ) {
                cleanupHook();
            }
            exceptionHook();
        } catch(std::exception const& e) {
            log(RTT::Error) << "stopHook(), cleanupHook() and/or exceptionHook() raised " << e.what() << ", going into Fatal" << endlog();
            fatal();
        }
        catch (...) {
            log(Error) << "stopHook(), cleanupHook() and/or exceptionHook() raised an exception, going into Fatal" << endlog();
            fatal();
        }
    }

    bool TaskCore::recover() {
        if ( mTaskState == Exception ) {
            mTargetState = mTaskState = mInitialState;
            return true;
        }
        if (mTaskState == RunTimeError ) {
            mTargetState = mTaskState = Running;
            return true;
        }
        return false;
    }

    bool TaskCore::start() {
        if ( mTaskState == Stopped ) {
            try {
                mTargetState = Running;
                if ( startHook() ) {
                    mTaskState = Running;
                    return true;
                }
                mTargetState = Stopped;
            } catch(...) {
                exception();
            }
        }
        return false;
    }

    bool TaskCore::stop() {
        if ( mTaskState >= Running ) {
            try {
                mTargetState = Stopped;
                stopHook();
                mTaskState = Stopped;
                return true;
            } catch(...) {
                exception();
            }
        }
        return false;
    }

    bool TaskCore::activate() {
        this->engine() && this->engine()->getActivity() && this->engine()->getActivity()->start();
        return isActive();
    }

    void TaskCore::cleanupHook() {
    }

    bool TaskCore::isRunning() const {
        return mTaskState >= Running;
    }

    bool TaskCore::isConfigured() const {
        return mTaskState >= Stopped;
    }

    bool TaskCore::inFatalError() const {
        return mTaskState == FatalError;
    }

    bool TaskCore::inException() const {
        return mTaskState == Exception;
    }

    bool TaskCore::inRunTimeError() const {
        return mTaskState == RunTimeError;
    }

    bool TaskCore::isActive() const
    {
        return this->engine() && this->engine()->getActivity() && this->engine()->getActivity()->isActive();
    }

    double TaskCore::getPeriod() const
    {
        return this->engine()->getActivity() ? this->engine()->getActivity()->getPeriod() : -1.0;
    }

    bool TaskCore::setPeriod(Seconds s)
    {
        return this->engine()->getActivity() ? this->engine()->getActivity()->setPeriod(s) : false;
    }

    bool TaskCore::configureHook() {
        return true;
    }

    bool TaskCore::startHook()
    {
        return true;
    }

    void TaskCore::errorHook() {
    }

    void TaskCore::prepareUpdateHook() {
    }

    void TaskCore::updateHook()
    {
    }

    bool TaskCore::breakUpdateHook()
    {
        return false;
    }

    void TaskCore::exceptionHook() {
    }

    void TaskCore::stopHook()
    {
    }

    void TaskCore::setExecutionEngine(ExecutionEngine* engine) {
        if ( ee == engine )
            return;
        // cleanup:
        if ( ee->getParent() == this )
            delete ee;
        else
            ee->removeChild(this);
        // set new:
        if ( engine ) {
            this->ee = engine;
            engine->addChild(this);
        } else {
            this->ee = new ExecutionEngine(this);
        }
    }

}

