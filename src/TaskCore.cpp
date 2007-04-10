
#include "TaskCore.hpp"

namespace RTT
{
    
    using namespace std;

    TaskCore::TaskCore(const std::string& name)
        :  mtask_name(name),
           ee( new ExecutionEngine(this) )
    {
    }

    TaskCore::TaskCore(const std::string& name, ExecutionEngine* parent )
        :  mtask_name(name),
           ee( parent )
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

