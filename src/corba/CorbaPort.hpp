/***************************************************************************
  tag: FMTC  do nov 2 13:06:19 CET 2006  CorbaPort.hpp

                        CorbaPort.hpp -  description
                           -------------------
    begin                : do november 02 2006
    copyright            : (C) 2006 FMTC
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



#ifndef ORO_CORBA_PORT_HPP
#define ORO_CORBA_PORT_HPP

#include <string>
#include "../PortInterface.hpp"
#include "DataFlowC.h"
#include "orbsvcs/CosEventChannelAdminC.h"
#include "orbsvcs/CosEventCommC.h"
#include <tao/corba.h>
#include <tao/PortableServer/PortableServer.h>
#include "CorbaConnection.hpp"
#include "CorbaLib.hpp"

namespace RTT
{ namespace Corba {

    /**
     * A port used by ControlTaskProxy objects to connect
     * a corba data or buffer channel as an Orocos Port.
     */
    class CorbaPort
        : public PortInterface
    {
    protected:
        friend class ConnectionInterface;
        AssignableExpression_var mdatachannel;
        BufferChannel_var mbufchannel;
        DataFlowInterface_var mdflow;
        mutable RTT::ConnectionInterface::shared_ptr dc;
        bool connect(RTT::ConnectionInterface::shared_ptr conn)
        {
            if ( dc || !conn )
                return false;
            // create a server and connect the remote port to this server.
            // ALWAYS check getBuffer() first because getDataSource() always returns a data source.
            if ( conn->getBuffer() ) {
                // OK: we have a buffered connection.
                if (mdflow->getConnectionModel(this->getName().c_str()) != DataFlowInterface::Buffered)
                    return false;
                detail::TypeTransporter* tt = getTypeInfo()->getProtocol(ORO_CORBA_PROTOCOL_ID);
                if (tt) {
                    BufferChannel_var bc = (BufferChannel_ptr)tt->bufferServer( conn->getBuffer(), 0);
                    if (mdflow->connectBufferPort(this->getName().c_str(), bc.in() ) ) {
                        dc=conn;
                        return true;
                    }
                }
                return false;
            }
            if ( conn->getDataSource() ) {
                if (mdflow->getConnectionModel(this->getName().c_str()) != DataFlowInterface::Data)
                    return false;
                detail::TypeTransporter* tt = getTypeInfo()->getProtocol(ORO_CORBA_PROTOCOL_ID);
                if (tt) {
                    Expression_var expr = (Expression_ptr)tt->dataServer(conn->getDataSource(), 0);
                    AssignableExpression_var ae = AssignableExpression::_narrow( expr.in() );
                    if ( CORBA::is_nil(expr.in()) || CORBA::is_nil(ae.in()) ) {
                        log(Error) << "Could not create server from "<<this->getName() <<endlog();
                        return false;
                    }
                    if (mdflow->connectDataPort(this->getName().c_str(), ae.in() ) ) {
                        dc=conn;
                        return true;
                    }
                }
                return false;
            }
            return false;
        }

    public:
        CorbaPort(const std::string& name, DataFlowInterface_ptr dflow, PortableServer::POA_ptr )
            : PortInterface(name),
              mdatachannel(0), mbufchannel(0),
              mdflow( DataFlowInterface::_duplicate(dflow) )
        {
        }

        CorbaPort(const std::string& name, DataFlowInterface_ptr dflow, AssignableExpression_ptr datachannel, PortableServer::POA_ptr )
            : PortInterface(name),
              mdatachannel(AssignableExpression::_duplicate(datachannel)), mbufchannel(0),
              mdflow( DataFlowInterface::_duplicate(dflow) )
        {
        }

        CorbaPort(const std::string& name, DataFlowInterface_ptr dflow, BufferChannel_ptr bufchannel, PortableServer::POA_ptr )
            : PortInterface(name),
              mdatachannel(0), mbufchannel( BufferChannel::_duplicate(bufchannel)),
              mdflow( DataFlowInterface::_duplicate(dflow) )
        {
        }

        AssignableExpression_ptr getDataChannel()
        {
            if ( mdatachannel.in() )
                return AssignableExpression::_duplicate(mdatachannel);
            return mdflow->createDataChannel( this->getName().c_str() );
        }

        BufferChannel_ptr getBufferChannel()
        {
            if ( mbufchannel.in() )
                return BufferChannel::_duplicate(mbufchannel);
            return mdflow->createBufferChannel( this->getName().c_str() );
        }

        virtual ConnectionModel getConnectionModel() const {
            return ConnectionModel(int(mdflow->getConnectionModel( this->getName().c_str() )));
        }

        /**
         * Get the PortType of this port.
         */
        virtual PortType getPortType() const {
            return PortType(int(mdflow->getPortType( this->getName().c_str() )));
        }

        virtual const TypeInfo* getTypeInfo() const {
            TypeInfo* ret = TypeInfoRepository::Instance()->type( mdflow->getDataType(this->getName().c_str()) ) ;
            if (ret) return ret;
            return detail::DataSourceTypeInfo<detail::UnknownType>::getTypeInfo();
        }

        virtual bool connected() const {
            return mdflow->isConnected( this->getName().c_str() );
        }

        virtual RTT::ConnectionInterface::shared_ptr connection() const {
            // return a connection object if remote side is connected.
            if ( !dc && this->connected() )
                dc = new CorbaConnection(this->getName(), mdflow.in(), 0 );
            return dc;
        }

        using PortInterface::connectTo;

        virtual bool connectTo( RTT::ConnectionInterface::shared_ptr conn ) {
            // Since a connection is given, an existing impl exists.
            if (this->getConnectionModel() == Buffered ) {
                BufferBase::shared_ptr impl = conn->getBuffer();
                if (!impl)
                    return false;
                detail::TypeTransporter* tt = conn->getTypeInfo()->getProtocol( ORO_CORBA_PROTOCOL_ID );
                if (tt) {
                    //let remote side create to local server.
                    Corba::BufferChannel_var bufs = (Corba::BufferChannel_ptr) tt->bufferServer(impl, 0);
                    mdflow->connectBufferPort( this->getName().c_str(), bufs.in() );
                    dc = conn;
                    return true;
                }
            }
            if (this->getConnectionModel() == Data ) {
                DataSourceBase::shared_ptr impl = conn->getDataSource();
                if (!impl)
                    return false;
                detail::TypeTransporter* tt = conn->getTypeInfo()->getProtocol( ORO_CORBA_PROTOCOL_ID );
                if (tt) {
                    //let remote side create to local server.
                    Corba::Expression_var expr = (Corba::Expression_ptr) tt->dataServer( impl, 0 );
                    Corba::AssignableExpression_var as_expr = Corba::AssignableExpression::_narrow( expr.in() );
                    mdflow->connectDataPort( this->getName().c_str(), as_expr.in() );
                    dc = conn;
                    return true;
                }
            }
            return false;
        }

        virtual void disconnect() {
            // disconnect the remote port
            mdflow->disconnect( this->getName().c_str() );
            dc = 0;
        }

        virtual PortInterface* clone() const {
            return new CorbaPort( this->getName(), mdflow.in(), 0 );
        }

        virtual PortInterface* antiClone() const {
            return new CorbaPort( this->getName(), mdflow.in(), 0 );
        }

        /**
         * Create a remote server to which one can connect to.
         */
        virtual RTT::ConnectionInterface::shared_ptr createConnection(ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree)
        {
            Logger::In in("CorbaPort");
            // create a connection one can write to.
            if ( this->getPortType() != ReadPort ) {
                RTT::ConnectionInterface::shared_ptr ci = new CorbaConnection( this->getName(), mdflow.in(), 0 );
                ci->addPort(this);
                return ci;
            }
            // read ports do not create writable connections.
            return 0;
        }

        virtual RTT::ConnectionInterface::shared_ptr createConnection( BufferBase::shared_ptr buf )
        {
            RTT::ConnectionInterface::shared_ptr ci;
            if (mdflow->getConnectionModel(this->getName().c_str()) != DataFlowInterface::Buffered)
                return ci;
            detail::TypeTransporter* tt = getTypeInfo()->getProtocol(ORO_CORBA_PROTOCOL_ID);
            if (tt) {
                BufferChannel_var bc = (BufferChannel_ptr)tt->bufferServer(buf, 0);
                if (bc.in() ) {
                    ci = new CorbaConnection( this->getName(), mdflow.in(), bc.in(), 0 );
                    ci->addPort(this);
                } // else: leave ci empty.
            }
            return ci;
        }

        virtual RTT::ConnectionInterface::shared_ptr createConnection( DataSourceBase::shared_ptr data )
        {
            RTT::ConnectionInterface::shared_ptr ci;
            if (mdflow->getConnectionModel(this->getName().c_str()) != DataFlowInterface::Data)
                return ci;
            detail::TypeTransporter* tt = getTypeInfo()->getProtocol(ORO_CORBA_PROTOCOL_ID);
            if (tt) {
                Expression_var ex = (Expression_ptr)tt->dataServer(data, 0);
                AssignableExpression_var ae = AssignableExpression::_narrow( ex.in() );
                if ( !CORBA::is_nil(ae.in()) ) {
                    ci = new CorbaConnection( this->getName(), mdflow.in(), ae.in(), 0 );
                    ci->addPort(this);
                } // else: leave ci empty.
            }
            return ci;
        }

        virtual int serverProtocol() const {
            return ORO_CORBA_PROTOCOL_ID;
        }
    };

}}

#endif
