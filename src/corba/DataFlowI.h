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

#include "../rtt-config.h"
#include "corba.h"
#ifdef CORBA_IS_TAO
#include "DataFlowS.h"
#else
#include "DataFlowC.h"
#endif
#include "CorbaConversion.hpp"
#include "../ChannelInterface.hpp"
#include "../DataSources.hpp"
#include "../TypeTransporter.hpp"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */


namespace RTT {
    class DataFlowInterface;
    class InputPortInterface;

    namespace Corba {
        class CorbaTypeTransporter;

        class ChannelElement_i
            : public POA_RTT::Corba::ChannelElement
            , public virtual PortableServer::RefCountServantBase
        {
        protected:
            ChannelElement_var remote_side;
            RTT::detail::TypeTransporter const& transport;
            PortableServer::POA_var mpoa;

        public:
            // standard constructor
            ChannelElement_i(detail::TypeTransporter const& transport,
			  PortableServer::POA_ptr poa);
            virtual ~ChannelElement_i();

            PortableServer::POA_ptr _default_POA();

            void setRemoteSide(ChannelElement_ptr remote);
	};

        class DataFlowInterface_i
            : public POA_RTT::Corba::DataFlowInterface
            , public virtual PortableServer::RefCountServantBase
        {
            RTT::DataFlowInterface* mdf;
	    PortableServer::POA_var mpoa;

        public:
            // standard constructor
            DataFlowInterface_i(RTT::DataFlowInterface* interface, PortableServer::POA_ptr poa);
            virtual ~DataFlowInterface_i();

		PortableServer::POA_ptr _default_POA();

            // methods corresponding to defined IDL attributes and operations
            Corba::DataFlowInterface::PortNames* getPorts();
            Corba::DataFlowInterface::PortDescriptions* getPortDescriptions();
            Corba::PortType getPortType(const char* port_name);
            char* getDataType(const char* port_name);
            ::CORBA::Boolean isConnected(const char* port_name);
            void disconnect(const char* port_name);
            void disconnectPort(
                    const char* writer_port,
		    DataFlowInterface_ptr reader_interface, const char* reader_port);

            ChannelElement_ptr buildReaderHalf(const char* reader_port, const ConnPolicy& policy);

            ::CORBA::Boolean createConnection(
                    const char* writer_port,
		    DataFlowInterface_ptr reader_interface, const char* reader_port,
		    ConnPolicy const& policy);
        };
    }
};


#endif /* DATAFLOWI_H_  */

