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
#include "../PortInterface.hpp"
#include "../ChannelInterface.hpp"
#include "../Logger.hpp"
#include "ControlTaskProxy.hpp"
#include "CorbaTypeTransporter.hpp"
#include "../Ports.hpp"

#include "RemotePorts.hpp"

#include <iostream>

using namespace RTT::Corba;

DataFlowInterface_i::ServantMap DataFlowInterface_i::s_servant_map;

static RTT::ConnPolicy toRTT(RTT::Corba::ConnPolicy const& corba_policy)
{
    RTT::ConnPolicy policy;
    policy.type        = corba_policy.type;
    policy.init        = corba_policy.init;
    policy.lock_policy = corba_policy.lock_policy;
    policy.pull        = corba_policy.pull;
    policy.size        = corba_policy.size;
    return policy;
}


DataFlowInterface_i::DataFlowInterface_i (RTT::DataFlowInterface* interface, PortableServer::POA_ptr poa)
    : mdf(interface), mpoa(PortableServer::POA::_duplicate(poa))
{
}

DataFlowInterface_i::~DataFlowInterface_i ()
{
}

void DataFlowInterface_i::registerServant(DataFlowInterface_ptr objref, RTT::DataFlowInterface* obj)
{
    s_servant_map.push_back(
            std::make_pair(
                DataFlowInterface_var(RTT::Corba::DataFlowInterface::_duplicate(objref)),
                obj)
            );
}
void DataFlowInterface_i::deregisterServant(RTT::DataFlowInterface* obj)
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

RTT::DataFlowInterface* DataFlowInterface_i::getLocalInterface(DataFlowInterface_ptr objref)
{
    for (ServantMap::const_iterator it = s_servant_map.begin();
            it != s_servant_map.end(); ++it)
    {
        if (it->first->_is_equivalent(objref))
            return it->second;
    }
    return NULL;
}

PortableServer::POA_ptr DataFlowInterface_i::_default_POA()
{
    return PortableServer::POA::_duplicate(mpoa);
}

DataFlowInterface::PortNames * DataFlowInterface_i::getPorts()
{
    ::RTT::DataFlowInterface::PortNames ports = mdf->getPortNames();

    RTT::Corba::DataFlowInterface::PortNames_var pn = new RTT::Corba::DataFlowInterface::PortNames();
    pn->length( ports.size() );

    for (unsigned int i=0; i != ports.size(); ++i )
        pn[i] = CORBA::string_dup( ports[i].c_str() );

    return pn._retn();
}

DataFlowInterface::PortDescriptions* DataFlowInterface_i::getPortDescriptions()
{
    RTT::DataFlowInterface::PortNames ports = mdf->getPortNames();
    RTT::Corba::DataFlowInterface::PortDescriptions_var result = new RTT::Corba::DataFlowInterface::PortDescriptions();
    result->length( ports.size() );

    unsigned int j = 0;
    for (unsigned int i = 0; i < ports.size(); ++i)
    {
        PortDescription port_desc;

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
            port_desc.type = Corba::Input;
        else
            port_desc.type = Corba::Output;

        result[j++] = port_desc;
    }
    result->length( j );
    return result._retn();
}

PortType DataFlowInterface_i::getPortType(const char * port_name)
{
    RTT::PortInterface* p = mdf->getPort(port_name);
    if (p == 0)
        throw NoSuchPortException();

    if (dynamic_cast<InputPortInterface*>(p))
        return RTT::Corba::Input;
    else return RTT::Corba::Output;
}

char* DataFlowInterface_i::getDataType(const char * port_name)
{
    RTT::PortInterface* p = mdf->getPort(port_name);
    if ( p == 0)
        throw NoSuchPortException();
    return CORBA::string_dup( p->getTypeInfo()->getTypeName().c_str() );
}

CORBA::Boolean DataFlowInterface_i::isConnected(const char * port_name)
{
    PortInterface* p = mdf->getPort(port_name);
    if (p == 0)
        throw Corba::NoSuchPortException();

    return p->connected();
}

void DataFlowInterface_i::disconnect(const char * port_name)
{
    RTT::PortInterface* p = mdf->getPort(port_name);
    if (p == 0)
        return;

    p->disconnect();
}

void DataFlowInterface_i::disconnectPort(
        const char* writer_port,
        DataFlowInterface_ptr reader_interface, const char* reader_port)
{
    RTT::OutputPortInterface* writer = 
        dynamic_cast<RTT::OutputPortInterface*>(mdf->getPort(writer_port));
    if (writer == 0)
        return;

    PortableServer::POA_var poa = _default_POA();
    RemoteInputPort reader(writer->getTypeInfo(), reader_interface, reader_port, poa);
    writer->disconnect(reader);
}

ChannelElement_ptr DataFlowInterface_i::buildReaderHalf(
        const char* reader_port, ConnPolicy const& corba_policy)
{
    RTT::InputPortInterface* reader = dynamic_cast<RTT::InputPortInterface*>(mdf->getPort(reader_port));
    if (reader == 0)
        throw NoSuchPortException();

    TypeInfo const* type_info = reader->getTypeInfo();
    if (!type_info)
        throw NoCorbaTransport();

    CorbaTypeTransporter* transporter =
        dynamic_cast<CorbaTypeTransporter*>(type_info->getProtocol(ORO_CORBA_PROTOCOL_ID));
    if (!transporter)
        throw NoCorbaTransport();

    RTT::ChannelElementBase* element = transporter->buildReaderHalf(*reader, toRTT(corba_policy));
    ChannelElement_i* this_element;
    PortableServer::ServantBase_var servant = this_element = transporter->createChannelElement_i(mpoa);
    dynamic_cast<ChannelElementBase*>(this_element)->setReader(element);

    return RTT::Corba::ChannelElement::_duplicate(this_element->_this());
}

::CORBA::Boolean DataFlowInterface_i::createConnection(
        const char* writer_port, DataFlowInterface_ptr reader_interface,
        const char* reader_port, ConnPolicy const& policy)
{
    RTT::OutputPortInterface* writer = dynamic_cast<RTT::OutputPortInterface*>(mdf->getPort(writer_port));
    if (writer == 0)
        return false;

    // Check if +reader_interface+ is local. If it is, use the non-CORBA
    // connection.
    RTT::DataFlowInterface* local_interface = DataFlowInterface_i::getLocalInterface(reader_interface);
    if (local_interface)
    {
        RTT::InputPortInterface* reader =
            dynamic_cast<RTT::InputPortInterface*>(local_interface->getPort(reader_port));
        if (!reader)
        {
            log(Warning) << "CORBA: createConnection() target is not an input port" << endlog();
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
        if (reader_interface->getPortType(reader_port) != Corba::Input)
            return false;

        RemoteInputPort port(writer->getTypeInfo(), reader_interface, reader_port, mpoa);
        return writer->createConnection(port, toRTT(policy));
    }
    catch(CORBA::COMM_FAILURE&) { throw; }
    catch(CORBA::TRANSIENT&) { throw; }
    catch(...) { return false; }
}

// standard constructor
ChannelElement_i::ChannelElement_i(RTT::detail::TypeTransporter const& transport,
	  PortableServer::POA_ptr poa)
    : transport(transport)
    , mpoa(PortableServer::POA::_duplicate(poa)) {}
ChannelElement_i::~ChannelElement_i() {}
PortableServer::POA_ptr ChannelElement_i::_default_POA()
{ return PortableServer::POA::_duplicate(mpoa); }
void ChannelElement_i::setRemoteSide(ChannelElement_ptr remote)
{ this->remote_side = RTT::Corba::ChannelElement::_duplicate(remote); }

