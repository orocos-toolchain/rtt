/***************************************************************************
  tag: FMTC  Tue Mar 11 21:49:24 CET 2008  DataFlowI.cpp

                        DataFlowI.cpp -  description
                           -------------------
    begin                : Tue March 11 2008
    copyright            : (C) 2008 FMTC
    email                : peter.soetens@fmtc.be

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

#include "DataFlowI.h"
#include "corba.h"
#ifdef CORBA_IS_TAO
#include "DataFlowS.h"
#else
#include "DataFlowC.h"
#endif
#include "../../base/PortInterface.hpp"
#include "../../Logger.hpp"
#include "ControlTaskProxy.hpp"
#include "CorbaTypeTransporter.hpp"
#include "../../InputPort.hpp"
#include "../../OutputPort.hpp"
#include "CorbaConnPolicy.hpp"

#include "RemotePorts.hpp"
#include "RemoteConnID.hpp"

#include <iostream>

using namespace std;
using namespace RTT::corba;
using namespace RTT::base;
using namespace RTT::types;
using namespace RTT::internal;

CDataFlowInterface_i::ServantMap CDataFlowInterface_i::s_servant_map;

CDataFlowInterface_i::CDataFlowInterface_i (RTT::interface::DataFlowInterface* interface, PortableServer::POA_ptr poa)
    : mdf(interface), mpoa(PortableServer::POA::_duplicate(poa))
{
}

CDataFlowInterface_i::~CDataFlowInterface_i ()
{
}

void CDataFlowInterface_i::registerServant(CDataFlowInterface_ptr objref, RTT::interface::DataFlowInterface* obj)
{
    s_servant_map.push_back(
            std::make_pair(
                CDataFlowInterface_var(RTT::corba::CDataFlowInterface::_duplicate(objref)),
                obj)
            );
}
void CDataFlowInterface_i::deregisterServant(RTT::interface::DataFlowInterface* obj)
{
    for (ServantMap::iterator it = s_servant_map.begin();
            it != s_servant_map.end(); ++it)
    {
        if (it->second == obj)
        {
            s_servant_map.erase(it);
            return;
        }
    }
}

RTT::interface::DataFlowInterface* CDataFlowInterface_i::getLocalInterface(CDataFlowInterface_ptr objref)
{
    for (ServantMap::const_iterator it = s_servant_map.begin();
            it != s_servant_map.end(); ++it)
    {
        if (it->first->_is_equivalent(objref))
            return it->second;
    }
    return NULL;
}

CDataFlowInterface_ptr CDataFlowInterface_i::getRemoteInterface(RTT::interface::DataFlowInterface* dfi, PortableServer::POA_ptr poa)
{
    for (ServantMap::const_iterator it = s_servant_map.begin();
            it != s_servant_map.end(); ++it)
    {
        if (it->second == dfi)
            return it->first;
    }
    CDataFlowInterface_i* servant = new CDataFlowInterface_i(dfi, poa );
    CDataFlowInterface_ptr server = servant->_this();
    registerServant( server, dfi);
    return server;
}

PortableServer::POA_ptr CDataFlowInterface_i::_default_POA()
{
    return PortableServer::POA::_duplicate(mpoa);
}

CDataFlowInterface::CPortNames * CDataFlowInterface_i::getPorts() ACE_THROW_SPEC ((
	      CORBA::SystemException
	    ))
{
    ::RTT::interface::DataFlowInterface::PortNames ports = mdf->getPortNames();

    RTT::corba::CDataFlowInterface::CPortNames_var pn = new RTT::corba::CDataFlowInterface::CPortNames();
    pn->length( ports.size() );

    for (unsigned int i=0; i != ports.size(); ++i )
        pn[i] = CORBA::string_dup( ports[i].c_str() );

    return pn._retn();
}

CDataFlowInterface::CPortDescriptions* CDataFlowInterface_i::getPortDescriptions() ACE_THROW_SPEC ((
	      CORBA::SystemException
	    ))
{
    RTT::interface::DataFlowInterface::PortNames ports = mdf->getPortNames();
    RTT::corba::CDataFlowInterface::CPortDescriptions_var result = new RTT::corba::CDataFlowInterface::CPortDescriptions();
    result->length( ports.size() );

    unsigned int j = 0;
    for (unsigned int i = 0; i < ports.size(); ++i)
    {
        CPortDescription port_desc;

        PortInterface* port = mdf->getPort(ports[i]);
        port_desc.name = CORBA::string_dup(ports[i].c_str());

        TypeInfo const* type_info = port->getTypeInfo();
        if (!type_info || !type_info->getProtocol(ORO_CORBA_PROTOCOL_ID))
        {
            log(Warning) << "the type of port " << ports[i] << " is not registered into the Orocos type system. It is ignored by the CORBA layer." << endlog();
            continue;
        }

        port_desc.type_name = CORBA::string_dup(type_info->getTypeName().c_str());
        if (dynamic_cast<InputPortInterface*>(port))
            port_desc.type = corba::CInput;
        else
            port_desc.type = corba::COutput;

        result[j++] = port_desc;
    }
    result->length( j );
    return result._retn();
}

CPortType CDataFlowInterface_i::getPortType(const char * port_name) ACE_THROW_SPEC ((
	      CORBA::SystemException
	      ,::RTT::corba::CNoSuchPortException
	    ))
{
    PortInterface* p = mdf->getPort(port_name);
    if (p == 0)
        throw CNoSuchPortException();

    if (dynamic_cast<InputPortInterface*>(p))
        return RTT::corba::CInput;
    else return RTT::corba::COutput;
}

char* CDataFlowInterface_i::getDataType(const char * port_name) ACE_THROW_SPEC ((
	      CORBA::SystemException
	      ,::RTT::corba::CNoSuchPortException
	    ))
{
    PortInterface* p = mdf->getPort(port_name);
    if ( p == 0)
        throw CNoSuchPortException();
    return CORBA::string_dup( p->getTypeInfo()->getTypeName().c_str() );
}

CORBA::Boolean CDataFlowInterface_i::isConnected(const char * port_name) ACE_THROW_SPEC ((
	      CORBA::SystemException
	      ,::RTT::corba::CNoSuchPortException
	    ))
{
    PortInterface* p = mdf->getPort(port_name);
    if (p == 0)
        throw corba::CNoSuchPortException();

    return p->connected();
}

CORBA::Boolean CDataFlowInterface_i::channelReady(const char * reader_port_name, CChannelElement_ptr channel) ACE_THROW_SPEC ((
	      CORBA::SystemException
	      ,::RTT::corba::CNoSuchPortException
	    ))
{
    PortInterface* p = mdf->getPort(reader_port_name);
    if (p == 0)
        throw corba::CNoSuchPortException();

    InputPortInterface* ip = dynamic_cast<InputPortInterface*>(p);
    if (ip == 0)
        throw corba::CNoSuchPortException();

    // lookup the C++ channel that matches the corba channel and
    // inform our local port that that C++ channel is ready.
    ChannelList::iterator it=channel_list.begin();
    for (; it != channel_list.end(); ++it) {
        if (it->first->_is_equivalent (channel) ) {
            return ip->channelReady( it->second );
        }
    }
    log(Error) << "Invalid CORBA channel given for port " << reader_port_name << ": could not match it to a local C++ channel." <<endlog();
    return false;
}

void CDataFlowInterface_i::disconnectPort(const char * port_name) ACE_THROW_SPEC ((
	      CORBA::SystemException
	      ,::RTT::corba::CNoSuchPortException
	    ))
{
    PortInterface* p = mdf->getPort(port_name);
    if (p == 0) {
        log(Error) << "disconnectPort: No such port: "<< port_name <<endlog();
        throw corba::CNoSuchPortException();
    }
    p->disconnect();
}

void CDataFlowInterface_i::removeConnection(
        const char* writer_port,
        CDataFlowInterface_ptr reader_interface, const char* reader_port) ACE_THROW_SPEC ((
        	      CORBA::SystemException
        	      ,::RTT::corba::CNoSuchPortException
        	    ))
{
    OutputPortInterface* writer = 
        dynamic_cast<OutputPortInterface*>(mdf->getPort(writer_port));
    if (writer == 0) {
        log(Error) << "disconnectPort: No such writer: "<< writer_port <<endlog();
        throw corba::CNoSuchPortException();
    }
    RemoteConnID rcid(reader_interface, reader_port);
    writer->removeConnection( &rcid );
}

::CORBA::Boolean CDataFlowInterface_i::createStream( const char* port,
                               RTT::corba::CConnPolicy & policy) ACE_THROW_SPEC ((
                               	      CORBA::SystemException
                               	      ,::RTT::corba::CNoSuchPortException
                               	    ))
{
    PortInterface* p = mdf->getPort(port);
    if (p == 0) {
        log(Error) << "createStream: No such port: "<< p->getName() <<endlog();
        throw corba::CNoSuchPortException();
    }

    RTT::ConnPolicy p2 = toRTT(policy);
    if ( p->createStream( p2 ) ) {
        policy = toCORBA(p2);
        return true;
    }
    return false;
}

void CDataFlowInterface_i::removeStream( const char* port, const char* stream_name) ACE_THROW_SPEC ((
	      CORBA::SystemException
	      ,::RTT::corba::CNoSuchPortException
	    ))
{
    PortInterface* p = mdf->getPort(port);
    if (p == 0) {
        log(Error) << "createStream: No such port: "<< p->getName() <<endlog();
        throw corba::CNoSuchPortException();
    }
    StreamConnID rcid(stream_name);
    p->removeConnection( &rcid );
}


CChannelElement_ptr CDataFlowInterface_i::buildChannelOutput(
        const char* port_name, CConnPolicy & corba_policy) ACE_THROW_SPEC ((
         	      CORBA::SystemException
         	      ,::RTT::corba::CNoCorbaTransport
                  ,::RTT::corba::CNoSuchPortException
         	    ))
{
    Logger::In in("CDataFlowInterface_i::buildChannelOutput");
    InputPortInterface* port = dynamic_cast<InputPortInterface*>(mdf->getPort(port_name));
    if (port == 0)
        throw CNoSuchPortException();

    TypeInfo const* type_info = port->getTypeInfo();
    if (!type_info)
        throw CNoCorbaTransport();

    CorbaTypeTransporter* transporter =
        dynamic_cast<CorbaTypeTransporter*>(type_info->getProtocol(ORO_CORBA_PROTOCOL_ID));
    if (!transporter)
        throw CNoCorbaTransport();

    // Convert to RTT policy.
    ConnPolicy policy2 = toRTT(corba_policy);

    ChannelElementBase::shared_ptr end = type_info->buildChannelOutput(*port);
    CRemoteChannelElement_i* this_element;
    PortableServer::ServantBase_var servant = this_element = transporter->createChannelElement_i(mdf, mpoa, corba_policy.pull);

    /*
     * This part is for out-of band (needs to be factored out).
     */
    if ( corba_policy.transport !=0 && corba_policy.transport != ORO_CORBA_PROTOCOL_ID) {
        // prepare out-of-band transport for this port.
        // if user supplied name, use that one.
        if ( type_info->getProtocol(corba_policy.transport) == 0 ) {
            log(Error) << "Could not create out-of-band transport for port "<< port_name << " with transport id " << corba_policy.transport <<endlog();
            log(Error) << "No such transport registered. Check your corba_policy.transport settings or add the transport for type "<< type_info->getTypeName() <<endlog();
        }
        RTT::base::ChannelElementBase* ceb = type_info->getProtocol(corba_policy.transport)->createStream(port, policy2, false);
        // if no user supplied name, pass on the new name.
        if ( strlen( corba_policy.name_id.in()) == 0 )
            corba_policy.name_id = CORBA::string_dup( policy2.name_id.c_str() );

        if (ceb) {
            // override, insert oob element between corba and endpoint and add a buffer between oob and endpoint.
            dynamic_cast<ChannelElementBase*>(this_element)->setOutput(ceb);
            ChannelElementBase::shared_ptr buf = type_info->buildDataStorage(toRTT(corba_policy));
            ceb->setOutput( buf );
            buf->setOutput(end);
            log(Info) <<"Receiving data for port "<< policy2.name_id << " from out-of-band protocol "<< corba_policy.transport <<endlog();
        } else {
            log(Error) << "The type transporter for type "<<type_info->getTypeName()<< " failed to create an out-of-band endpoint for port " << port_name<<endlog();
            return RTT::corba::CChannelElement::_nil();
        }
        //
    } else {
        // No OOB. omit buffer if in pull
        if ( !corba_policy.pull ) {
            ChannelElementBase::shared_ptr buf = type_info->buildDataStorage(toRTT(corba_policy));
            dynamic_cast<ChannelElementBase*>(this_element)->setOutput(buf);
            buf->setOutput(end);
        } else {
            dynamic_cast<ChannelElementBase*>(this_element)->setOutput(end);
        }
    }

    // store our mapping of corba channel elements to C++ channel elements. We need this for channelReady() and removing a channel again.
    channel_list.push_back( ChannelList::value_type(RTT::corba::CChannelElement::_duplicate(this_element->_this()), end->getOutputEndPoint()));

    return RTT::corba::CChannelElement::_duplicate(this_element->_this());
}

