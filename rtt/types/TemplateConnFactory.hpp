#ifndef ORO_TEMPLATE_CONN_FACTORY_HPP
#define ORO_TEMPLATE_CONN_FACTORY_HPP

#include <string>
#include "../internal/ConnFactory.hpp"

#include "../InputPort.hpp"
#include "../OutputPort.hpp"
#include "../Logger.hpp"

namespace RTT
{ namespace types {
        using internal::ConnFactory;

        template<class T>
        class TemplateConnFactory : public ConnFactory
        {
        public:
            base::InputPortInterface*  inputPort(std::string const& name) const { return new InputPort<T>(name); }
            base::OutputPortInterface* outputPort(std::string const& name) const { return new OutputPort<T>(name); }

            base::ChannelElementBase::shared_ptr buildDataStorage(ConnPolicy const& policy) const {
                return ConnFactory::buildDataStorage<T>(policy);
            }

            base::ChannelElementBase::shared_ptr buildChannelOutput(base::InputPortInterface& port, ConnPolicy const& policy) const
            {
                return ConnFactory::buildChannelOutput(static_cast<RTT::InputPort<T>&>(port), policy);
            }

            base::ChannelElementBase::shared_ptr buildChannelInput(base::OutputPortInterface& port, ConnPolicy const& policy) const
            {
                return ConnFactory::buildChannelInput(static_cast<RTT::OutputPort<T>&>(port), policy);
            }

            internal::SharedConnectionBase::shared_ptr buildSharedConnection(base::OutputPortInterface *output_port, base::InputPortInterface *input_port, ConnPolicy const& policy) const
            {
                return ConnFactory::buildSharedConnection(static_cast<RTT::OutputPort<T> *>(output_port), input_port, policy);
            }
        };
}
}

#endif
