#include "PortInterface.hpp"
#include "../internal/TaskObject.hpp"
#include "../Method.hpp"

using namespace RTT;
using namespace std;

PortInterface::PortInterface(const std::string& name)
    : name(name) {}

bool PortInterface::setName(const std::string& name)
{
    if ( !connected() ) {
        this->name = name;
        return true;
    }
    return false;
}

ConnFactory* PortInterface::getConnFactory() { return 0; }
bool PortInterface::isLocal() const
{ return serverProtocol() == 0; }
int PortInterface::serverProtocol() const
{ return 0; }

bool PortInterface::isSameID(RTT::PortID const& id) const
{ 
    PortID const* real_id = dynamic_cast<PortID const*>(&id);
    if (!real_id)
        return false;
    else return real_id->ptr == this;
}
RTT::PortID* PortInterface::getPortID() const
{ return new PortID(this); }

TaskObject* PortInterface::createPortObject()
{
#ifndef ORO_EMBEDDED
    TaskObject* to = new TaskObject( this->getName() );
    to->methods()->addMethod( method("name",&PortInterface::getName, this),
            "Returns the port name.");
    to->methods()->addMethod( method("connected",&PortInterface::connected, this),
            "Check if this port is connected and ready for use.");
    to->methods()->addMethod( method("disconnect",&PortInterface::disconnect, this),
            "Disconnects this port from any connection it is part of.");
    return to;
#else
    return 0;
#endif
}

InputPortInterface::InputPortInterface(std::string const& name, ConnPolicy const& default_policy)
    : PortInterface(name)
    , channel(NULL) 
    , default_policy(default_policy)
    , new_data_on_port_event(0) {}

InputPortInterface::~InputPortInterface()
{
    disconnect();
    if (new_data_on_port_event)
        delete new_data_on_port_event;
}

ConnPolicy InputPortInterface::getDefaultPolicy() const
{ return default_policy; }

InputPortInterface::NewDataOnPortEvent* InputPortInterface::getNewDataOnPortEvent()
{
    if (!new_data_on_port_event)
        new_data_on_port_event = new NewDataOnPortEvent( this->getName() );
    return new_data_on_port_event;
}

bool InputPortInterface::connectTo(PortInterface& other, ConnPolicy const& policy)
{
    OutputPortInterface* output = dynamic_cast<OutputPortInterface*>(&other);
    if (! output)
        return false;
    return output->createConnection(*this, policy);
}


void InputPortInterface::setInputChannel(ChannelElementBase* channel_output)
{ this->channel = channel_output; }
void InputPortInterface::clearInputChannel()
{ this->channel = 0; }

bool InputPortInterface::read(DataSourceBase::shared_ptr source)
{ throw std::runtime_error("calling default InputPortInterface::read(datasource) implementation"); }
/** Returns true if this port is connected */
bool InputPortInterface::connected() const
{ return channel; }

void InputPortInterface::clear()
{
    ChannelElementBase::shared_ptr channel = this->channel;
    if (channel)
        channel->clear();
}

void InputPortInterface::disconnect()
{
    ChannelElementBase::shared_ptr channel = this->channel;
    this->channel = 0;
    if (channel)
        channel->disconnect(false);
}

OutputPortInterface::OutputPortInterface(std::string const& name)
    : PortInterface(name), channels(10), connections(10) { }

OutputPortInterface::~OutputPortInterface()
{
    disconnect();
}

/** Returns true if this port is connected */
bool OutputPortInterface::connected() const
{ return !channels.empty(); }

bool OutputPortInterface::eraseIfMatchingPort(PortInterface const* port, ChannelDescriptor& descriptor)
{
    if (port->isSameID(*descriptor.get<0>()))
    {
        descriptor.get<1>()->disconnect(true);
        removeChannel(descriptor.get<1>());
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
    channels.clear();
    connections.delete_if( boost::bind(&OutputPortInterface::eraseConnection, this, _1) );
}

void OutputPortInterface::addConnection(RTT::PortID* port_id, ChannelElementBase::shared_ptr channel_input, ConnPolicy const& policy)
{
    ChannelDescriptor descriptor = boost::make_tuple(port_id, channel_input, policy);
    addChannel(descriptor.get<1>());
    if (!connections.append(descriptor))
    { OS::MutexLock locker(connection_resize_mtx);
        connections.grow(1);
        connections.append(descriptor);
    }
}

void OutputPortInterface::addChannel(ChannelElementBase::shared_ptr channel)
{
    if (!channels.append(channel))
    { OS::MutexLock locker(connection_resize_mtx);
        channels.grow(1);
        channels.append(channel);
    }
}

void OutputPortInterface::removeChannel(ChannelElementBase::shared_ptr channel)
{
    channels.delete_if( boost::bind(std::equal_to<ChannelElementBase::shared_ptr>(), channel, _1));
}

bool OutputPortInterface::matchConnectionChannel(ChannelElementBase::shared_ptr channel, ChannelDescriptor const& descriptor) const
{ return (channel == descriptor.get<1>()); }
bool OutputPortInterface::removeConnection(ChannelElementBase::shared_ptr channel)
{
    removeChannel(channel);
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

