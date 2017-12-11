#include "ConnectionIntrospector.hpp"

namespace RTT {
namespace internal {

    ConnectionIntrospector::PortQualifier::PortQualifier() {}
    ConnectionIntrospector::PortQualifier::PortQualifier(const base::PortInterface* port)
            : port_ptr(port), is_remote(false) {
        // If the port is invalid, we have a remote port.
        if (!port_ptr) {
            port_name = "{REMOTE_PORT}";
            owner_name = "{REMOTE_OWNER}";
            is_remote = true;
            return;
        }
        port_name = port_ptr->getName();
        if (port_ptr->getInterface() && port_ptr->getInterface()->getOwner()) {
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

    bool ConnectionIntrospector::PortQualifier::operator==(
            const PortQualifier& other) const {
        // For simplicity, remote ports are always different for now.
        return !this->is_remote && !other.is_remote &&
                (this->owner_name == other.owner_name) &&
                (this->port_name == other.port_name);
    }

    bool ConnectionIntrospector::PortQualifier::operator<(
            const PortQualifier& other) const {
        if (owner_name < other.owner_name) return true;
        if (owner_name > other.owner_name) return false;
        return port_name < other.port_name;
    }

    ConnectionIntrospector::ConnectionIntrospector(
            const ConnectionManager::ChannelDescriptor& descriptor,
            bool forward, int curr_depth) {
        is_forward = forward;
        connection_id = descriptor.get<0>();
        connection_policy = descriptor.get<2>();
        in_port = PortQualifier(
                    descriptor.get<1>()->getOutputEndPoint()->getPort());
        out_port = PortQualifier(
                    descriptor.get<1>()->getInputEndPoint()->getPort());
        depth = curr_depth;
    }

    ConnectionIntrospector::ConnectionIntrospector(
            const base::PortInterface* port_ptr) {
        PortQualifier port(port_ptr);
        if (port.is_forward) {
            out_port = port;
        } else {
            in_port = port;
        }
        // This is the fake "incoming" port connection, so forward is the
        // opposite of the port value.
        is_forward = !port.is_forward;
        depth = 0;
    }

    ConnectionIntrospector::ConnectionIntrospector(const TaskContext* tc) {
        if (!tc) {
            in_port.is_remote = true;
            in_port.owner_name = "{EMPTY_COMPONENT}";
            is_forward = true;
            depth = -1;
            return;
        }
        in_port.owner_name = tc->getName();
        is_forward = true;
        depth = -1;
        if (tc->ports()) {
            DataFlowInterface::PortNames port_names = tc->ports()->getPortNames();
            for (size_t j = 0; j < port_names.size(); ++j) {
                const std::string& port_name = port_names.at(j);
                base::PortInterface* port_ptr = tc->getPort(port_name);
                ConnectionIntrospector ci(port_ptr);
                sub_connections.push_back(ci);
            }
        }
    }

    bool ConnectionIntrospector::operator==(const ConnectionIntrospector& other)
            const {
        return this->in_port == other.in_port &&
                this->out_port == other.out_port;
    }

    bool ConnectionIntrospector::operator<(const ConnectionIntrospector& other)
            const {
        if (out_port < other.out_port) return true;
        if (other.out_port < out_port) return false;
        return in_port < other.in_port;
    }

    void ConnectionIntrospector::createGraph(int depth) {
        std::list<ConnectionIntrospector*> to_visit;
        if (sub_connections.empty()) {
            to_visit.push_back(this);
        } else {
            for (std::list< ConnectionIntrospector >::iterator
                    it = this->sub_connections.begin();
                    it != this->sub_connections.end(); ++it) {
                to_visit.push_back(&(*it));
            }
        }
        std::set<ConnectionIntrospector> visited;
        createGraph(depth, to_visit, visited);
    }

    void ConnectionIntrospector::createGraph(
            int depth,
            std::list<ConnectionIntrospector*>& to_visit,
            std::set<ConnectionIntrospector>& visited) {
        if (depth < 1) {depth = 1;}

        while (!to_visit.empty()) {
            ConnectionIntrospector& node(*(to_visit.front()));
            to_visit.pop_front();
            int curr_depth = node.depth;
            if (curr_depth >= depth) {
                continue;
            }
            std::list<ConnectionIntrospector>& connection_list = node.sub_connections;
            PortQualifier& port = node.is_forward ? node.in_port : node.out_port;

            // Would be false for remote ports.
            if (!port.is_remote) {
                std::list<ConnectionManager::ChannelDescriptor>
                        connections = port.port_ptr->getManager()->getConnections();
                for (std::list<ConnectionManager::ChannelDescriptor>::const_iterator it = connections.begin();
                     it != connections.end(); ++it) {
                    // Push back one connection, and add the node to the "to_visit" list.
                    ConnectionIntrospector
                            conn_descriptor(*it, !node.is_forward, curr_depth + 1);
                    if (visited.count(conn_descriptor)) {
                        continue;
                    }
                    visited.insert(conn_descriptor);
                    connection_list.push_back(conn_descriptor);
                    to_visit.push_back(&(connection_list.back()));
                }
            } else {
                if (node.connection_id) {
                    port.port_name = node.connection_id->portName();
                }
            }
        }
    }

    std::ostream& operator<<(
            std::ostream& os,
            const ConnectionIntrospector::PortQualifier& pq) {
        os << pq.owner_name << "." << pq.port_name;
        return os;
    }

    std::ostream& ConnectionIntrospector::printIndented(std::ostream& os,
            int i) const {
        const int currIndent = 4 * this->depth + i;
        if (this->depth == -1) {
            // For depth -1, only log the component name.
            os << std::string(i, ' ') << this->in_port.owner_name
               << " COMPONENT\n";
        } else if (this->depth == 0) {
            // For depth 0, only log the port.
            os << std::string(currIndent, ' ')
               << (this->is_forward ? " IN " : " OUT ")
               << (this->is_forward ? this->in_port
                                         : this->out_port) << ":\n";
        } else if (this->depth > 0) {
            // Positive depth, log the full connection information.
            os << std::string(currIndent, ' ')
               << (this->is_forward ? " -> IN " : " <- OUT ")
               << (this->is_forward ? this->in_port
                                         : this->out_port)
               << " : (" << this->connection_id->typeString()
               << " = " << this->connection_policy << ")\n";
        }
        for (std::list< ConnectionIntrospector >::const_iterator
                it = this->sub_connections.begin();
             it != this->sub_connections.end(); ++it) {
            os << *it;
        }
        return os;
    }

    boost::function<std::ostream&(std::ostream&)>
    ConnectionIntrospector::indent(int i) const {
        return boost::bind(&ConnectionIntrospector::printIndented, this, _1, i);
    }

    std::ostream& operator<<(
            std::ostream& os,
            const ConnectionIntrospector& descriptor) {
        return descriptor.indent(0)(os);
    }

}  // namespace internal
}  // namespace RTT
