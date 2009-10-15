#include "RemotePorts.hpp"
#include "CorbaTypeTransporter.hpp"
#include "DataFlowI.h"
#include "../../interface/DataFlowInterface.hpp"
#include <cassert>
#include "CorbaConnPolicy.hpp"
#include "RemoteConnID.hpp"
#include "../../internal/ConnID.hpp"
#include "../../rtt-detail-fwd.hpp"


using namespace std;
using namespace RTT::detail;

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
{ return dataflow->disconnectPort(this->getName().c_str()); }
template<typename BaseClass>
PortableServer::POA_ptr RemotePort<BaseClass>::_default_POA()
{ return PortableServer::POA::_duplicate(mpoa); }

template<typename BaseClass>
RTT::internal::ConnID* RemotePort<BaseClass>::getPortID() const
{ return new RemoteConnID(dataflow, this->getName()); }

template<typename BaseClass>
bool RemotePort<BaseClass>::createStream( const ConnPolicy& policy )
{
    log(Error) << "Can't create a data stream on a remote port !" <<endlog();
    return false;
}

template<typename BaseClass>
bool RemotePort<BaseClass>::addConnection(internal::ConnID* port_id, ChannelElementBase::shared_ptr channel_input, ConnPolicy const& policy)
{
    assert(false && "Can/Should not add connection to remote port object !");
    return false;
}


RemoteInputPort::RemoteInputPort(RTT::types::TypeInfo const* type_info,
        CDataFlowInterface_ptr dataflow, std::string const& reader_port,
        PortableServer::POA_ptr poa)
    : RemotePort< RTT::base::InputPortInterface >(type_info, dataflow, reader_port, poa)
{}

RTT::base::DataSourceBase* RemoteInputPort::getDataSource()
{ throw std::runtime_error("InputPort::getDataSource() is not supported in CORBA port proxies"); }

RTT::base::ChannelElementBase* RemoteInputPort::buildRemoteChannelOutput(RTT::types::TypeInfo const* type,
                                                          RTT::base::InputPortInterface& reader_,
                                                          RTT::ConnPolicy const& policy)
{
    // This is called by the createConnection()->createRemoteConnection() code of the ConnFactory.
    Logger::In in("RemoteInputPort::buildRemoteChannelOutput");

    // First we delegate this call to the remote side, which will create a corba channel element,
    // buffers and channel output and attach this to the real input port.
    CRemoteChannelElement_var remote;
    RTT::base::ChannelElementBase* buf = 0;
    try {
        CConnPolicy cpolicy = toCORBA(policy);
        CChannelElement_var ret = dataflow->buildChannelOutput(CORBA::string_dup(getName().c_str()), cpolicy);
        if ( CORBA::is_nil(ret) ) {
            return 0;
        }
        remote = CRemoteChannelElement::_narrow( ret.in() );
        policy.name_id = toRTT(cpolicy).name_id;
    }
    catch(CORBA::Exception& e)
    {
        log(Error) << "Caught CORBA exception while creating port's input half:" << endlog();
        log(Error) << CORBA_EXCEPTION_INFO( e ) <<endlog();
        return NULL;
    }

    // Input side is now ok and waiting for us to complete. We build our corba channel element too
    // and connect it to the remote side and vice versa.
    CRemoteChannelElement_i*  local;
    PortableServer::ServantBase_var servant = local =
        static_cast<CorbaTypeTransporter*>(type->getProtocol(ORO_CORBA_PROTOCOL_ID))
                            ->createChannelElement_i(mpoa, policy.pull);

    local->setRemoteSide(remote);
    remote->setRemoteSide(local->_this());

    RTT::base::ChannelElementBase* corba_ceb = dynamic_cast<RTT::base::ChannelElementBase*>(local);

    // Note: this probably needs to factored out, see also DataFlowI.cpp:buildChannelOutput() for the counterpart of this code.
    // If the user specified OOB, we prepend the prefered transport.
    // This inserts a channel element before our corba channel element.
    // The remote input side will have done this too in the above step.
    if ( policy.transport != 0 && policy.transport != ORO_CORBA_PROTOCOL_ID ) {
        // create alternative path / out of band transport.
        string name =  policy.name_id ;
        if ( type->getProtocol(policy.transport) == 0 ) {
            log(Error) << "Could not create out-of-band transport for port "<< name << " with transport id " << policy.transport <<endlog();
            log(Error) << "No such transport registered. Check your policy.transport settings or add the transport for type "<< type->getTypeName() <<endlog();
        }
        RTT::base::ChannelElementBase* ceb = type->getProtocol(policy.transport)->createStream(this, policy, true);
        if (ceb) {
            // insertion before corba.
            ceb->setOutput( corba_ceb );
            corba_ceb = ceb;
            log(Info) <<"Redirecting data for port "<<name << " to out-of-band protocol "<< policy.transport << endlog();
        } else {
            log(Error) << "The type transporter for type "<<type->getTypeName()<< " failed to create a dual channel for port " << name<<endlog();
        }
    } else {
        // if no oob present, create a buffer at output port to guarantee RT delivery of data. (is always present in push&pull).
        buf = type->buildDataStorage(policy);
        assert(buf);
        buf->setOutput( corba_ceb );
        corba_ceb = buf;
    }
    // store the object reference in a map, for future lookup in channelReady().
    // this is coupled with the use of channelReady(). We assume the caller will always pass
    // chan->getOutputEndPoint() in that function.
    channel_map[ corba_ceb->getOutputEndPoint().get() ] = CChannelElement::_duplicate( remote );
    // The ChannelElementBase object that represents reader_half on this side
    return corba_ceb;
}

