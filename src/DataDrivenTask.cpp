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
        size_t port_count = 0;
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
                port_count++;

                log(Info) << getName() << " will be triggered when new data is available on " << (*it)->getName() << endlog();
            }
        }
        updated_ports.resize(port_count);
    }
    else
    {
        log(Warning) << "activity is not an EventDrivenActivity" << endlog();
    }

    return TaskContext::start();
}

void DataDrivenTask::updateHook()
{
    EventDrivenActivity* activity = dynamic_cast<EventDrivenActivity*>(engine()->getActivity());
    if (activity)
    {
        typedef std::vector< Event<void()>* > TriggerSet;
        TriggerSet const& updates = activity->getWakeupEvents();
        updated_ports.clear();

        for (TriggerSet::const_iterator it = updates.begin(); it != updates.end(); ++it)
        {
            PortInterface::NewDataEvent* ev = static_cast<PortInterface::NewDataEvent*>(*it);
            if (ev)
                updated_ports.push_back(ev->getPort());
        }
    }

    updateHook(updated_ports);
}

void DataDrivenTask::updateHook(std::vector<PortInterface*> const& updated_ports)
{
}

