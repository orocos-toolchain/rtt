/***************************************************************************
  tag: Intermodalics BVBA  Sat Jan 27 00:13:45 CET 2018  ConnectionIntrospector.hpp

                        ConnectionIntrospector.hpp -  description
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


#ifndef ORO_CONNECTION_INTROSPECTOR_HPP
#define ORO_CONNECTION_INTROSPECTOR_HPP

#include "../rtt-config.h"
#include "../rtt-fwd.hpp"
#include "../base/ChannelElementBase.hpp"
#include "ConnectionManager.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <list>
#include <set>

namespace RTT
{

namespace internal
{

class RTT_API ConnectionIntrospector
{
public:
    ConnectionIntrospector(const base::PortInterface* port);
    ConnectionIntrospector(const DataFlowInterface* dfi);
    ConnectionIntrospector(const TaskContext* tc);

    void add(const base::PortInterface*);
    void add(const DataFlowInterface*);
    void add(const TaskContext*);

    void reset();
    void createGraph(int depth = 1);

    std::ostream& operator>>(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os,
                                    const ConnectionIntrospector&);

private:
    class Node;
    typedef boost::shared_ptr<Node> NodePtr;
    typedef boost::weak_ptr<Node> NodeWPtr;
    typedef std::list<NodePtr> Nodes;

    class Connection;
    typedef boost::shared_ptr<Connection> ConnectionPtr;
    typedef std::list<ConnectionPtr> Connections;
    typedef std::set<const Connection *> ConnectionSet;

    class Node {
    public:
        virtual ~Node();

        virtual std::string getName() const;
        virtual const base::PortInterface *getPort() const;
        virtual base::ChannelElementBase::shared_ptr getEndpoint() const;

        typedef enum { Unspecified, Input, Output } Direction;
        Direction getDirection() const { return direction_; }

        virtual bool isPort() const { return false; }
        virtual bool isRemote() const { return false; }
        virtual bool isSharedConnection() const { return false; }

        virtual bool operator==(const Node& other) const;
        virtual bool operator<(const Node& other) const;

        typedef Connections::const_iterator const_iterator;
        const_iterator begin() const;
        const_iterator end() const;

    protected:
        Node(const std::string &name = std::string());
        Direction direction_;

    private:
        std::string getConnectionSummary() const;
        std::ostream& printIndented(std::ostream& os, int indent_lvl, const ConnectionPtr &incoming_connection, ConnectionSet &printed_connections) const;

        friend class ConnectionIntrospector;
        std::string name_;
        Connections connections_;
    };
    friend class Node;

    class PortNode : public Node {
    public:
        const base::PortInterface *getPort() const { return port_; }
        virtual bool isPort() const { return true; }
        virtual bool isRemote() const;

    protected:
        PortNode(const base::PortInterface* port);

    private:
        friend class ConnectionIntrospector;
        const base::PortInterface* port_;
    };

    class RemoteNode : public Node {
    public:
        virtual base::ChannelElementBase::shared_ptr getEndpoint() const { return endpoint_; }
        virtual bool isRemote() const { return true; }

    protected:
        RemoteNode(const base::ChannelElementBase::shared_ptr &endpoint,
                   Node::Direction direction,
                   const std::string &name);

    private:
        friend class ConnectionIntrospector;
        base::ChannelElementBase::shared_ptr endpoint_;
    };

    class SharedConnectionNode : public Node {
    public:
        virtual base::ChannelElementBase::shared_ptr getEndpoint() const { return shared_connection_; }
        virtual bool isSharedConnection() const { return true; }

        const SharedConnectionBase::shared_ptr &getSharedConnection() const { return shared_connection_; }

    protected:
        SharedConnectionNode(const internal::SharedConnectionBase::shared_ptr &shared_connection);

    private:
        friend class ConnectionIntrospector;
        SharedConnectionBase::shared_ptr shared_connection_;
    };

    struct Connection {
    public:
        virtual ~Connection();

        NodePtr from() const { return from_.lock(); }
        NodePtr to() const { return to_.lock(); }
        NodePtr getOutput() const;
        NodePtr getInput() const;

        bool isForward() const;

        const ConnPolicy &getConnPolicy() const { return policy_; }

    protected:
        Connection(const NodePtr &from, const NodePtr &to,
                   const ConnPolicy &policy);

    private:
        friend class ConnectionIntrospector;
        NodeWPtr from_;
        NodeWPtr to_;
        ConnPolicy policy_;
        bool is_forward_;
    };

    void collectStartNodes(const DataFlowInterface *);
    void createGraphInternal(int remaining_depth, const Nodes& to_visit);

    bool findOrCreateNeighbor(
            const NodePtr &node,
            const base::ChannelElementBase::shared_ptr &endpoint,
            Node::Direction direction,
            const std::string &name, const ConnPolicy &policy,
            NodePtr &other, ConnectionPtr &connection);
    NodePtr findNode(const Node &node) const;
    static ConnectionPtr findConnectionTo(const Connections &connections, const Node &node);

    Nodes start_nodes_;
    Nodes nodes_;
};

}  // namespace internal

}  // namespace RTT

#endif // ORO_CONNECTION_INTROSPECTOR_HPP
