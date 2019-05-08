/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  RemotePorts.hpp

                        RemotePorts.hpp -  description
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


#ifndef RTT_CORBA_REMOTE_PORTS_HPP
#define RTT_CORBA_REMOTE_PORTS_HPP

#include "../../base/PortInterface.hpp"
#include "../../base/ChannelElement.hpp"
#include "../../InputPort.hpp"
#include "../../OutputPort.hpp"
#include "DataFlowI.h"
#include <cassert>

namespace RTT {
    namespace corba {
        /**
         * Contains the common CORBA management code for
         * proxy port objects representing ports available through CORBA.
         *
         * A Remote port object is just a view (ie proxy) on a real port.
         * This means that many remote port objects can point to the same
         * real port and that destroying a remote port object does not lead
         * to disconnection of the real port. All important functions are to be
         * forwarded to the real port.
         */
        template<typename PortClass>
        class RemotePort : public PortClass
        {
        protected:
            types::TypeInfo const* type_info;
            CDataFlowInterface_var dataflow;
            PortableServer::POA_var mpoa;

            bool connectionAdded( base::ChannelElementBase::shared_ptr channel, ConnPolicy const& policy ) { assert(false && "Can/Should not add connection to remote port object !");return false; }

        public:
            RemotePort(types::TypeInfo const* type_info,
                    CDataFlowInterface_ptr dataflow,
                    std::string const& name,
                    PortableServer::POA_ptr poa);

            PortableServer::POA_ptr _default_POA();
            CDataFlowInterface_ptr getDataFlowInterface() const;

            internal::ConnID* getPortID() const;

            types::TypeInfo const* getTypeInfo() const;
            int serverProtocol() const;
            bool connected() const;
            bool createStream( const ConnPolicy& policy );
            virtual bool addConnection(internal::ConnID* port_id, base::ChannelElementBase::shared_ptr channel_input, ConnPolicy const& policy);
            virtual void disconnect();
            using PortClass::disconnect;
            base::ChannelElementBase* getEndpoint() const;
        };

        /**
         * Proxy for a remote output port.
         * Allows for creation of a connection from the remote
         * port to a local input port.
         */
        class RemoteOutputPort
            : public RemotePort<base::OutputPortInterface>
        {
        public:
            RemoteOutputPort(types::TypeInfo const* type_info,
                    CDataFlowInterface_ptr dataflow,
                    std::string const& name,
                    PortableServer::POA_ptr poa);

            bool keepsLastWrittenValue() const;
            void keepLastWrittenValue(bool new_flag);

            virtual bool disconnect(PortInterface* port);
            using RemotePort<base::OutputPortInterface>::disconnect;

            bool createConnection( base::InputPortInterface& sink, ConnPolicy const& policy );
            using base::OutputPortInterface::createConnection;

            virtual base::DataSourceBase::shared_ptr getDataSource() const;

            base::PortInterface* clone() const;
            base::PortInterface* antiClone() const;
        };

        /**
         * Proxy for a remote input port.
         * Since it inherits from ConnFactory, it also allows
         * for building an output endpoint.
         * You can not access its datasource.
         */
        class RemoteInputPort
            : public RemotePort<base::InputPortInterface>
        {
        protected:
            /**
             * The ConnectionFactory calls this. Overload to do nothing when dealing with remote ports.
             * @param port_id
             * @param channel_input
             * @param policy
             * @return
             */
            virtual bool addConnection(internal::ConnID*, base::ChannelElementBase::shared_ptr, ConnPolicy const&) { return true; }

        public:
            RemoteInputPort(types::TypeInfo const* type_info,
                    CDataFlowInterface_ptr dataflow,
                    std::string const& name,
                    PortableServer::POA_ptr poa);

            void clear();

            /**
             * This method will do more than just building the output half, it
             * will create the two crucial ChannelElements on both sides of the
             * CORBA connection to marshal/demarshal the channel data. The
             * policy is used to determine if storage must be allocated remotely
             * or (has been allocated) locally. reader_ is ignored and must be this.
             * @param output_port The local port that will be sending data to the remote channel.
             * @param type The type of data to transport
             * @param reader_ Ignored. Must be this.
             * @param policy The policy for the ConnFactory.
             * @return The local endpoint for the output.
             */
            base::ChannelElementBase::shared_ptr buildRemoteChannelOutput(
                    base::OutputPortInterface& output_port,
                    types::TypeInfo const* type,
                    base::InputPortInterface& reader_,
                    ConnPolicy const& policy);

            /**
             * Overridden version of \ref InputPortInterface::createConnection(internal::SharedConnectionBase::shared_ptr, ConnPolicy const&), which
             * forwards the call to the remote data flow interface.
             */
            bool createConnection( internal::SharedConnectionBase::shared_ptr shared_connection, ConnPolicy const& policy = ConnPolicy() );
            using base::InputPortInterface::createConnection;

            base::PortInterface* clone() const;
            base::PortInterface* antiClone() const;

            virtual bool disconnect(PortInterface* port);
            using RemotePort<base::InputPortInterface>::disconnect;

            base::DataSourceBase* getDataSource();
        };
    }
}

#endif

