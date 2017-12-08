#include "ConnectionIntrospector.hpp"

namespace RTT {
namespace internal {

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
            const base::PortInterface* port) {
        PortQualifier port_(port);
        if (port_.is_forward) {
            out_port = port_;
        } else {
            in_port = port_;
        }
        // This is the fake "incoming" port connection, so forward is the
        // opposite of the port value.
        is_forward = !port_.is_forward;
        depth = 0;
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
        to_visit.push_back(this);
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

            // Would be undefined for remote ports.
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
                port.port_name = node.connection_id->portName();
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
        // For the first level (entry-point), only log the port.
        if (this->depth == 0) {
            os << std::string(currIndent, ' ')
               << (this->is_forward ? " IN " : " OUT ")
               << (this->is_forward ? this->in_port
                                         : this->out_port) << ":\n";
        } else {
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
