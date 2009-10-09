#include "PortInterface.hpp"
#include "InputPortInterface.hpp"
#include "OutputPortInterface.hpp"
#include "../internal/TaskObject.hpp"
#include "../Method.hpp"
#include "../Logger.hpp"

using namespace RTT;
using namespace detail;
using namespace std;


InputPortInterface::InputPortInterface(std::string const& name, ConnPolicy const& default_policy)
: PortInterface(name)
  , cmanager(this)
  , default_policy( default_policy )
  , new_data_on_port_event(0) {}

InputPortInterface::~InputPortInterface()
{
    cmanager.disconnect();
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
    if (! output) {
        log(Error) << "InputPort "<< getName() <<" could not connect to "<< other.getName() << ": not an Output port." <<endlog();
        return false;
    }
    return output->createConnection(*this, policy);
}

bool InputPortInterface::connectTo(PortInterface& other)
{
    return connectTo(other, default_policy);
}

void InputPortInterface::startConnection(PortID* port_id, ChannelElementBase::shared_ptr channel_output)
{
    // this will call us back on connectionAdded.
    cmanager.addConnection( port_id, channel_output, default_policy);
}

bool InputPortInterface::channelReady(ChannelElementBase::shared_ptr channel)
{
    if ( channel && channel->inputReady() )
        return true;
    if (channel)
        channel->disconnect(false);

    return false;
}

#if 0
bool InputPortInterface::channelReady(PortInterface const& port)
{
    // NOTE: we can't get hold of the ChannelElement of port, and even
    // if we could, we wouldn't know which one of the many to use (output goes to many inputs).
    // furthermore, CORBA tricks the C++ layer with 'empty' ports that
    // just serve to be passed on for PortID comparison.
    // So don't touch this line unless you know what you're doing.

    // Ask connection manager to iterate over all connections and find
    // out which one points to port:
    return channelReady( cmanager.getConnection(port) );
}
#endif

void InputPortInterface::removeConnection(ChannelElementBase::shared_ptr channel)
{
    cmanager.removeConnection(channel);
}

FlowStatus InputPortInterface::read(DataSourceBase::shared_ptr source)
{ throw std::runtime_error("calling default InputPortInterface::read(datasource) implementation"); }
/** Returns true if this port is connected */
bool InputPortInterface::connected() const
{ return cmanager.connected(); }

void InputPortInterface::clear()
{
    cmanager.clear();
}

void InputPortInterface::disconnect()
{
    cmanager.disconnect();
}

void InputPortInterface::disconnect(PortInterface& port)
{
    cmanager.disconnect(port);
}

