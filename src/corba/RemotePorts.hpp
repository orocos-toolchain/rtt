#ifndef RTT_CORBA_REMOTE_PORTS_HPP
#define RTT_CORBA_REMOTE_PORTS_HPP

#include "PortInterface.hpp"
#include "ConnectionInterface.hpp"
#include "Ports.hpp"
#include "DataFlowI.h"

namespace RTT {
    namespace Corba {
        template<typename PortClass>
        class RemotePort : public PortClass
        {
            struct PortID : public RTT::PortID
            {
                DataFlowInterface_var dataflow;
                std::string name;

                PortID(DataFlowInterface_ptr dataflow, std::string const& name);
            };

        protected:
            RTT::TypeInfo const* type_info;
            DataFlowInterface_var dataflow;
            PortableServer::POA_var mpoa;

        public:
            RemotePort(RTT::TypeInfo const* type_info,
                    DataFlowInterface_ptr dataflow,
                    std::string const& name,
                    PortableServer::POA_ptr poa);

            PortableServer::POA_ptr _default_POA();
            DataFlowInterface_ptr getDataFlowInterface() const;

            RTT::PortID* getPortID() const;
            bool isSameID(RTT::PortID const& id) const;

            RTT::TypeInfo const* getTypeInfo() const;
            int serverProtocol() const;
            bool connected() const;

            void disconnect();
        };

        class RemoteWritePort
            : public RemotePort<RTT::WritePortInterface>
        {
        public:
            RemoteWritePort(RTT::TypeInfo const* type_info,
                    DataFlowInterface_ptr dataflow,
                    std::string const& name,
                    PortableServer::POA_ptr poa);

            bool keepsLastWrittenValue() const;
            void keepLastWrittenValue(bool new_flag);

            using RTT::WritePortInterface::createConnection;
            bool createConnection( ReadPortInterface& sink, RTT::ConnPolicy const& policy );

            RTT::PortInterface* clone() const;
            RTT::PortInterface* antiClone() const;
        };

        class RemoteReadPort
            : public RemotePort<RTT::ReadPortInterface>
            , public RTT::ConnFactory
        {
        public:
            RemoteReadPort(RTT::TypeInfo const* type_info,
                    DataFlowInterface_ptr dataflow,
                    std::string const& name,
                    PortableServer::POA_ptr poa);

            ConnFactory* getConnFactory();

            RTT::ConnElementBase* buildReaderHalf(RTT::TypeInfo const* type,
                    RTT::ReadPortInterface& reader_,
                    RTT::ConnPolicy const& policy);

            RTT::PortInterface* clone() const;
            RTT::PortInterface* antiClone() const;
        };
    }
}

#endif

