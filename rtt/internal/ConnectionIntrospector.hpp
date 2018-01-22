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

    class Node {
    public:
        virtual ~Node();

        base::ChannelElementBase::shared_ptr getEndpoint() const;
        virtual std::string getName() const;

        typedef enum { Unspecified, Input, Output, Both } Direction;
        Direction getDirection() const { return direction_; }

        virtual bool isPort() const { return false; }
        virtual bool isRemote() const { return true; }

        virtual bool operator==(const Node& other) const;
        virtual bool operator<(const Node& other) const;

        typedef Connections::const_iterator const_iterator;
        const_iterator begin() const;
        const_iterator end() const;

    protected:
        Node(base::ChannelElementBase *endpoint, const std::string &name = std::string());
        Direction direction_;

    private:
        friend class ConnectionIntrospector;
        base::ChannelElementBase::shared_ptr endpoint_;
        std::string name_;
        Connections connections_;
    };
    friend class Node;

    class PortNode : public Node {
    public:
        const base::PortInterface *getPort() const;
        std::string getPortType() const;

        virtual bool isPort() const { return true; }
        virtual bool isRemote() const;

    protected:
        PortNode(const base::PortInterface* port);

    private:
        friend class ConnectionIntrospector;
        const base::PortInterface* port_;
    };

    struct Connection {
    public:
        virtual ~Connection();

        NodePtr from() const;
        NodePtr to() const;
        NodePtr getOutput() const;
        NodePtr getInput() const;

        bool isForward() const;

    protected:
        Connection(const NodePtr &from, const NodePtr &to,
                   const ConnectionManager::ChannelDescriptor &from_descriptor);

    private:
        friend class ConnectionIntrospector;
        typedef std::pair<NodeWPtr, ConnectionManager::ChannelDescriptor> EndpointDescriptor;
        EndpointDescriptor from_;
        EndpointDescriptor to_;
        bool is_forward_;
    };

    void collectStartNodes(const DataFlowInterface *);
    void createGraphInternal(int remaining_depth, const Nodes& to_visit);
    NodePtr findNode(const Node &node) const;
    static ConnectionPtr findConnectionTo(const Connections &connections, const Node &node);

    Nodes start_nodes_;
    Nodes nodes_;
    int depth_;
};

}  // namespace internal

}  // namespace RTT

#endif // ORO_CONNECTION_INTROSPECTOR_HPP
