#include "DataDrivenTask.hpp"
#include "EventDrivenActivity.hpp"

using namespace RTT;

DataDrivenTask::DataDrivenTask( const std::string& name, TaskState initial_state )
    : TaskContext(name, initial_state) {}

DataDrivenTask::DataDrivenTask(const std::string& name, ExecutionEngine* parent, TaskState initial_state )
    : TaskContext(name, parent, initial_state) {}

bool DataDrivenTask::start()
{
    EventDrivenActivity* activity = dynamic_cast<EventDrivenActivity*>(engine()->getActivity());
    if (activity)
    {
        log(Info) << getName() << " is attached to an EventDrivenActivity. Registering ports." << endlog();
        DataFlowInterface::Ports ports = this->ports()->getPorts();
        for (DataFlowInterface::Ports::iterator it = ports.begin(); it != ports.end(); ++it)
        {
            int porttype = (*it)->getPortType();
            if (porttype == PortInterface::ReadPort || porttype == PortInterface::ReadWritePort)
            {
                PortInterface::NewDataEvent* ev = (*it)->getNewDataEvent();
                if (!activity->addEvent(ev))
                    return false;

                log(Info) << getName() << " will be triggered when new data is available on " << (*it)->getName() << endlog();
            }
        }
    }
    else
    {
        log(Warning) << "activity is not an EventDrivenActivity" << endlog();
    }

    return TaskContext::start();
}

