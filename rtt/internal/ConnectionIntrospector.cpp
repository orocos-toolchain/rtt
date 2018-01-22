#include "ConnectionIntrospector.hpp"

#include "../Port.hpp"
#include "../TaskContext.hpp"

#include <cassert>

namespace RTT {

// forward declaration of corba::RemoteConnID
namespace corba {
   class RemoteConnID;
}

namespace internal {

ConnectionIntrospector::Node::Node(base::ChannelElementBase *endpoint, const std::string &name)
    : direction_(Unspecified), endpoint_(endpoint), name_(name) {}

ConnectionIntrospector::Node::~Node() {}

inline base::ChannelElementBase::shared_ptr ConnectionIntrospector::Node::getEndpoint() const {
    return endpoint_;
}

inline std::string ConnectionIntrospector::Node::getName() const {
    return !name_.empty() ? name_ : "(unnamed)";
}

inline bool ConnectionIntrospector::Node::operator==(const Node& other) const {
    return (endpoint_ == other.endpoint_);
}

inline bool ConnectionIntrospector::Node::operator<(const Node& other) const {
    return (endpoint_ < other.endpoint_);
}

inline ConnectionIntrospector::Node::const_iterator ConnectionIntrospector::Node::begin() const {
    return connections_.begin();
}

inline ConnectionIntrospector::Node::const_iterator ConnectionIntrospector::Node::end() const {
    return connections_.end();
}

ConnectionIntrospector::PortNode::PortNode(const base::PortInterface* port)
    : Node(port->getEndpoint(), port->getQualifiedName()), port_(port) {
    const RTT::base::InputPortInterface *input = dynamic_cast<const RTT::base::InputPortInterface *>(port);
    const RTT::base::OutputPortInterface *output = dynamic_cast<const RTT::base::OutputPortInterface *>(port);
    if (input) {
        direction_ = Input;
    } else if (output) {
        direction_ = Output;
    }
}

inline const base::PortInterface *ConnectionIntrospector::PortNode::getPort() const {
    return port_;
}

std::string ConnectionIntrospector::PortNode::getPortType() const {
    switch(getDirection()) {
        case Input:  return "In";
        case Output: return "Out";
        default:     return "";
    }
}

inline bool ConnectionIntrospector::PortNode::isRemote() const {
    return !(port_->isLocal());
}

ConnectionIntrospector::Connection::Connection(const ConnectionIntrospector::NodePtr &from,
                                               const ConnectionIntrospector::NodePtr &to,
                                               const ConnectionManager::ChannelDescriptor &from_descriptor)
    : from_(from, from_descriptor), to_(to, ConnectionManager::ChannelDescriptor()) {
    if (from->getDirection() == Node::Input) {
        assert(to->getDirection() != Node::Input);
        is_forward_ = false;
    } else if (from->getDirection() == Node::Output) {
        assert(to->getDirection() != Node::Output);
        is_forward_ = true;
    } else if (to->getDirection() == Node::Input) {
        is_forward_ = true;
    } else if (to->getDirection() == Node::Output) {
        is_forward_ = false;
    } else {
        assert(false && "Direction of data flow is not well-defined");
        is_forward_ = true;
    }
}

ConnectionIntrospector::Connection::~Connection() {}

inline ConnectionIntrospector::NodePtr ConnectionIntrospector::Connection::from() const {
    return from_.first.lock();
}

inline ConnectionIntrospector::NodePtr ConnectionIntrospector::Connection::to() const {
    return to_.first.lock();
}

inline ConnectionIntrospector::NodePtr ConnectionIntrospector::Connection::getOutput() const {
    return is_forward_ ? from() : to();
}

inline ConnectionIntrospector::NodePtr ConnectionIntrospector::Connection::getInput() const {
    return is_forward_ ? to() : to();
}

inline bool ConnectionIntrospector::Connection::isForward() const {
    return is_forward_;
}

ConnectionIntrospector::ConnectionIntrospector(const base::PortInterface* port)
    : depth_(0)
{
    add(port);
}

ConnectionIntrospector::ConnectionIntrospector(const DataFlowInterface* interface)
    : depth_(0)
{
    add(interface);
}

ConnectionIntrospector::ConnectionIntrospector(const TaskContext* tc)
    : depth_(0)
{
    add(tc);
}

void ConnectionIntrospector::add(const base::PortInterface* port)
{
    NodePtr node(new PortNode(port));
    start_nodes_.push_back(NodePtr(new PortNode(port)));
}

void ConnectionIntrospector::add(const DataFlowInterface *interface) {
    DataFlowInterface::PortNames ports = interface->getPortNames();
    for(DataFlowInterface::PortNames::const_iterator it = ports.begin();
        it != ports.end(); ++it) {
        NodePtr node(new PortNode(interface->getPort(*it)));
        start_nodes_.push_back(node);
    }
}

void ConnectionIntrospector::add(const TaskContext* tc) {
    return add(tc->ports());
}

void ConnectionIntrospector::reset() {
    nodes_.clear();
    for(Nodes::iterator it = nodes_.begin(); it != nodes_.end(); ++it) {
        (*it)->connections_.clear();
    }
    depth_ = 0;
}

void ConnectionIntrospector::createGraph(int depth) {
    createGraphInternal(depth, start_nodes_);
}

void ConnectionIntrospector::createGraphInternal(int remaining_depth, const Nodes& to_visit) {
    if (remaining_depth < 1) remaining_depth = 1;
    --remaining_depth;
    Nodes next_visit;

    for(Nodes::const_iterator node_it = to_visit.begin(); node_it != to_visit.end(); ++node_it) {
        const NodePtr &node = *node_it;

        // If the node represents a port, iterate over all connections:
        PortNode *port_node = dynamic_cast<PortNode *>(node.get());
        if (port_node) {
            ConnectionManager::Connections connections = port_node->getPort()->getManager()->getConnections();
            for(ConnectionManager::Connections::const_iterator con_it = connections.begin();
                con_it != connections.end(); ++con_it) {
                const ConnectionManager::ChannelDescriptor &descriptor = *con_it;
//                const ConnID *conn_id = descriptor.get<0>().get();
//                const base::PortInterface *other_port = conn_id->getPort();
                const base::PortInterface *other_port = descriptor.get<1>()->getPort();
                NodePtr other;
                if (other_port) {
                    other.reset( new PortNode(other_port) );
                } else {
                    other.reset( new Node(descriptor.get<1>().get()) );
                }

                // Check if the other node is already known
                ConnectionPtr connection;
                NodePtr found = findNode(*other);
                if (found) {
                    // replace other pointer with found node
                    other = found;

                    // try to find matching connection in other where to == node
                    connection = findConnectionTo(other->connections_, *node);
                    if (connection) {
                        // complete descriptor (to part should be empty)
                        assert(connection->to_.second.get<0>() == 0);
                        assert(connection->to_.second.get<1>() == 0);
                        assert(connection->from_.second.get<1>() == node->getEndpoint());
                        connection->to_.second = descriptor;
                    }

                } else {
                    nodes_.push_back(other);
                    if (remaining_depth > 0) {
                        next_visit.push_back(other);
                    }
                }

                // Create new connection if none has been found
                if (!connection.get()) {
                    connection.reset(new Connection(node, other, descriptor));
                }
                node->connections_.push_back(connection);
            }
        }
    }

    if (!next_visit.empty()) {
        createGraphInternal(remaining_depth, next_visit);
    }
}

ConnectionIntrospector::NodePtr ConnectionIntrospector::findNode(const Node &node) const {
    Nodes::const_iterator it;
    for(it = start_nodes_.begin(); it != start_nodes_.end(); ++it) {
        if (**it == node) return *it;
    }
    for(it = nodes_.begin(); it != nodes_.end(); ++it) {
        if (**it == node) return *it;
    }
    return NodePtr();
}

ConnectionIntrospector::ConnectionPtr ConnectionIntrospector::findConnectionTo(const Connections &connections, const Node &node) {
    Connections::const_iterator it;
    for(it = connections.begin(); it != connections.end(); ++it) {
        if (*((*it)->to()) == node) return *it;
    }
    return ConnectionPtr();
}

//static std::ostream& ConnectionIntrospector::printIndented(std::ostream& os,
//        int i) const {
//    const int currIndent = 4 * this->depth + i;
//    if (this->depth == -1) {
//        // For depth -1, only log the component name.
//        os << std::string(i, ' ') << this->in_port.owner_name
//           << " COMPONENT\n";
//    } else if (this->depth == 0) {
//        // For depth 0, only log the port.
//        std::string connection_summary;
//        const int connection_nr = this->sub_connections.size();
//        switch (connection_nr) {
//        case 0:
//            connection_summary = "No";
//            break;
//        case 1:
//            connection_summary = "Single";
//            break;
//        default:
//            connection_summary = "Multiple";
//            break;
//        }
//        os << std::string(currIndent, ' ')
//           << (this->is_forward ? " IN " : " OUT ")
//           << (this->is_forward ? this->in_port
//                                     : this->out_port)
//           << " with " << connection_summary << " connection(s) (#"
//           << connection_nr << ")" << ":\n";
//    } else if (this->depth > 0) {
//        // Positive depth, log the full connection information.
//        os << std::string(currIndent, ' ')
//           << (this->is_forward ? " -> IN " : " <- OUT ")
//           << ((this->is_forward ? this->in_port
//                                     : this->out_port).is_remote ?
//                  "(remote) " : "")
//           << (this->is_forward ? this->in_port
//                                     : this->out_port)
//           << " : (" << this->connection_policy << ")\n";
//    }
//    for (std::list< ConnectionIntrospector >::const_iterator
//            it = this->sub_connections.begin();
//         it != this->sub_connections.end(); ++it) {
//        os << *it;
//    }
//    return os;
//}

//boost::function<std::ostream&(std::ostream&)>
//ConnectionIntrospector::indent(int i) const {
//    return boost::bind(&ConnectionIntrospector::printIndented, this, _1, i);
//}

std::ostream& operator<<(
        std::ostream& os,
        const ConnectionIntrospector& descriptor) {
//    return descriptor.indent(0)(os);
//    return printTo(descriptor.start_nodes_, os);
    return os;
}

}  // namespace internal
}  // namespace RTT
