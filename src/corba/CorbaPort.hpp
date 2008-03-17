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

namespace RTT
{ namespace Corba {

    /** 
     * A port used by ControlTaskProxy objects to connect
     * a corba data or buffer channel as an Orocos Port.
     */
    class CorbaPort
        : public PortInterface
    {
        AssignableExpression_var mdatachannel;
        BufferChannel_var mbufchannel;
        DataFlowInterface_var mdflow;
        ConnectionInterface::shared_ptr dc;
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

        virtual ConnectionInterface::shared_ptr connection() const {
            return dc;
        }

        using PortInterface::connectTo;

        virtual bool connectTo( ConnectionInterface::shared_ptr conn ) {
            dc = conn;
            return true;
        }

        virtual void disconnect() {
            dc = 0;
        }

        virtual PortInterface* clone() const {
            return new CorbaPort( this->getName(), mdflow.in(), 0 );
        }

        virtual PortInterface* antiClone() const {
            return new CorbaPort( this->getName(), mdflow.in(), 0 );
        }

        /**
         * Create a connection to a local port.
         */
        virtual ConnectionInterface::shared_ptr createConnection(PortInterface* other, ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree) 
        {
            // prevent infinite call-back:
            if ( dynamic_cast<CorbaPort*>(other) ) {
                log(Error) << "Can not connect two remote ports." <<endlog();
                log(Error) << "One must be local and one must be remote." <<endlog();
                return 0;
            }

            // if the other is local, its connection factory will setup the connection.
            return other->createConnection(this, con_type);
        }

        /**
         * Do not create a connection without a peer port.
         */
        virtual ConnectionInterface::shared_ptr createConnection(ConnectionTypes::ConnectionType con_type = ConnectionTypes::lockfree) 
        {
            Logger::In in("CorbaPort");
            log(Error) << "Can not create remote port connection without local port." <<endlog();
            return 0;
        }
    };

}}

#endif
