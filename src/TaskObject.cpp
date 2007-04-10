#include "TaskObject.hpp"
#include "ExecutionEngine.hpp"

namespace RTT
{

    TaskObject::TaskObject(std::string name, std::string description)
        : mname(name), mdescription(description), mparent(0)
    {}

    TaskObject::TaskObject(OperationInterface* parent, std::string name, std::string description)
        : mname(name), mdescription(description), mparent(0)
    {
        assert(parent);
        if ( parent->addObject( this ) )
            mparent = parent;
    }

    TaskObject::~TaskObject()
    {
        this->attributes()->clear();
    }

    void TaskObject::setEngine(ExecutionEngine* newengine) {
        if(newengine)
            mevents.setEventProcessor( newengine->events() );
        else
            mevents.setEventProcessor( 0 );
        // In case objects form circular links, this will run forever
        for (Objects::iterator it = mobjects.begin(); it != mobjects.end(); ++it)
            (*it)->setEngine(newengine);
    }

}
