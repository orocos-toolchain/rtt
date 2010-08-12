#ifndef RTT_TRANSPORT_CORBA_PLUGIN
#define RTT_TRANSPORT_CORBA_PLUGIN

#include <rtt/types/TransportPlugin.hpp>

namespace RTT
{
    namespace corba
    {
        /** The CORBA transport plugin */
        struct CorbaLibPlugin: public RTT::types::TransportPlugin
        {
            bool registerTransport(std::string name, RTT::types::TypeInfo* ti);
            std::string getTransportName() const;
            std::string getTypekitName() const;
            std::string getName() const;
        };
    }
}

#endif

