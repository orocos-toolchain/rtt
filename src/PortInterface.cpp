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

ConnFactory* PortInterface::getConnFactory() { return 0; }
bool PortInterface::isLocal() const
{ return serverProtocol() == 0; }
int PortInterface::serverProtocol() const
{ return 0; }


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

ReadPortInterface::ReadPortInterface(std::string const& name, ConnPolicy const& default_policy)
    : PortInterface(name)
    , writer(NULL) 
    , default_policy(default_policy)
    , new_data_on_port_event(0) {}

ReadPortInterface::~ReadPortInterface()
{
    disconnect();
}

ConnPolicy ReadPortInterface::getDefaultPolicy() const
{ return default_policy; }

ReadPortInterface::NewDataOnPortEvent* ReadPortInterface::getNewDataOnPortEvent()
{
    if (!new_data_on_port_event)
        new_data_on_port_event = new NewDataOnPortEvent( this->getName() );
    return new_data_on_port_event;
}

/** Returns true if this port is connected */
bool ReadPortInterface::connected() const
{ return writer; }

void ReadPortInterface::clear()
{ if (writer) writer->clear(); }

void ReadPortInterface::disconnect()
{
    ConnElementBase::shared_ptr source = this->writer;
    this->writer = 0;
    if (source)
        source->disconnect(false);
}

WritePortInterface::WritePortInterface(std::string const& name)
    : PortInterface(name), connections(10) { }

WritePortInterface::~WritePortInterface()
{
    disconnect();
}

/** Returns true if this port is connected */
bool WritePortInterface::connected() const
{ return !connections.empty(); }

bool WritePortInterface::eraseMatchingConnection(PortInterface const* port, ConnDescriptor& descriptor)
{
    if (port == descriptor.get<0>())
    {
        descriptor.get<1>()->disconnect(true);
        return true;
    }
    else return false;
}
void WritePortInterface::disconnect(PortInterface& port)
{
    connections.delete_if( boost::bind(&WritePortInterface::eraseMatchingConnection, this, &port, _1) );
}

bool WritePortInterface::eraseConnection(ConnDescriptor& descriptor)
{
    descriptor.get<1>()->disconnect(true);
    return true;
}

void WritePortInterface::disconnect()
{
    connections.delete_if( boost::bind(&WritePortInterface::eraseConnection, this, _1) );
}

void WritePortInterface::addConnection(ConnDescriptor const& descriptor)
{
    if (!connections.append(descriptor))
    { OS::MutexLock locker(connection_resize_mtx);
        connections.grow(1);
        connections.append(descriptor);
    }
}

bool WritePortInterface::createDataConnection( ReadPortInterface& reader, int lock_policy )
{ return createConnection( reader, ConnPolicy::data(lock_policy) ); }

bool WritePortInterface::createBufferConnection( ReadPortInterface& reader, int size, int lock_policy )
{ return createConnection( reader, ConnPolicy::buffer(size, lock_policy) ); }

bool WritePortInterface::createConnection( ReadPortInterface& reader )
{ return createConnection(reader, reader.getDefaultPolicy()); }