/**
 * This code is a major copy-past of the above. Amazing how much boiler plate we need.
 */
CChannelElement_ptr CDataFlowInterface_i::buildChannelInput(
        const char* port_name, CConnPolicy & corba_policy) ACE_THROW_SPEC ((
        	      CORBA::SystemException
        	      ,::RTT::corba::CNoCorbaTransport
        	      ,::RTT::corba::CNoSuchPortException
        	    ))
{
    Logger::In in("CDataFlowInterface_i::buildChannelInput");
    // First check validity of user input...
    OutputPortInterface* port = dynamic_cast<OutputPortInterface*>(mdf->getPort(port_name));
    if (port == 0)
        throw CNoSuchPortException();

    TypeInfo const* type_info = port->getTypeInfo();
    if (!type_info)
        throw CNoCorbaTransport();

    CorbaTypeTransporter* transporter =
        dynamic_cast<CorbaTypeTransporter*>(type_info->getProtocol(ORO_CORBA_PROTOCOL_ID));
    if (!transporter)
        throw CNoCorbaTransport();

    // Convert to RTT policy.
    ConnPolicy policy2 = toRTT(corba_policy);

    // Now create the output-side channel elements.
    ChannelElementBase::shared_ptr start = type_info->buildChannelInput(*port);

    // The channel element that exposes our channel in CORBA
    CRemoteChannelElement_i* this_element;
    PortableServer::ServantBase_var servant = this_element = transporter->createChannelElement_i(mdf, mpoa, corba_policy.pull);

    // Attach the corba channel element first (so OOB is after corba).
    assert( dynamic_cast<ChannelElementBase*>(this_element) );
    start->getOutputEndPoint()->setOutput( dynamic_cast<ChannelElementBase*>(this_element));

    /*
     * This part if for out-of band. (needs to be factored out).
     */
    if ( corba_policy.transport !=0 && corba_policy.transport != ORO_CORBA_PROTOCOL_ID) {
        // prepare out-of-band transport for this port.
        // if user supplied name, use that one.
        if ( type_info->getProtocol(corba_policy.transport) == 0 ) {
            log(Error) << "Could not create out-of-band transport for port "<< port_name << " with transport id " << corba_policy.transport <<endlog();
            log(Error) << "No such transport registered. Check your corba_policy.transport settings or add the transport for type "<< type_info->getTypeName() <<endlog();
            throw CNoCorbaTransport();
        }
        RTT::base::ChannelElementBase* ceb = type_info->getProtocol(corba_policy.transport)->createStream(port, policy2, true);
        // if no user supplied name, pass on the new name.
        if ( strlen( corba_policy.name_id.in()) == 0 )
            corba_policy.name_id = CORBA::string_dup( policy2.name_id.c_str() );

        if (ceb) {
            // OOB is added to end of chain.
            start->getOutputEndPoint()->setOutput( ceb );
            log(Info) <<"Sending data from port "<< policy2.name_id << " to out-of-band protocol "<< corba_policy.transport <<endlog();
        } else {
            log(Error) << "The type transporter for type "<<type_info->getTypeName()<< " failed to create an out-of-band endpoint for port " << port_name<<endlog();
            throw CNoCorbaTransport();
        }
    } else {
        // No OOB. Always add output buffer.
        ChannelElementBase::shared_ptr buf = type_info->buildDataStorage(toRTT(corba_policy));
        start->setOutput(buf);
        buf->setOutput( dynamic_cast<ChannelElementBase*>(this_element) );
    }


    // Attach to our output port:
    port->addConnection( new SimpleConnID(), start->getInputEndPoint(), policy2);

    // Finally, store our mapping of corba channel elements to C++ channel elements. We need this for channelReady() and removing a channel again.
    channel_list.push_back( ChannelList::value_type(RTT::corba::CChannelElement::_duplicate(this_element->_this()), start->getInputEndPoint()));

    return RTT::corba::CChannelElement::_duplicate(this_element->_this());
}


