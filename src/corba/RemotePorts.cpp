#include "RemotePorts.hpp"
#include "CorbaTypeTransporter.hpp"

using namespace RTT::Corba;

static RTT::Corba::ConnPolicy toCORBA(RTT::ConnPolicy const& policy)
{
    RTT::Corba::ConnPolicy corba_policy;
    corba_policy.type        = RTT::Corba::ConnectionModel(policy.type);
    corba_policy.init        = policy.init;
    corba_policy.lock_policy = RTT::Corba::LockPolicy(policy.lock_policy);
    corba_policy.pull        = policy.pull;
    corba_policy.size        = policy.size;
    return corba_policy;
}

template<typename BaseClass>
RemotePort<BaseClass>::RemotePort(RTT::TypeInfo const* type_info,
        DataFlowInterface_ptr dataflow,
        std::string const& name,
        PortableServer::POA_ptr poa)
    : BaseClass(name)
    , type_info(type_info)
    , dataflow(DataFlowInterface::_duplicate(dataflow))
    , mpoa(PortableServer::POA::_duplicate(poa)) { }

template<typename BaseClass>
DataFlowInterface_ptr RemotePort<BaseClass>::getDataFlowInterface() const
{ return DataFlowInterface::_duplicate(dataflow); }
template<typename BaseClass>
RTT::TypeInfo const* RemotePort<BaseClass>::getTypeInfo() const { return type_info; }
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
RemotePort<BaseClass>::PortID::PortID(DataFlowInterface_ptr dataflow, std::string const& name)
    : dataflow(DataFlowInterface::_duplicate(dataflow))
    , name(name) {}

template<typename BaseClass>
RTT::PortID* RemotePort<BaseClass>::getPortID() const
{ return new PortID(dataflow, this->getName()); }
template<typename BaseClass>
bool RemotePort<BaseClass>::isSameID(RTT::PortID const& id) const
{
    PortID const* real_id = dynamic_cast<PortID const*>(&id);
    if (!real_id) return false;
    return real_id->dataflow == dataflow && real_id->name == this->getName();
}

RemoteReadPort::RemoteReadPort(RTT::TypeInfo const* type_info,
        DataFlowInterface_ptr dataflow, std::string const& reader_port,
        PortableServer::POA_ptr poa)
    : RemotePort< RTT::ReadPortInterface >(type_info, dataflow, reader_port, poa)
{}

RTT::DataSourceBase* RemoteReadPort::getDataSource()
{ throw std::runtime_error("ReadPort::getDataSource() is not supported in CORBA port proxies"); }

RTT::ConnElementBase* RemoteReadPort::buildReaderHalf(RTT::TypeInfo const* type,
        RTT::ReadPortInterface& reader_,
        RTT::ConnPolicy const& policy)
{
    ConnElement_var remote =
        dataflow->buildReaderHalf(CORBA::string_dup(getName().c_str()), toCORBA(policy));

    ConnElement_i*  local;
    PortableServer::ServantBase_var servant = local =
        static_cast<CorbaTypeTransporter*>(type->getProtocol(ORO_CORBA_PROTOCOL_ID))
                            ->createConnElement_i(mpoa);

    local->setRemoteSide(remote);
    remote->setRemoteSide(local->_this());

    // The ConnElementBase object that represents reader_half on this side
    return dynamic_cast<RTT::ConnElementBase*>(local);
}

RTT::ConnFactory* RemoteReadPort::getConnFactory() { return this; }
RTT::PortInterface* RemoteReadPort::clone() const
{ return type_info->readPort(getName()); }
RTT::PortInterface* RemoteReadPort::antiClone() const
{ return type_info->writePort(getName()); }

RemoteWritePort::RemoteWritePort(RTT::TypeInfo const* type_info,
        DataFlowInterface_ptr dataflow, std::string const& reader_port,
        PortableServer::POA_ptr poa)
    : RemotePort< RTT::WritePortInterface >(type_info, dataflow, reader_port, poa)
{}

bool RemoteWritePort::keepsLastWrittenValue() const
{ return false; }
void RemoteWritePort::keepLastWrittenValue(bool new_flag)
{ throw std::runtime_error("WritePort::keepLastWrittenValue() is not supported in CORBA port proxies"); }
bool RemoteWritePort::createConnection( ReadPortInterface& sink, RTT::ConnPolicy const& policy )
{ throw std::runtime_error("WritePort::createConnection() is not supported in CORBA port proxies"); }
RTT::PortInterface* RemoteWritePort::clone() const
{ return type_info->writePort(getName()); }
RTT::PortInterface* RemoteWritePort::antiClone() const
{ return type_info->readPort(getName()); }

