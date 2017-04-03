/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  RemotePorts.cpp

                        RemotePorts.cpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "RemotePorts.hpp"
#include "CorbaTypeTransporter.hpp"
#include "DataFlowI.h"
#include "../../DataFlowInterface.hpp"
#include <cassert>
#include "CorbaConnPolicy.hpp"
#include "CorbaLib.hpp"
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
{
    return dataflow->isConnected(this->getName().c_str());
}
template<typename BaseClass>
void RemotePort<BaseClass>::disconnect()
{
    dataflow->disconnectPort(this->getName().c_str());
}


template<typename BaseClass>
PortableServer::POA_ptr RemotePort<BaseClass>::_default_POA()
{ return PortableServer::POA::_duplicate(mpoa); }

template<typename BaseClass>
RTT::internal::ConnID* RemotePort<BaseClass>::getPortID() const
{ return new RemoteConnID(dataflow, this->getName()); }

template<typename BaseClass>
bool RemotePort<BaseClass>::createStream( const RTT::ConnPolicy& policy )
{
    log(Error) << "Can't create a data stream on a remote port !" <<endlog();
    return false;
}

template<typename BaseClass>
bool RemotePort<BaseClass>::addConnection(RTT::internal::ConnID* port_id, ChannelElementBase::shared_ptr channel_input, RTT::ConnPolicy const& policy)
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

RTT::base::ChannelElementBase::shared_ptr RemoteInputPort::buildRemoteChannelOutput(
        RTT::base::OutputPortInterface& output_port,
        RTT::types::TypeInfo const* type,
        RTT::base::InputPortInterface& reader_,
        RTT::ConnPolicy const& policy)
{
    // This is called by the createConnection()->createRemoteConnection() code of the ConnFactory.
    Logger::In in("RemoteInputPort::buildRemoteChannelOutput");

    // First we delegate this call to the remote side, which will create a corba channel element,
    // buffers and channel output and attach this to the real input port.
    CRemoteChannelElement_var remote;
    RTT::base::ChannelElementBase::shared_ptr buf;
    try {
        CConnPolicy cpolicy = toCORBA(policy);
        CChannelElement_var ret = dataflow->buildChannelOutput(getName().c_str(), cpolicy);
        if ( CORBA::is_nil(ret) ) {
            return 0;
        }
        remote = CRemoteChannelElement::_narrow( ret.in() );
        policy.name_id = toRTT(cpolicy).name_id;
    }
    catch(CORBA::Exception& e)
    {
        log(Error) << "Caught CORBA exception while creating a remote channel output:" << endlog();
        log(Error) << CORBA_EXCEPTION_INFO( e ) <<endlog();
        return NULL;
    }

    // Input side is now ok and waiting for us to complete. We build our corba channel element too
    // and connect it to the remote side and vice versa.

    std::string dispatcherName = CDataFlowInterface_i::dispatcherNameFromPolicy(output_port.getInterface(), policy);
    CRemoteChannelElement_i*  local =
        static_cast<CorbaTypeTransporter*>(type->getProtocol(ORO_CORBA_PROTOCOL_ID))
                            ->createOutputChannelElement_i(dispatcherName, mpoa, policy.pull);

    CRemoteChannelElement_var proxy = local->_this();
    local->setRemoteSide(remote);
    remote->setRemoteSide(proxy.in());
    local->_remove_ref();

    RTT::base::ChannelElementBase::shared_ptr corba_ceb = dynamic_cast<RTT::base::ChannelElementBase*>(local);

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
        RTT::base::ChannelElementBase::shared_ptr ceb = type->getProtocol(policy.transport)->createStream(this, policy, true);
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

bool RemoteInputPort::disconnect(PortInterface* port)
{
    //just calling the implementation from the other side. (implemented already for RemoteOutputPort).
    return port->disconnect(this);
}

RTT::base::PortInterface* RemoteInputPort::clone() const
{ return type_info->inputPort(getName()); }

RTT::base::PortInterface* RemoteInputPort::antiClone() const
{ return type_info->outputPort(getName()); }


bool RemoteInputPort::channelReady(RTT::base::ChannelElementBase::shared_ptr channel, RTT::ConnPolicy const& policy) {
    if (! channel_map.count( channel.get() ) ) {
        log(Error) <<"No such channel found in "<< getName() <<".channelReady( channel ): aborting connection."<<endlog();
        return false;
    }
    try {
        CChannelElement_ptr cce = channel_map[ channel.get() ];
        assert( cce );
        CConnPolicy cpolicy = toCORBA(policy);
        return dataflow->channelReady( this->getName().c_str(),  cce, cpolicy );
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

DataSourceBase::shared_ptr RemoteOutputPort::getDataSource() const
{
    return DataSourceBase::shared_ptr();
}


bool RemoteOutputPort::disconnect(PortInterface* port)
{
    RemoteInputPort *portI = dynamic_cast<RemoteInputPort *>(port);

    //if not a remote port, we can not handle at the moment!
    if(portI == NULL){
        Logger::In in("RemoteOutputPort::disconnect(PortInterface& port)");
        log(Error) << "Port: " << port->getName() << " could not be disconnected from: " << this->getName()
                   << " because it could not be casted to a RemoteInputPort type!" << nlog()
                   << "Only disconnect of two remote ports supported by corba layer, yet!" << endlog();
        return false;

    }

    //if Remote Input Port:
    return dataflow->removeConnection(this->getName().c_str(), portI->getDataFlowInterface(),
                                      portI->getName().c_str());
}

bool RemoteOutputPort::createConnection( RTT::base::InputPortInterface& sink, RTT::ConnPolicy const& policy )
{
    try {
        CConnPolicy cpolicy = toCORBA(policy);
        // first check if we're connecting to another remote:
        RemoteInputPort* rip = dynamic_cast<RemoteInputPort*>(&sink);
        if ( rip ){
            CDataFlowInterface_var cdfi = rip->getDataFlowInterface();
            if ( dataflow->createConnection( this->getName().c_str(), cdfi.in() , sink.getName().c_str(), cpolicy ) ) {
                policy.name_id = cpolicy.name_id;
                return true;
            } else
                return false;
        }
        // !!! only if sink is local:
        // this dynamic CDataFlowInterface lookup is tricky, we re/ab-use the DataFlowInterface pointer of sink !
        if(sink.getInterface() == 0){
            log(Error)<<"RemotePort connection is only possible if the local port '"<<sink.getName()<<"' is added to a DataFlowInterface. Use addPort for this."<<endlog();
            return false;
        }
        CDataFlowInterface_ptr cdfi = CDataFlowInterface_i::getRemoteInterface( sink.getInterface(), mpoa.in() );
        if ( dataflow->createConnection( this->getName().c_str(), cdfi , sink.getName().c_str(), cpolicy ) ) {
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
	return false;
}

RTT::base::PortInterface* RemoteOutputPort::clone() const
{ return type_info->outputPort(getName()); }

RTT::base::PortInterface* RemoteOutputPort::antiClone() const
{ return type_info->inputPort(getName()); }

