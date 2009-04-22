#include "PortInterface.hpp"
#include "TaskObject.hpp"
#include "Method.hpp"

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

ChannelFactory* PortInterface::getConnFactory() { return 0; }
bool PortInterface::isLocal() const
{ return serverProtocol() == 0; }
int PortInterface::serverProtocol() const
{ return 0; }

bool PortInterface::connectTo(PortInterface& other, ConnPolicy const& policy)
{
    OutputPortInterface* writer;
    InputPortInterface* reader;
    
    writer = dynamic_cast<OutputPortInterface*>(this);
    if (writer)
        reader = dynamic_cast<InputPortInterface*>(&other);
    else
    {
        writer = dynamic_cast<OutputPortInterface*>(&other);
        reader = dynamic_cast<InputPortInterface*>(this);
    }

    if (writer && reader)
        return writer->createConnection(*reader, policy);
    else return false;
}

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
    , writer(NULL) 
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

bool InputPortInterface::read(DataSourceBase::shared_ptr source)
{ throw std::runtime_error("calling default InputPortInterface::read(datasource) implementation"); }
/** Returns true if this port is connected */
bool InputPortInterface::connected() const
{ return writer; }

void InputPortInterface::clear()
{ if (writer) writer->clear(); }

void InputPortInterface::disconnect()
{
    ChannelElementBase::shared_ptr source = this->writer;
    this->writer = 0;
    if (source)
        source->disconnect(false);
}

OutputPortInterface::OutputPortInterface(std::string const& name)
    : PortInterface(name), connections(10) { }

OutputPortInterface::~OutputPortInterface()
{
    disconnect();
}

/** Returns true if this port is connected */
bool OutputPortInterface::connected() const
{ return !connections.empty(); }

bool OutputPortInterface::eraseMatchingConnection(PortInterface const* port, ChannelDescriptor& descriptor)
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
    connections.delete_if( boost::bind(&OutputPortInterface::eraseMatchingConnection, this, &port, _1) );
}

bool OutputPortInterface::eraseConnection(ChannelDescriptor& descriptor)
{
    descriptor.get<1>()->disconnect(true);
    delete descriptor.get<0>();
    return true;
}

void OutputPortInterface::disconnect()
{
    connections.delete_if( boost::bind(&OutputPortInterface::eraseConnection, this, _1) );
}

void OutputPortInterface::addConnection(ChannelDescriptor const& descriptor)
{
    if (!connections.append(descriptor))
    { OS::MutexLock locker(connection_resize_mtx);
        connections.grow(1);
        connections.append(descriptor);
    }
}


void OutputPortInterface::write(DataSourceBase::shared_ptr source)
{ throw std::runtime_error("calling default OutputPortInterface::write(datasource) implementation"); }
bool OutputPortInterface::createDataConnection( InputPortInterface& reader, int lock_policy )
{ return createConnection( reader, ConnPolicy::data(lock_policy) ); }

bool OutputPortInterface::createBufferConnection( InputPortInterface& reader, int size, int lock_policy )
{ return createConnection( reader, ConnPolicy::buffer(size, lock_policy) ); }

bool OutputPortInterface::createConnection( InputPortInterface& reader )
{ return createConnection(reader, reader.getDefaultPolicy()); }

