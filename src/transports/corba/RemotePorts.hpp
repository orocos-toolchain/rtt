#ifndef RTT_CORBA_REMOTE_PORTS_HPP
#define RTT_CORBA_REMOTE_PORTS_HPP

#include "../../base/PortInterface.hpp"
#include "../../base/ChannelElement.hpp"
#include "../../InputPort.hpp"
#include "../../OutputPort.hpp"
#include "DataFlowI.h"

namespace RTT {
    namespace corba {
        template<typename PortClass>
        class RemotePort : public PortClass
        {
        protected:
            types::TypeInfo const* type_info;
            DataFlowInterface_var dataflow;
            PortableServer::POA_var mpoa;

        public:
            RemotePort(types::TypeInfo const* type_info,
                    DataFlowInterface_ptr dataflow,
                    std::string const& name,
                    PortableServer::POA_ptr poa);

            PortableServer::POA_ptr _default_POA();
            DataFlowInterface_ptr getDataFlowInterface() const;

            base::PortID* getPortID() const;
            bool isSameID(base::PortID const& id) const;

            types::TypeInfo const* getTypeInfo() const;
            int serverProtocol() const;
            bool connected() const;

            void disconnect();
        };

        class RemoteOutputPort
            : public RemotePort<base::OutputPortInterface>
        {
        public:
            RemoteOutputPort(types::TypeInfo const* type_info,
                    DataFlowInterface_ptr dataflow,
                    std::string const& name,
                    PortableServer::POA_ptr poa);

            bool keepsLastWrittenValue() const;
            void keepLastWrittenValue(bool new_flag);

            using base::OutputPortInterface::createConnection;
            bool createConnection( base::InputPortInterface& sink, internal::ConnPolicy const& policy );

            base::PortInterface* clone() const;
            base::PortInterface* antiClone() const;
        };

        class RemoteInputPort
            : public RemotePort<base::InputPortInterface>
            , public internal::ConnFactory
        {
        public:
            RemoteInputPort(types::TypeInfo const* type_info,
                    DataFlowInterface_ptr dataflow,
                    std::string const& name,
                    PortableServer::POA_ptr poa);

            internal::ConnFactory* getConnFactory();

            base::ChannelElementBase* buildOutputHalf(types::TypeInfo const* type,
                    base::InputPortInterface& reader_,
                    internal::ConnPolicy const& policy);

            base::PortInterface* clone() const;
            base::PortInterface* antiClone() const;

            base::DataSourceBase* getDataSource();
        };
    }
}

#endif

