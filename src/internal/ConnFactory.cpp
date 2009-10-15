#include "../Port.hpp"
#include "ConnFactory.hpp"
#include "../base/InputPortInterface.hpp"
#include "../interface/DataFlowInterface.hpp"

using namespace std;
using namespace RTT;
using namespace RTT::internal;

bool LocalConnID::isSameID(ConnID const& id) const
{
    LocalConnID const* real_id = dynamic_cast<LocalConnID const*>(&id);
    if (!real_id)
        return false;
    else return real_id->ptr == this->ptr;
}

ConnID* LocalConnID::clone() const {
    return new LocalConnID(this->ptr);
}

bool StreamConnID::isSameID(ConnID const& id) const
{
    StreamConnID const* real_id = dynamic_cast<StreamConnID const*>(&id);
    if (!real_id)
        return false;
    else return real_id->name_id == this->name_id;
}

ConnID* StreamConnID::clone() const {
    return new StreamConnID(this->name_id);
}

base::ChannelElementBase* RTT::internal::ConnFactory::createRemoteConnection(base::OutputPortInterface& output_port, base::InputPortInterface& input_port, const ConnPolicy& policy)
{
    // Remote connection
    base::ChannelElementBase* output_half = 0;
    // if the policy's transport is set to zero, use the input ports server protocol,
    // otherwise, use the policy's protocol
    int transport = policy.transport == 0 ? input_port.serverProtocol() : policy.transport;
    types::TypeInfo const* type_info = output_port.getTypeInfo();
    if (!type_info || input_port.getTypeInfo() != type_info)
    {
        log(Error) << "Type of port " << output_port.getName() << " is not registered into the type system, cannot marshal it into the right transporter" << endlog();
        // There is no type info registered for this type
        return false;
    }
    else if ( !type_info->getProtocol( transport ) )
    {
        log(Error) << "type " << type_info->getTypeName() << " cannot be marshalled into the right transporter" << endlog();
        // This type cannot be marshalled into the right transporter
        return false;
    }
    else
    {
        assert( input_port.getConnFactory() );
        output_half = input_port.
                getConnFactory()->buildRemoteChannelOutput(type_info, input_port, policy);
    }
    return output_half;
}
