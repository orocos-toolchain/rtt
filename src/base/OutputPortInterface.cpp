#include "PortInterface.hpp"
#include "OutputPortInterface.hpp"
#include "InputPortInterface.hpp"
#include "../internal/TaskObject.hpp"
#include "../Method.hpp"

using namespace RTT;
using namespace detail;
using namespace std;


OutputPortInterface::OutputPortInterface(std::string const& name)
    : PortInterface(name), connections(10) { }

OutputPortInterface::~OutputPortInterface()
{
    disconnect();
}

/** Returns true if this port is connected */
bool OutputPortInterface::connected() const
{ return !connections.empty(); }

bool OutputPortInterface::eraseIfMatchingPort(PortInterface const* port, ChannelDescriptor& descriptor)
{
    if (port->isSameID(*descriptor.get<0>()))
    {
        descriptor.get<1>()->disconnect(true);
        delete descriptor.get<0>();
        return true;
    }
    else return false;
}

void OutputPortInterface::disconnect(PortInterface& port)
{
    connections.delete_if( boost::bind(&OutputPortInterface::eraseIfMatchingPort, this, &port, _1) );
}

bool OutputPortInterface::eraseConnection(OutputPortInterface::ChannelDescriptor& descriptor)
{
    descriptor.get<1>()->disconnect(true);
    delete descriptor.get<0>();
    return true;
}

void OutputPortInterface::disconnect()
{
    connections.delete_if( boost::bind(&OutputPortInterface::eraseConnection, this, _1) );
}

void OutputPortInterface::addConnection(PortID* port_id, ChannelElementBase::shared_ptr channel_input, ConnPolicy const& policy)
{
    ChannelDescriptor descriptor = boost::make_tuple(port_id, channel_input, policy);
    if (!connections.append(descriptor))
    { os::MutexLock locker(connection_resize_mtx);
        connections.grow(1);
        connections.append(descriptor);
    }
    this->connectionAdded(channel_input, policy);
}

bool OutputPortInterface::matchConnectionChannel(ChannelElementBase::shared_ptr channel, ChannelDescriptor const& descriptor) const
{ return (channel == descriptor.get<1>()); }
bool OutputPortInterface::removeConnection(ChannelElementBase::shared_ptr channel)
{
    return connections.delete_if( bind(&OutputPortInterface::matchConnectionChannel, this, channel, _1) );
}

void OutputPortInterface::write(DataSourceBase::shared_ptr source)
{ throw std::runtime_error("calling default OutputPortInterface::write(datasource) implementation"); }
bool OutputPortInterface::createDataConnection( InputPortInterface& input, int lock_policy )
{ return createConnection( input, ConnPolicy::data(lock_policy) ); }

bool OutputPortInterface::createBufferConnection( InputPortInterface& input, int size, int lock_policy )
{ return createConnection( input, ConnPolicy::buffer(size, lock_policy) ); }

bool OutputPortInterface::createConnection( InputPortInterface& input )
{ return createConnection(input, input.getDefaultPolicy()); }

bool OutputPortInterface::connectTo(PortInterface& other, ConnPolicy const& policy)
{
    InputPortInterface* input  = dynamic_cast<InputPortInterface*>(&other);
    if (! input)
        return false;
    return createConnection(*input, policy);
}

bool OutputPortInterface::connectTo(PortInterface& other)
{
    InputPortInterface* input  = dynamic_cast<InputPortInterface*>(&other);
    if (! input)
        return false;
    return createConnection(*input);
}
