#include "PortInterface.hpp"
#include "../internal/TaskObject.hpp"
#include "../Method.hpp"

using namespace RTT;
using namespace detail;
using namespace std;

namespace {

    struct LocalPortID : public PortID
    {
        PortInterface const* ptr;
        LocalPortID(PortInterface const* obj)
            : ptr(obj) {}
    };
}

 

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

bool PortInterface::isSameID(PortID const& id) const
{ 
    LocalPortID const* real_id = dynamic_cast<LocalPortID const*>(&id);
    if (!real_id)
        return false;
    else return real_id->ptr == this;
}
PortID* PortInterface::getPortID() const
{ return new LocalPortID(this); }

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

