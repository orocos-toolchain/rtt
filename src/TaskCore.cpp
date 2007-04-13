
#include "TaskCore.hpp"
#include "ActivityInterface.hpp"

namespace RTT
{
    
    using namespace std;

    TaskCore::TaskCore(const std::string& name)
        :  mtask_name(name),
           ee( new ExecutionEngine(this) )
           ,mTaskState(Stopped)

    {
    }

    TaskCore::TaskCore(const std::string& name, ExecutionEngine* parent )
        :  mtask_name(name),
           ee( parent )
           ,mTaskState(Stopped)

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

    bool TaskCore::configureHook() {
        return true;
    }
        
    bool TaskCore::start() {
        if ( this->engine()->getActivity() == 0 || mTaskState != Stopped )
            return false;
        if (this->engine()->getActivity()->start() ) {
            mTaskState = Running;
            return true;
        }
        return false;
    }

    bool TaskCore::stop() {
        if ( this->engine()->getActivity() == 0 || mTaskState != Running )
            return false;
        if (this->engine()->getActivity()->stop() ) {
            mTaskState = Stopped;
            return true;
        }
        return false;
    }

    bool TaskCore::cleanup() {
        if ( mTaskState == Stopped ) {
            cleanupHook();
            mTaskState = PreOperational;
            return true;
        }
        return false; // no cleanup when running or not configured.
    }

    void TaskCore::cleanupHook() {
    }
  
    bool TaskCore::isRunning() const {
        return mTaskState == Running;
    }

    bool TaskCore::isConfigured() const {
        return mTaskState >= Stopped;
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

