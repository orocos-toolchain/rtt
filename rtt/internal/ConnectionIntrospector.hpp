#ifndef CONNECTIONINTROSPECTOR_HPP
#define CONNECTIONINTROSPECTOR_HPP

#include "ConnID.hpp"
#include "ConnectionManager.hpp"
#include <boost/shared_ptr.hpp>
#include "../base/InputPortInterface.hpp"
#include "../base/PortInterface.hpp"
#include "../DataFlowInterface.hpp"
#include "../TaskContext.hpp"

#include <list>
#include <set>

namespace RTT
{

namespace internal
{

class ConnectionIntrospector
{
public:
    struct PortQualifier {
        std::string owner_name;
        std::string port_name;
        const base::PortInterface* port_ptr;
        bool is_forward;
        bool is_remote;

        PortQualifier() {}
        PortQualifier(const base::PortInterface* port)
                : port_ptr(port), is_remote(false) {
            // When the port is invalid, we have a remote port.
            if (!port_ptr) {
                port_name = "{REMOTE_PORT}";
                owner_name = "{REMOTE_OWNER}";
                is_remote = true;
                return;
            }
            port_name = port_ptr->getName();
            if (port_ptr->getInterface()) {
                owner_name = port_ptr->getInterface()->getOwner()->getName();
            } else {
                owner_name = "{FREE}";
            }
            if ( dynamic_cast<const base::InputPortInterface*>(port_ptr) ) {
                is_forward = false;
            } else {
                is_forward = true;
            }
        }

        bool operator==(const PortQualifier& other) const {
            // For simplicity, remote ports are always different for now.
            return !this->is_remote && this->owner_name == other.owner_name &&
                    this->port_name == other.port_name;
        }

        bool operator<(const PortQualifier& other) const {
            if (owner_name < other.owner_name) return true;
            if (owner_name > other.owner_name) return false;
            return port_name < other.port_name;
        }

        friend std::ostream& operator<<(std::ostream& os, const PortQualifier&);
    };

    ConnectionIntrospector(
            const ConnectionManager::ChannelDescriptor& descriptor,
            bool forward, int curr_depth);
    ConnectionIntrospector(const base::PortInterface* port);

    bool operator==(const ConnectionIntrospector& other) const;

    bool operator<(const ConnectionIntrospector& other) const;

    void createGraph(int depth = 1);

    boost::function<std::ostream&(std::ostream&)> indent(int i) const;

    std::ostream& printIndented(std::ostream& os, int i) const;

    void createGraph(int depth, std::list<ConnectionIntrospector*>& to_visit,
        std::set<ConnectionIntrospector>& visited);

//    void createDot(const std::string& file_name);

    friend std::ostream& operator<<(std::ostream& os,
                                    const ConnectionIntrospector&);

private:
    ConnectionIntrospector();

    bool is_forward;
    PortQualifier in_port;
    PortQualifier out_port;
    boost::shared_ptr<ConnID> connection_id;  // Can be casted to port if local
    // std::string connection_name;
    ConnPolicy connection_policy;
    int depth;
    std::list<ConnectionIntrospector> sub_connections;
};

}  // namespace internal

}  // namespace RTT

#endif // CONNECTIONINTROSPECTOR_HPP