RTT::internal::ConnFactory* RemoteInputPort::getConnFactory() { return this; }

RTT::base::PortInterface* RemoteInputPort::clone() const
{ return type_info->inputPort(getName()); }

RTT::base::PortInterface* RemoteInputPort::antiClone() const
{ return type_info->outputPort(getName()); }


bool RemoteInputPort::channelReady(base::ChannelElementBase::shared_ptr channel) {
    if (! channel_map.count( channel.get() ) ) {
        log(Error) <<"No such channel found in "<< getName() <<".channelReady( channel ): aborting connection."<<endlog();
        return false;
    }
    try {
        CChannelElement_ptr cce = channel_map[ channel.get() ];
        assert( cce );
        return dataflow->channelReady( this->getName().c_str(),  cce );
    }
    catch(CORBA::Exception& e)
    {
        log(Error) <<"Remote call to "<< getName() <<".channelReady( channel ) failed with a CORBA exception: aborting connection."<<endlog();
        log(Error) << CORBA_EXCEPTION_INFO( e ) <<endlog();
        return false;
    }
}

RemoteOutputPort::RemoteOutputPort(RTT::types::TypeInfo const* type_info,
        CDataFlowInterface_ptr dataflow, std::string const& reader_port,
        PortableServer::POA_ptr poa)
    : RemotePort< RTT::base::OutputPortInterface >(type_info, dataflow, reader_port, poa)
{}

bool RemoteOutputPort::keepsLastWrittenValue() const
{ return false; }

void RemoteOutputPort::keepLastWrittenValue(bool new_flag)
{ throw std::runtime_error("OutputPort::keepLastWrittenValue() is not supported in CORBA port proxies"); }

bool RemoteOutputPort::createConnection( base::InputPortInterface& sink, RTT::ConnPolicy const& policy )
{
    try {
        CConnPolicy cpolicy = toCORBA(policy);
        // this dynamic CDataFlowInterface lookup is tricky, we re/ab-use the DataFlowInterface pointer of sink !
        if ( dataflow->createConnection( this->getName().c_str(), CDataFlowInterface_i::getRemoteInterface( sink.getInterface(), mpoa.in() ), sink.getName().c_str(), cpolicy ) ) {
            policy.name_id = cpolicy.name_id;
            return true;
        }
    }
    catch(CORBA::Exception& e)
    {
        log(Error) <<"Remote call to "<< getName() <<".createConnection() failed with a CORBA exception: aborting connection."<<endlog();
        log(Error) << CORBA_EXCEPTION_INFO( e ) <<endlog();
        return false;
    }
}

RTT::base::PortInterface* RemoteOutputPort::clone() const
{ return type_info->outputPort(getName()); }

RTT::base::PortInterface* RemoteOutputPort::antiClone() const
{ return type_info->inputPort(getName()); }

