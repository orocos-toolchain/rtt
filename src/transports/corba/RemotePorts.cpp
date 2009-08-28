#include "RemotePorts.hpp"
#include "CorbaTypeTransporter.hpp"
#include "DataFlowI.h"

using namespace RTT::corba;
using namespace RTT::base;

namespace {
    struct RemotePortID : public PortID
    {
        CDataFlowInterface_var dataflow;
        std::string name;
        
        RemotePortID(CDataFlowInterface_ptr dataflow, std::string const& name);
    };

    RemotePortID::RemotePortID(CDataFlowInterface_ptr dataflow, std::string const& name)
        : dataflow(CDataFlowInterface::_duplicate(dataflow))
        , name(name) {}

}

static RTT::corba::CConnPolicy toCORBA(RTT::internal::ConnPolicy const& policy)
{
    RTT::corba::CConnPolicy corba_policy;
    corba_policy.type        = RTT::corba::CConnectionModel(policy.type);
    corba_policy.init        = policy.init;
    corba_policy.lock_policy = RTT::corba::CLockPolicy(policy.lock_policy);
    corba_policy.pull        = policy.pull;
    corba_policy.size        = policy.size;
    return corba_policy;
}

template<typename BaseClass>
RemotePort<BaseClass>::RemotePort(RTT::types::TypeInfo const* type_info,
        CDataFlowInterface_ptr dataflow,
        std::string const& name,
        PortableServer::POA_ptr poa)
    : BaseClass(name)
    , type_info(type_info)
    , dataflow(CDataFlowInterface::_duplicate(dataflow))
    , mpoa(PortableServer::POA::_duplicate(poa)) { }

template<typename BaseClass>
CDataFlowInterface_ptr RemotePort<BaseClass>::getDataFlowInterface() const
{ return CDataFlowInterface::_duplicate(dataflow); }
template<typename BaseClass>
RTT::types::TypeInfo const* RemotePort<BaseClass>::getTypeInfo() const { return type_info; }
template<typename BaseClass>
int RemotePort<BaseClass>::serverProtocol() const { return ORO_CORBA_PROTOCOL_ID; }
template<typename BaseClass>
bool RemotePort<BaseClass>::connected() const
{ return dataflow->isConnected(this->getName().c_str()); }
template<typename BaseClass>
void RemotePort<BaseClass>::disconnect()
{ return dataflow->disconnect(this->getName().c_str()); }
template<typename BaseClass>
PortableServer::POA_ptr RemotePort<BaseClass>::_default_POA()
{ return PortableServer::POA::_duplicate(mpoa); }

template<typename BaseClass>
RTT::base::PortID* RemotePort<BaseClass>::getPortID() const
{ return new RemotePortID(dataflow, this->getName()); }
template<typename BaseClass>
bool RemotePort<BaseClass>::isSameID(RTT::base::PortID const& id) const
{
    RemotePortID const* real_id = dynamic_cast<RemotePortID const*>(&id);
    if (!real_id) return false;
    return real_id->dataflow->_is_equivalent(dataflow) && real_id->name == this->getName();
}


RemoteInputPort::RemoteInputPort(RTT::types::TypeInfo const* type_info,
        CDataFlowInterface_ptr dataflow, std::string const& reader_port,
        PortableServer::POA_ptr poa)
    : RemotePort< RTT::base::InputPortInterface >(type_info, dataflow, reader_port, poa)
{}

RTT::base::DataSourceBase* RemoteInputPort::getDataSource()
{ throw std::runtime_error("InputPort::getDataSource() is not supported in CORBA port proxies"); }

RTT::base::ChannelElementBase* RemoteInputPort::buildOutputHalf(RTT::types::TypeInfo const* type,
                                                          RTT::base::InputPortInterface& reader_,
                                                          RTT::internal::ConnPolicy const& policy)
{
    CChannelElement_var remote;
    try {
        remote = dataflow->buildOutputHalf(CORBA::string_dup(getName().c_str()), toCORBA(policy));
    }
    catch(CORBA::Exception&)
    {
        log(Error) << "caught CORBA exception while creating port's input half" << endlog();
        return NULL;
    }

    CChannelElement_i*  local;
    PortableServer::ServantBase_var servant = local =
        static_cast<CorbaTypeTransporter*>(type->getProtocol(ORO_CORBA_PROTOCOL_ID))
                            ->createChannelElement_i(mpoa);

    local->setRemoteSide(remote);
    remote->setRemoteSide(local->_this());

    // The ChannelElementBase object that represents reader_half on this side
    return dynamic_cast<RTT::base::ChannelElementBase*>(local);
}

RTT::internal::ConnFactory* RemoteInputPort::getConnFactory() { return this; }

RTT::base::PortInterface* RemoteInputPort::clone() const
{ return type_info->inputPort(getName()); }

RTT::base::PortInterface* RemoteInputPort::antiClone() const
{ return type_info->outputPort(getName()); }




RemoteOutputPort::RemoteOutputPort(RTT::types::TypeInfo const* type_info,
        CDataFlowInterface_ptr dataflow, std::string const& reader_port,
        PortableServer::POA_ptr poa)
    : RemotePort< RTT::base::OutputPortInterface >(type_info, dataflow, reader_port, poa)
{}

bool RemoteOutputPort::keepsLastWrittenValue() const
{ return false; }

void RemoteOutputPort::keepLastWrittenValue(bool new_flag)
{ throw std::runtime_error("OutputPort::keepLastWrittenValue() is not supported in CORBA port proxies"); }

bool RemoteOutputPort::createConnection( base::InputPortInterface& sink, RTT::internal::ConnPolicy const& policy )
{

    return dataflow->createConnection( this->getName().c_str(), CDataFlowInterface_i::getRemoteInterface( sink.getInterface(), mpoa.in() ), sink.getName().c_str(), toCORBA(policy) );

    //throw std::runtime_error("OutputPort::createConnection() is not supported in CORBA port proxies");
}

RTT::base::PortInterface* RemoteOutputPort::clone() const
{ return type_info->outputPort(getName()); }

RTT::base::PortInterface* RemoteOutputPort::antiClone() const
{ return type_info->inputPort(getName()); }

