#ifndef STATEGRAPH_HPP
#define STATEGRAPH_HPP

#include <corelib/StateInterface.hpp>
#include <corelib/StateContext.hpp>
#include "ProcessorControlInterface.hpp"

#include "VertexNode.hpp"
#include "EdgeCondition.hpp"

#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <vector>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp> // the type of our graph


namespace ORO_Execution
{
    using ORO_CoreLib::StateInterface;
    using boost::adjacency_list;
    using boost::property;
    using boost::graph_traits;

    class StateDescription;

	/**
	 * @brief This class represents a finite state machine
	 * in a state graph tree, based on the
     * Boost Graph Library.
     *
     * The StateContext base class is used at run-time
     * for doing the state transitions. This subclass 
     * implements the construction of states and transitions
     * and it is mostly accessed by the StateGraphParser.
	 */
	class StateGraph
        : public ORO_CoreLib::StateContext
	{
    public:

        typedef EdgeCondition::EdgeProperty EdgeProperty;
        typedef VertexNode::VertProperty    VertProperty;

        typedef adjacency_list<boost::vecS, boost::listS, boost::directedS, VertProperty, EdgeProperty> Graph;
        typedef graph_traits<Graph>::vertex_descriptor Vertex;
        typedef graph_traits<Graph>::edge_descriptor Edge;

        boost::property_map<Graph, vertex_command_t>::type  cmap;
        boost::property_map<Graph, edge_condition_t>::type  emap;

        /**
         * A CommandNode serves as a token to construct
         * a vertex or node, containing a command.
         */
        typedef Vertex CommandNode ;

        /**
         * A ConditionEdge serves as a token to construct
         * an edge, containing a condition.
         */
        typedef Edge ConditionEdge ;

        /**
         * Constructs an empty state diagram.
         *
         */
        StateGraph();

        virtual ~StateGraph();

        /**
         * Returns the Graph of the state.
         */
        const Graph& getGraph() const;

        /**
         * Runs through the graph from n until
         * an Edge-less node.
         */
        void process( Vertex n );

        /**
         * Reset the internal state before
         * starting execution. This must be done before
         * the StateGraph is used (process'ed) the first time.
         */
        void reset();

        /**
         * Create a new, empty, state.
         */
        StateDescription* newState();

        /**
         * Start filling up (defining) a given state.
         */
        void startState( StateDescription* state );

        /**
         * End the State definition.
         */
        void endState();

        /**
         * Select the entry node for adding commands.
         */
        void selectEntryNode();
        
        /**
         * Select the exit node for adding commands.
         */
        void selectExitNode();
        
        /**
         * Select the handle node for adding commands.
         */
        void selectHandleNode();

        /**
         * Add a new command from the current CommandNode under a condition.
         *
         * @return the 'new' current CommandNode.
         */
        CommandNode addCommand( ConditionInterface* cond,  CommandInterface* com );

        /**
         * Add an edge from the current CommandNode to the given CommandNode
         * without changing the current CommandNode.
         *
         */
        void addConditionEdge( ConditionInterface* cond, CommandNode vert );

        /**
         * Add an edge between the given CommandNode and the current CommandNode.
         *
         */
        void closeConditionEdge( CommandNode vert, ConditionInterface* cond );

        /**
         * Select an already added CommandNode.
         *
         * @return the previous CommandNode.
         */
        CommandNode moveTo( CommandNode _current, CommandNode _next );

        /**
         * Sets a new Command on the current CommandNode and
         * returns the previous one.
         *
         */
        void setCommand( CommandInterface* comm );

        /** 
         * Sets a (new) command on a given CommandNode.
         * 
         * @param vert The CommandNode to be adapted
         * @param comm The new Command to be executed in that node.
         * 
         */
        void setCommand( CommandNode vert, CommandInterface* comm);

        /**
         * Proceed to the 'next' CommandNode.
         *
         * @return The new current CommandNode.
         */
        CommandNode proceedToNext();

        /**
         * Proceed to the 'next' CommandNode and add an edge
         * with a condition.
         *
         * @return The new current CommandNode.
         */
        CommandNode proceedToNext( ConditionInterface* cond );

        /**
         * Connect the given CommandNode to the 'next' CommandNode.
         */
        void connectToNext( CommandNode v, ConditionInterface* cond );

        /**
         * Return the current CommandNode.
         */
        CommandNode currentNode() const;

        /**
         * Return the number of edges of the current CommandNode.
         */
        size_t currentEdges() const;

        /**
         * Return the next CommandNode.
         */
        CommandNode nextNode() const;

    private:
        /**
         * The graph containing all the state nodes.
         */
        Graph graph;
        
        StateDescription* mstate;

        /**
         * The node which is built now
         */
        CommandNode current;

        /**
         * The node which will be built next.
         */
        CommandNode next;

        /**
         * The node that was run before this one.
         */
        CommandNode previous;

        std::vector<StateDescription*> state_list;
	};


}

#endif


