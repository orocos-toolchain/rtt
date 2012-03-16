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
        class RTT_API TemplateConnFactory : public ConnFactory
        {
        public:
            base::InputPortInterface*  inputPort(std::string const& name) const { return new InputPort<T>(name); }
            base::OutputPortInterface* outputPort(std::string const& name) const { return new OutputPort<T>(name); }

            base::ChannelElementBase::shared_ptr buildDataStorage(ConnPolicy const& policy) const {
                return ConnFactory::buildDataStorage<T>(policy);
            }

            base::ChannelElementBase::shared_ptr buildChannelOutput(base::InputPortInterface& port) const
            {
                return ConnFactory::buildChannelOutput(
                                                                 static_cast<RTT::InputPort<T>&>(port), new internal::SimpleConnID());
            }

            base::ChannelElementBase::shared_ptr buildChannelInput(base::OutputPortInterface& port) const
            {
                return ConnFactory::buildChannelInput(
                                                                static_cast<RTT::OutputPort<T>&>(port), new internal::SimpleConnID(), 0 );
            }
        };
}
}

#endif
