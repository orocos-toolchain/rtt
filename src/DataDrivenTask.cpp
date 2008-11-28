#include "DataDrivenTask.hpp"
#include "EventDrivenActivity.hpp"

using namespace RTT;

DataDrivenTask::DataDrivenTask( const std::string& name, TaskState initial_state )
    : TaskContext(name, initial_state) {}

DataDrivenTask::DataDrivenTask(const std::string& name, ExecutionEngine* parent, TaskState initial_state )
    : TaskContext(name, parent, initial_state) {}

bool DataDrivenTask::start()
{
    size_t port_count = 0;
    const DataFlowInterface::Ports& ports = this->ports()->getEventPorts();
    for (DataFlowInterface::Ports::const_iterator it = ports.begin(); it != ports.end(); ++it)
    {
        int porttype = (*it)->getPortType();
        if (porttype == PortInterface::ReadPort || porttype == PortInterface::ReadWritePort)
        {
            (*it)->getNewDataOnPortEvent()->connect(boost::bind(&DataDrivenTask::dataOnPort, this, _1), this->events()->getEventProcessor());
            port_count++;
            log(Info) << getName() << " will be triggered when new data is available on " << (*it)->getName() << endlog();
        }
    }
    updated_ports.reserve(port_count);
    return TaskContext::start();
}

void DataDrivenTask::dataOnPort(PortInterface* port)
{
    // Since this handler is executed in our thread, we are always running.
    if (find(updated_ports.begin(), updated_ports.end(), port) == updated_ports.end() )
        updated_ports.push_back(port);
    // this is in essence superfluous. We are already triggered.
    //this->getActivity()->trigger();
}

void DataDrivenTask::updateHook()
{
    updateHook(updated_ports);
    updated_ports.clear();
}

void DataDrivenTask::updateHook(std::vector<PortInterface*> const& updated_ports)
{
}

