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
#include "../ConnectionInterface.hpp"
#include "../Logger.hpp"
#include "ControlTaskProxy.hpp"
#include "CorbaTypeTransporter.hpp"
#include "../Ports.hpp"

#include <iostream>

using namespace RTT::Corba;

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

namespace
{
    // This implementation of a remote port is private to the connection-establishment code, and
    // should not be use outside of this file without proper review
    class RemoteReadPort
        : public RTT::ReadPortInterface
        , public RTT::ConnFactory
    {
	RTT::TypeInfo const* type_info;
        DataFlowInterface_var dataflow;
        PortableServer::POA_var mpoa;

    public:
        RemoteReadPort(RTT::TypeInfo const* type_info, DataFlowInterface_var dataflow, std::string const& name,
                PortableServer::POA_ptr poa);

        DataFlowInterface_var getDataFlowInterface() const { return dataflow; }

        ConnFactory* getConnFactory() { return this; }
        RTT::ConnElementBase* buildReaderHalf(RTT::TypeInfo const* type,
                RTT::PortInterface& reader_,
                RTT::ConnPolicy const& policy);

	RTT::TypeInfo const* getTypeInfo() const { return type_info; }
	RTT::PortInterface* clone() const { return 0; }
	RTT::PortInterface* antiClone() const { return 0; }
	int serverProtocol() const { return ORO_CORBA_PROTOCOL_ID; }
    };

    RemoteReadPort::RemoteReadPort(RTT::TypeInfo const* type_info,
	    DataFlowInterface_var dataflow, std::string const& reader_port,
            PortableServer::POA_ptr poa)
        : RTT::ReadPortInterface(reader_port)
	, type_info(type_info)
        , dataflow(dataflow)
        , mpoa(PortableServer::POA::_duplicate(poa)) { }

    RTT::ConnElementBase* RemoteReadPort::buildReaderHalf(RTT::TypeInfo const* type,
            RTT::PortInterface& reader_,
            RTT::ConnPolicy const& policy)
    {
        ConnElement_var remote =
            dataflow->buildReaderHalf(CORBA::string_dup(getName().c_str()), toCORBA(policy));

        ConnElement_i*  local;
        PortableServer::ServantBase_var servant = local =
            static_cast<CorbaTypeTransporter*>(type->getProtocol(ORO_CORBA_PROTOCOL_ID))->createConnElement_i(mpoa);

	local->setRemoteSide(remote);
	remote->setRemoteSide(local->_this());

        // The ConnElementBase object that represents reader_half on this side
        return dynamic_cast<RTT::ConnElementBase*>(local);
    }
}



DataFlowInterface_i::DataFlowInterface_i (RTT::DataFlowInterface* interface, PortableServer::POA_ptr poa)
    : mdf(interface), mpoa(PortableServer::POA::_duplicate(poa))
{
}

DataFlowInterface_i::~DataFlowInterface_i ()
{  }

PortableServer::POA_ptr DataFlowInterface_i::_default_POA()
{
    return PortableServer::POA::_duplicate(mpoa);
}

DataFlowInterface::PortNames * DataFlowInterface_i::getPorts()
{
  // Add your implementation here
    ::RTT::DataFlowInterface::PortNames ports = mdf->getPortNames();

    RTT::Corba::DataFlowInterface::PortNames_var pn = new RTT::Corba::DataFlowInterface::PortNames();
    pn->length( ports.size() );

    for (unsigned int i=0; i != ports.size(); ++i )
        pn[i] = CORBA::string_dup( ports[i].c_str() );

    return pn._retn();
}

DataFlowInterface::PortType DataFlowInterface_i::getPortType(const char * port_name)
{
    RTT::PortInterface* p = mdf->getPort(port_name);
    if (p == 0)
        throw NoSuchPortException();

    if (dynamic_cast<ReadPortInterface*>(p))
        return RTT::Corba::DataFlowInterface::Reader;
    else return RTT::Corba::DataFlowInterface::Writer;
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
    if ( p == 0)
        throw NoSuchPortException();
    p->disconnect();
}


ConnElement_ptr DataFlowInterface_i::buildReaderHalf(
        const char* reader_port, ConnPolicy const& corba_policy)
{
    RTT::ReadPortInterface* reader = dynamic_cast<RTT::ReadPortInterface*>(mdf->getPort(reader_port));
    if (reader == 0)
        throw NoSuchPortException();

    TypeInfo const* type_info = reader->getTypeInfo();
    if (!type_info)
        throw NoCorbaTransport();

    CorbaTypeTransporter* transporter =
        dynamic_cast<CorbaTypeTransporter*>(type_info->getProtocol(ORO_CORBA_PROTOCOL_ID));
    if (!transporter)
        throw NoCorbaTransport();

    RTT::ConnElementBase* element = transporter->buildReaderHalf(*reader, toRTT(corba_policy));
    ConnElement_i* this_element;
    PortableServer::ServantBase_var servant = this_element = transporter->createConnElement_i(mpoa);
    dynamic_cast<ConnElementBase*>(this_element)->setReader(element);

    return RTT::Corba::ConnElement::_duplicate(this_element->_this());
}

::CORBA::Boolean DataFlowInterface_i::createConnection(
        const char* writer_port, DataFlowInterface_ptr reader_interface,
        const char* reader_port, ConnPolicy const& policy)
{
    RTT::WritePortInterface* writer = dynamic_cast<RTT::WritePortInterface*>(mdf->getPort(writer_port));
    if (writer == 0)
        throw NoSuchPortException();

    RemoteReadPort port(writer->getTypeInfo(), reader_interface, reader_port, mpoa);
    return writer->createConnection(port, toRTT(policy));
}

// standard constructor
ConnElement_i::ConnElement_i(RTT::detail::TypeTransporter const& transport,
	  PortableServer::POA_ptr poa)
    : transport(transport)
    , mpoa(PortableServer::POA::_duplicate(poa)){}
ConnElement_i::~ConnElement_i() { }

PortableServer::POA_ptr ConnElement_i::_default_POA()
{
    return PortableServer::POA::_duplicate(mpoa);
}

// methods corresponding to defined IDL attributes and operations
void ConnElement_i::setRemoteSide(ConnElement_ptr remote)
{
    this->remote_side = remote;
}

