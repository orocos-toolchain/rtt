#include "PortInterface.hpp"
#include "InputPortInterface.hpp"
#include "OutputPortInterface.hpp"
#include "../internal/TaskObject.hpp"
#include "../Method.hpp"

using namespace RTT;
using namespace detail;
using namespace std;


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

bool InputPortInterface::connectTo(PortInterface& other)
{
    OutputPortInterface* output = dynamic_cast<OutputPortInterface*>(&other);
    if (! output)
        return false;
    return output->createConnection(*this);
}

void InputPortInterface::setInputChannel(ChannelElementBase* channel_output)
{
    this->channel = channel_output;
}

bool InputPortInterface::channelsReady()
{
    if ( this->channel && this->channel->inputReady() )
        return true;
    this->disconnect();
    return false;
}

void InputPortInterface::clearInputChannel()
{
    this->channel = 0;
}

FlowStatus InputPortInterface::read(DataSourceBase::shared_ptr source)
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

