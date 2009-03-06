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
    : PortInterface(name) { }

WritePortInterface::~WritePortInterface()
{
    disconnect();
}

/** Returns true if this port is connected */
bool WritePortInterface::connected() const
{ return !connections.empty(); }

void WritePortInterface::disconnect(PortInterface& port)
{
    for (list<ConnDescriptor>::iterator it = connections.begin(); it != connections.end(); ++it)
    {
        if (it->get<0>() == &port)
        {
            it->get<1>()->disconnect(true);
            connections.erase(it);
            return;
        }
    }
}

/** Removes any connection that either go to or come from this port */
void WritePortInterface::disconnect()
{
    list<ConnDescriptor> connections = this->connections;
    this->connections.clear();

    for (list<ConnDescriptor>::iterator it = connections.begin(); it != connections.end(); ++it)
        it->get<1>()->disconnect(true);
}

/** Connects this write port to the given read port, using a single-data
 * policy with the given locking mechanism */
bool WritePortInterface::createDataConnection( ReadPortInterface& reader, int lock_policy )
{ return createConnection( reader, ConnPolicy::data(lock_policy) ); }

/** Connects this write port to the given read port, using a buffered
 * policy, with the buffer of the given size and the given locking
 * mechanism */
bool WritePortInterface::createBufferConnection( ReadPortInterface& reader, int size, int lock_policy )
{ return createConnection( reader, ConnPolicy::buffer(size, lock_policy) ); }

/** Connects this write port to the given read port, using the as policy
 * the default policy of the reader port
 */
bool WritePortInterface::createConnection( ReadPortInterface& reader )
{ return createConnection(reader, reader.getDefaultPolicy()); }

