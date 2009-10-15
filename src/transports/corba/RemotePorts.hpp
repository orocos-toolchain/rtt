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

            bool connectionAdded( base::ChannelElementBase::shared_ptr channel, internal::ConnPolicy const& policy ) { assert(false && "Can/Should not add connection to remote port object !");return false; }

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
            bool createStream( const internal::ConnPolicy& policy );
            virtual bool addConnection(internal::ConnID* port_id, base::ChannelElementBase::shared_ptr channel_input, internal::ConnPolicy const& policy);
            void disconnect();
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

            using base::OutputPortInterface::createConnection;
            bool createConnection( base::InputPortInterface& sink, internal::ConnPolicy const& policy );

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
            : public RemotePort<base::InputPortInterface>,
              public internal::ConnFactory
        {
            typedef std::map<base::ChannelElementBase*,RTT::corba::CChannelElement_var> ChannelMap;
            ChannelMap channel_map;
        protected:
            /**
             * The ConnectionFactory calls this. Overload to do nothing when dealing with remote ports.
             * @param port_id
             * @param channel_input
             * @param policy
             * @return
             */
            virtual bool addConnection(internal::ConnID* port_id, base::ChannelElementBase::shared_ptr channel_input, internal::ConnPolicy const& policy) { return true; }
        public:
            RemoteInputPort(types::TypeInfo const* type_info,
                    CDataFlowInterface_ptr dataflow,
                    std::string const& name,
                    PortableServer::POA_ptr poa);

            internal::ConnFactory* getConnFactory();

            /**
             * This method will do more than just building the output half, it
             * will create the two crucial ChannelElements on both sides of the
             * CORBA connection to marshal/demarshal the channel data. The
             * policy is used to determine if storage must be allocated remotely
             * or (has been allocated) locally. reader_ is ignored and must be this.
             * @param type The type of data to transport
             * @param reader_ Ignored. Must be this.
             * @param policy The policy for the ConnFactory.
             * @return The local endpoint for the output.
             */
            base::ChannelElementBase* buildRemoteChannelOutput(types::TypeInfo const* type,
                    base::InputPortInterface& reader_,
                    internal::ConnPolicy const& policy);

            base::PortInterface* clone() const;
            base::PortInterface* antiClone() const;

            base::DataSourceBase* getDataSource();

            /**
             * For remote input port objects, this is forwarded to the other end
             * over the Data Flow Interface. The given channel must be the
             * output endpoint of a connection, which was built using buildRemoteChannelOutput.
             * So channel->getOutputEndpoint() == channel
             * @return
             */
            virtual bool channelReady(base::ChannelElementBase::shared_ptr channel);
        };
    }
}

#endif

