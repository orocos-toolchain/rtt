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
         */
        template<typename PortClass>
        class RemotePort : public PortClass
        {
        protected:
            types::TypeInfo const* type_info;
            CDataFlowInterface_var dataflow;
            PortableServer::POA_var mpoa;

        public:
            RemotePort(types::TypeInfo const* type_info,
                    CDataFlowInterface_ptr dataflow,
                    std::string const& name,
                    PortableServer::POA_ptr poa);

            PortableServer::POA_ptr _default_POA();
            CDataFlowInterface_ptr getDataFlowInterface() const;

            base::PortID* getPortID() const;
            bool isSameID(base::PortID const& id) const;

            types::TypeInfo const* getTypeInfo() const;
            int serverProtocol() const;
            bool connected() const;
            bool createStream( const internal::ConnPolicy& policy );

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
            virtual void connectionAdded( base::ChannelElementBase::shared_ptr channel_input, internal::ConnPolicy const& policy ) {
                assert(false && "Can not add connection to remote port object !");
            }


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
            , public internal::ConnFactory
        {
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
            base::ChannelElementBase* buildRemoteOutputHalf(types::TypeInfo const* type,
                    base::InputPortInterface& reader_,
                    internal::ConnPolicy& policy);

            base::PortInterface* clone() const;
            base::PortInterface* antiClone() const;

            base::DataSourceBase* getDataSource();
        };
    }
}

#endif

