/***************************************************************************
  tag: FMTC  do nov 2 13:06:20 CET 2006  DataFlowI.h

                        DataFlowI.h -  description
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

#ifndef DATAFLOWI_H_
#define DATAFLOWI_H_

#include "../../rtt-config.h"
#include "corba.h"
#ifdef CORBA_IS_TAO
#include "DataFlowS.h"
#else
#include "DataFlowC.h"
#endif
#include "CorbaConversion.hpp"
#include "../../base/ChannelElement.hpp"
#include "../../internal/DataSources.hpp"
#include "CorbaTypeTransporter.hpp"
#include <list>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */


namespace RTT {

    namespace corba {

        /**
         * Base class for CORBA channel servers.
         * Derive from this class to implement a channel
         * which transports data over a CORBA connection.
         */
        class CRemoteChannelElement_i
            : public POA_RTT::corba::CRemoteChannelElement
            , public virtual PortableServer::RefCountServantBase
        {
        protected:
            CRemoteChannelElement_var remote_side;
            RTT::corba::CorbaTypeTransporter const& transport;
            PortableServer::POA_var mpoa;

        public:
            // standard constructor
            CRemoteChannelElement_i(corba::CorbaTypeTransporter const& transport,
			  PortableServer::POA_ptr poa);
            virtual ~CRemoteChannelElement_i();

            virtual RTT::corba::CRemoteChannelElement_ptr activate_this() {
                PortableServer::ObjectId_var oid = mpoa->activate_object(this); // ref count=2
                _remove_ref(); // ref count=1
                return _this();
            }

            virtual void transferSamples() = 0;

            PortableServer::POA_ptr _default_POA();

            void setRemoteSide(CRemoteChannelElement_ptr remote);
        };

        /**
         * Represents a remote data flow interface.
         * Allows to build connections from a local port
         * to a remote port.
         * If the ConnPolicy says so, this interface can also build
         * connections using other transports.
         */
        class CDataFlowInterface_i
            : public POA_RTT::corba::CDataFlowInterface
            , public virtual PortableServer::RefCountServantBase
        {
            interface::DataFlowInterface* mdf;
            PortableServer::POA_var mpoa;

            typedef std::list<
                std::pair<RTT::corba::CDataFlowInterface_var, interface::DataFlowInterface*>
                > ServantMap;
            static ServantMap s_servant_map;

            typedef std::list<
                std::pair<RTT::corba::CChannelElement_var, base::ChannelElementBase::shared_ptr>
                > ChannelList;
            ChannelList channel_list;
        public:
            // standard constructor
            CDataFlowInterface_i(interface::DataFlowInterface* interface, PortableServer::POA_ptr poa);
            virtual ~CDataFlowInterface_i();

            static void registerServant(CDataFlowInterface_ptr objref, interface::DataFlowInterface* obj);
            static void deregisterServant(interface::DataFlowInterface* obj);
            static interface::DataFlowInterface* getLocalInterface(CDataFlowInterface_ptr objref);
            static CDataFlowInterface_ptr getRemoteInterface(interface::DataFlowInterface* dfi, PortableServer::POA_ptr poa);

            virtual RTT::corba::CDataFlowInterface_ptr activate_this() {
                 PortableServer::ObjectId_var oid = mpoa->activate_object(this); // ref count=2
                 //_remove_ref(); // ref count=1
                 return _this();
             }

            PortableServer::POA_ptr _default_POA();

            // methods corresponding to defined IDL attributes and operations
            RTT::corba::CDataFlowInterface::CPortNames* getPorts();
            RTT::corba::CDataFlowInterface::CPortDescriptions* getPortDescriptions();
            RTT::corba::CPortType getPortType(const char* port_name);
            char* getDataType(const char* port_name);
            ::CORBA::Boolean isConnected(const char* port_name);
            ::CORBA::Boolean channelReady(const char* port_name, RTT::corba::CChannelElement_ptr channel);
            void disconnectPort(const char* port_name);

            CChannelElement_ptr buildChannelOutput(const char* reader_port, RTT::corba::CConnPolicy& policy);
            CChannelElement_ptr buildChannelInput(const char* writer_port, RTT::corba::CConnPolicy& policy);

            ::CORBA::Boolean createConnection( const char* writer_port,
                                               CDataFlowInterface_ptr reader_interface,
                                               const char* reader_port,
                                               RTT::corba::CConnPolicy & policy);
            void removeConnection( const char* writer_port,
                                               CDataFlowInterface_ptr reader_interface,
                                               const char* reader_port);

            ::CORBA::Boolean createStream( const char* port,
                                           RTT::corba::CConnPolicy & policy);
            void removeStream( const char* port, const char* stream_name);
        };
    }
};


#endif /* DATAFLOWI_H_  */

