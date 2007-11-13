
#include "TaskCore.hpp"
#include "ExecutionEngine.hpp"
#include "ActivityInterface.hpp"

namespace RTT
{
    
    using namespace std;

    TaskCore::TaskCore(const std::string& name, TaskState initial_state /*= Stopped*/ )
        :  mtask_name(name),
           ee( new ExecutionEngine(this) )
           ,mTaskState(initial_state)
           ,runtime_warnings(0), runtime_errors(0)
    {
    }

    TaskCore::TaskCore(const std::string& name, ExecutionEngine* parent, TaskState initial_state /*= Stopped*/  )
        :  mtask_name(name),
           ee( parent )
           ,mTaskState(initial_state)
           ,runtime_warnings(0), runtime_errors(0)
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


    bool TaskCore::doUpdate()
    {
        if ( this->engine()->getActivity() == 0 )
            return false;
        return this->engine()->getActivity()->execute();
    }

    bool TaskCore::doTrigger()
    {
        if ( this->engine()->getActivity() == 0 )
            return false;
        return this->engine()->getActivity()->trigger();
    }

    bool TaskCore::configure() {
        if ( mTaskState <= Stopped ) {
            if (configureHook() ) {
                mTaskState = Stopped;
                return true;
            } else {
                mTaskState = PreOperational;
                return false;
            }
        }
        return false; // no configure when running.
    }

    bool TaskCore::cleanup() {
        if ( mTaskState == Stopped ) {
            cleanupHook();
            mTaskState = PreOperational;
            runtime_warnings = 0;
            runtime_errors = 0;
            return true;
        }
        return false; // no cleanup when running or not configured.
    }

    void TaskCore::fatal() {
        mTaskState = FatalError;
    }

    void TaskCore::warning() {
        if (mTaskState < Running )
            return;
        ++runtime_warnings;
        mTaskState = RunTimeWarning;
    }

    void TaskCore::error() {
        if (mTaskState < Running )
            return;
        ++runtime_errors;
        mTaskState = RunTimeError;
    }

    void TaskCore::recovered() {
        if (mTaskState <= Running )
            return;
        mTaskState = Running;
    }

    int TaskCore::getWarningCount() const
    {
        return runtime_warnings;
    }

    int TaskCore::getErrorCount() const
    {
        return runtime_errors;
    }

    bool TaskCore::resetError() {
        if (mTaskState == FatalError ) {
            if ( resetHook() == true )
                mTaskState = Stopped;
            else
                mTaskState = PreOperational;
            return true;
        }
        return false;
    }

    bool TaskCore::start() {
        return this->engine()->start();
    }

    bool TaskCore::stop() {
        return this->engine()->stop();
    }

    bool TaskCore::activate() {
        return this->engine()->activate();
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

    bool TaskCore::inRunTimeError() const {
        return mTaskState == RunTimeError;
    }

    bool TaskCore::inRunTimeWarning() const {
        return mTaskState == RunTimeWarning;
    }

    bool TaskCore::isActive() const
    {
        return mTaskState >= Active;
    }

    double TaskCore::getPeriod() const
    {
        return this->engine()->getActivity() ? this->engine()->getActivity()->getPeriod() : -1.0;
    }

    bool TaskCore::configureHook() {
        return true;
    }
        
    void TaskCore::errorHook() {
    }
        
    bool TaskCore::startup()
    {
        return true;
    }
    void TaskCore::update()
    {}
    void TaskCore::shutdown()
    {}

    bool TaskCore::startHook()
    {
        return startup();
    }

    bool TaskCore::resetHook()
    {
        return true;
    }

    bool TaskCore::activateHook()
    {
        return true;
    }

    void TaskCore::updateHook()
    {
        update();
    }

    void TaskCore::stopHook()
    {
        shutdown();
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