::CORBA::Boolean CDataFlowInterface_i::createConnection(
        const char* writer_port, CDataFlowInterface_ptr reader_interface,
        const char* reader_port, CConnPolicy & policy) ACE_THROW_SPEC ((
        	      CORBA::SystemException
        	      ,::RTT::corba::CNoSuchPortException
        	    ))
{
    Logger::In in("CDataFlowInterface_i::createConnection");
    OutputPortInterface* writer = dynamic_cast<OutputPortInterface*>(mdf->getPort(writer_port));
    if (writer == 0)
        throw CNoSuchPortException();

    // Check if +reader_interface+ is local. If it is, use the non-CORBA
    // connection.
    RTT::interface::DataFlowInterface* local_interface = CDataFlowInterface_i::getLocalInterface(reader_interface);
    if (local_interface && policy.transport == 0)
    {
        InputPortInterface* reader =
            dynamic_cast<InputPortInterface*>(local_interface->getPort(reader_port));
        if (!reader)
        {
            log(Warning) << "CORBA: createConnection() target is not an input port" << endlog();
            throw CNoSuchPortException();
            return false;
        }

        log(Debug) << "CORBA: createConnection() is creating a LOCAL connection between " <<
           writer_port << " and " << reader_port << endlog();
        return writer->createConnection(*reader, toRTT(policy));
    }
    else
        log(Debug) << "CORBA: createConnection() is creating a REMOTE connection between " <<
           writer_port << " and " << reader_port << endlog();

    try {
        if (reader_interface->getPortType(reader_port) != corba::CInput) {
            log(Error) << "Could not create connection: " << reader_port <<" is not an input port."<<endlog();
            throw CNoSuchPortException();
            return false;
        }

        // Creates a proxy to the remote input port
        RemoteInputPort port(writer->getTypeInfo(), reader_interface, reader_port, mpoa);
        port.setInterface( mdf ); // cheating !
        // Connect to proxy.
        return writer->createConnection(port, toRTT(policy));
    }
    catch(CORBA::COMM_FAILURE&) { throw; }
    catch(CORBA::TRANSIENT&) { throw; }
    catch(...) { throw; }
    return false;
}

// standard constructor
CRemoteChannelElement_i::CRemoteChannelElement_i(RTT::corba::CorbaTypeTransporter const& transport,
	  PortableServer::POA_ptr poa)
    : transport(transport)
    , mpoa(PortableServer::POA::_duplicate(poa)) {}
CRemoteChannelElement_i::~CRemoteChannelElement_i() {}
PortableServer::POA_ptr CRemoteChannelElement_i::_default_POA()
{ return PortableServer::POA::_duplicate(mpoa); }
void CRemoteChannelElement_i::setRemoteSide(CRemoteChannelElement_ptr remote) ACE_THROW_SPEC ((
	      CORBA::SystemException
	    ))
{ this->remote_side = RTT::corba::CRemoteChannelElement::_duplicate(remote); }

