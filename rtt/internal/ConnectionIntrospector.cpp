/***************************************************************************
  tag: Intermodalics BVBA  Sat Jan 27 00:13:48 CET 2018  ConnectionIntrospector.cpp

                        ConnectionIntrospector.cpp -  description
                           -------------------
    begin                : Sat January 27 2018
    copyright            : (C) 2018 Intermodalics BVBA
    email                : info@intermodalics.eu

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#include "ConnectionIntrospector.hpp"
#include "SharedConnection.hpp"

#include "../Port.hpp"
#include "../TaskContext.hpp"

#include <boost/lexical_cast.hpp>
#include <cassert>

namespace RTT {

namespace internal {

ConnectionIntrospector::Node::Node(const std::string &name)
    : direction_(Unspecified), name_(name)
{
}

ConnectionIntrospector::Node::~Node()
{
}

inline const base::PortInterface *ConnectionIntrospector::Node::getPort() const
{
    return 0;
}

inline base::ChannelElementBase::shared_ptr ConnectionIntrospector::Node::getEndpoint() const
{
    if (getPort()) {
        return getPort()->getEndpoint();
    }
    return 0;
}

inline std::string ConnectionIntrospector::Node::getName() const
{
    return !name_.empty() ? name_ : "(unknown)";
}

inline bool ConnectionIntrospector::Node::operator==(const Node& other) const
{
    return (this->getEndpoint() == other.getEndpoint());
}

inline bool ConnectionIntrospector::Node::operator<(const Node& other) const
{
    return (this->getEndpoint() < other.getEndpoint());
}

inline ConnectionIntrospector::Node::const_iterator ConnectionIntrospector::Node::begin() const
{
    return connections_.begin();
}

inline ConnectionIntrospector::Node::const_iterator ConnectionIntrospector::Node::end() const
{
    return connections_.end();
}

ConnectionIntrospector::PortNode::PortNode(const base::PortInterface* port)
    : Node(port->getQualifiedName()), port_(port)
{
    const RTT::base::InputPortInterface *input = dynamic_cast<const RTT::base::InputPortInterface *>(port);
    const RTT::base::OutputPortInterface *output = dynamic_cast<const RTT::base::OutputPortInterface *>(port);
    if (input) {
        direction_ = Input;
    } else if (output) {
        direction_ = Output;
    }
}

inline bool ConnectionIntrospector::PortNode::isRemote() const
{
    return !(port_->isLocal());
}

ConnectionIntrospector::RemoteNode::RemoteNode(const base::ChannelElementBase::shared_ptr &endpoint,
                                               Node::Direction direction,
                                               const std::string &name)
    : Node(name), endpoint_(endpoint)
{
    direction_ = direction;
}

ConnectionIntrospector::SharedConnectionNode::SharedConnectionNode(const internal::SharedConnectionBase::shared_ptr &shared_connection)
    : Node(shared_connection->getName()), shared_connection_(shared_connection)
{
}

ConnectionIntrospector::Connection::Connection(const ConnectionIntrospector::NodePtr &from,
                                               const ConnectionIntrospector::NodePtr &to,
                                               const ConnPolicy &policy)
    : from_(from), to_(to), policy_(policy)
{
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

inline ConnectionIntrospector::NodePtr ConnectionIntrospector::Connection::getOutput() const {
    return is_forward_ ? from() : to();
}

inline ConnectionIntrospector::NodePtr ConnectionIntrospector::Connection::getInput() const {
    return is_forward_ ? to() : from();
}

inline bool ConnectionIntrospector::Connection::isForward() const {
    return is_forward_;
}

ConnectionIntrospector::ConnectionIntrospector(const base::PortInterface* port)
{
    add(port);
}

ConnectionIntrospector::ConnectionIntrospector(const DataFlowInterface* interface)
{
    add(interface);
}

ConnectionIntrospector::ConnectionIntrospector(const TaskContext* tc)
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
}

void ConnectionIntrospector::createGraph(int depth) {
    createGraphInternal(depth, start_nodes_);
}

void ConnectionIntrospector::createGraphInternal(int remaining_depth, const Nodes& to_visit) {
    if (remaining_depth <= 0) return;
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
                base::ChannelElementBase::shared_ptr endpoint;
                Node::Direction direction = Node::Unspecified;
                if (port_node->getDirection() == Node::Output) {
                    endpoint = descriptor.get<1>()->getOutputEndPoint();
                    direction = Node::Input;
                } else if (port_node->getDirection() == Node::Input) {
                    endpoint = descriptor.get<1>()->getInputEndPoint();
                    direction = Node::Output;
                } else {
                    endpoint = descriptor.get<1>();
                }

                NodePtr other;
                ConnectionPtr connection;
                bool found = findOrCreateNeighbor(node, endpoint, direction,
                                                  descriptor.get<0>()->getName(),
                                                  descriptor.get<2>(),
                                                  other, connection);

                if (!found) { // new node
                    if (other->isSharedConnection()) {
                        // immediately expand all connected nodes without decrementing remaining_depth
                        // (traversing a shared connection counts as a single hop)
                        createGraphInternal(remaining_depth, Nodes(1, other));
                    } else if (remaining_depth > 1) {
                        next_visit.push_back(other);
                    }
                }
            }
        }

        SharedConnectionNode *shared_connection_node = dynamic_cast<SharedConnectionNode *>(node.get());
        if (shared_connection_node) {
            const internal::SharedConnectionBase::shared_ptr &shared_connection
                = shared_connection_node->getSharedConnection();

            base::MultipleOutputsChannelElementBase::Outputs outputs = shared_connection->getOutputs();
            for(base::MultipleOutputsChannelElementBase::Outputs::const_iterator output = outputs.begin();
                output != outputs.end(); ++output) {
                base::ChannelElementBase::shared_ptr endpoint = output->channel->getOutputEndPoint();
                NodePtr other;
                ConnectionPtr connection;
                bool found = findOrCreateNeighbor(node, endpoint,  Node::Output,
                                                  std::string(),
                                                  *shared_connection->getConnPolicy(),
                                                  other, connection);
                if (!found && remaining_depth > 1) {
                    next_visit.push_back(other);
                }
            }

            base::MultipleInputsChannelElementBase::Inputs inputs = shared_connection->getInputs();
            for(base::MultipleInputsChannelElementBase::Inputs::const_iterator input = inputs.begin();
                input != inputs.end(); ++input) {
                base::ChannelElementBase::shared_ptr endpoint = (*input)->getInputEndPoint();
                NodePtr other;
                ConnectionPtr connection;
                bool found = findOrCreateNeighbor(node, endpoint,  Node::Input,
                                                  std::string(),
                                                  *shared_connection->getConnPolicy(),
                                                  other, connection);
                if (!found && remaining_depth > 1) {
                    next_visit.push_back(other);
                }
            }
        }
    }

    if (!next_visit.empty()) {
        createGraphInternal(remaining_depth - 1, next_visit);
    }
}

bool ConnectionIntrospector::findOrCreateNeighbor(
        const NodePtr &node,
        const base::ChannelElementBase::shared_ptr &endpoint,
        ConnectionIntrospector::Node::Direction direction,
        const std::string &name, const ConnPolicy &policy,
        NodePtr &other, ConnectionPtr &connection) {
    // a port?
    const base::PortInterface *port = endpoint->getPort();
    if (port) {
        other.reset( new PortNode(port) );
    } else {
        // a shared connection?
        internal::SharedConnectionBase::shared_ptr shared_connection
            = dynamic_cast<RTT::internal::SharedConnectionBase *>(endpoint.get());
        if (shared_connection) {
            other.reset( new SharedConnectionNode(shared_connection) );
        } else {
            // or a remote connection?
            other.reset( new RemoteNode(endpoint, direction, name) );
        }
    }

    // Check if the node is already known
    NodePtr found = findNode(*other);
    if (found) {
        // replace node pointer with found node
        other = found;

        // try to find matching connection from other to node
        connection = findConnectionTo(other->connections_, *node);
    } else {
        nodes_.push_back(other);
    }

    // Create new connection if none has been found
    if (!connection) {
        connection.reset(new Connection(node, other, policy));
    }
    node->connections_.push_back(connection);

    return found;
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

std::string ConnectionIntrospector::Node::getConnectionSummary() const {
    const std::size_t connection_nr = this->connections_.size();
    switch (connection_nr) {
    case 0:
        return " has no connections.";
    case 1:
        return " has a single connection:";
    default:
        return " has multiple connections (#"
               + boost::lexical_cast<std::string>(connection_nr) + "):";
    }
}

std::ostream& ConnectionIntrospector::Node::printIndented(
        std::ostream& os, int indent_lvl,
        const ConnectionPtr &incoming_connection,
        ConnectionSet &printed_connections) const {

    // Never traverse the same connections twice
    if (printed_connections.find(incoming_connection.get()) != printed_connections.end()) {
        return os;
    } else if (incoming_connection.get()){
        printed_connections.insert(incoming_connection.get());
    }

    const std::string indent(2 * indent_lvl, ' ');
    os << indent;
    if (incoming_connection) {
        if (( incoming_connection->isForward() && (incoming_connection->to().get() == this)) ||
            (!incoming_connection->isForward() && (incoming_connection->from().get() == this))) {
            os << "--> ";
        } else {
            os << "<-- ";
        }
    }
    switch(getDirection()) {
    case Input:
        os << " In ";
        break;
    case Output:
        os << "Out ";
        break;
    default:
        break;
    }
    if (isRemote()) {
        os << "[REMOTE " << this->getEndpoint()->getElementName() << "] ";
    }
    if (isSharedConnection()) {
        os << "[SHARED] ";
    }
    os << this->getName();
    if (indent_lvl == 0) {
        os << this->getConnectionSummary();
    }
    if (incoming_connection) {
        os << " [" << incoming_connection->getConnPolicy() << "]";
    }
    os << std::endl;

    for (Connections::const_iterator conn = this->connections_.begin();
         conn != this->connections_.end(); ++conn) {
        if (*((*conn)->to()) == *this) {
            (*conn)->from()->printIndented(os, indent_lvl + 1, *conn,
                                           printed_connections);
        } else {
            (*conn)->to()->printIndented(os, indent_lvl + 1, *conn,
                                         printed_connections);
        }
    }

    return os;
}

std::ostream& ConnectionIntrospector::operator>>(
        std::ostream& os) const {

    for (Nodes::const_iterator node = start_nodes_.begin();
         node != start_nodes_.end(); ++node) {
        ConnectionSet printed_connections;
        (*node)->printIndented(
                    os, 0, ConnectionPtr(),
                    printed_connections) << "\n";
    }

    return os;
}

std::ostream& operator<<(
        std::ostream& os,
        const ConnectionIntrospector& introspector) {
    return introspector >> os;
}

}  // namespace internal
}  // namespace RTT
