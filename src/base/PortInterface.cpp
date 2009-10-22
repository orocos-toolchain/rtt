#include "PortInterface.hpp"
#include "../internal/TaskObject.hpp"
#include "../Method.hpp"
#include "../internal/ConnFactory.hpp"

using namespace RTT;
using namespace detail;
using namespace std;

PortInterface::PortInterface(const std::string& name)
    : name(name), iface(0) {}

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

ConnID* PortInterface::getPortID() const
{ return new LocalConnID(this); }

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

void PortInterface::setInterface(DataFlowInterface* dfi) {
    iface = dfi;
}

interface::DataFlowInterface* PortInterface::getInterface() const
{
    return iface;
}

