#include "execution/StateGraph.hpp"
#include "execution/StateDescription.hpp"

#include "execution/CommandStopProgram.hpp"

#include "corelib/CommandNOP.hpp"
#include "corelib/ConditionFalse.hpp"
#include "corelib/ConditionTrue.hpp"
#include <utility>

#include <iostream>

namespace ORO_Execution
{
    using namespace boost;
    using namespace std;
    using ORO_CoreLib::CommandNOP;
    using ORO_CoreLib::ConditionTrue;

	StateGraph::StateGraph()
	{
        // The initial next node.
        next = add_vertex(graph);
	}

	StateGraph::~StateGraph()
	{
        // Clean up all conditions in the TransitionMap
        TransList::iterator v_it;
        TransitionMap::iterator m_it;
        for ( m_it= stateMap.begin(); m_it != stateMap.end(); ++m_it )
            for ( v_it = m_it->second.begin(); v_it!= m_it->second.end() ; ++v_it )
                delete get<0>(*v_it);
        // Clean up all conditions in the TransitionAnyMap
        TransitionAnyMap::iterator ma_it;
        for ( ma_it= stateAnyMap.begin(); ma_it != stateAnyMap.end(); ++ma_it )
                delete ma_it->second;

        // cleanup all created states.
        std::vector<StateDescription*>::iterator s_it;
        for ( s_it = state_list.begin(); s_it != state_list.end(); ++s_it )
            delete *s_it;
	}

	void StateGraph::process( Vertex n )
	{
        // The process function is called by the StateDescription.
        // It is meant to handle one action (eg onEntry, handle, onExit).
        // This function processes a whole graph in once.
        // It returns when no out_edges are found on a
        // node.
        graph_traits<Graph>::out_edge_iterator ei, ei_end;

        for ( tie(ei, ei_end) = boost::out_edges( n, graph ); ei != ei_end; ++ei)
            emap[*ei].reset();

        cmap[n].startExecution();
        // execute the current command.
        cmap[n].execute();

        tie(ei, ei_end) = boost::out_edges( n, graph );
        // We must find a way to sort the edges in the same order
        // as they are added to the graph.
        while ( ei != ei_end )
            {
                if ( emap[*ei].evaluate() ) // on Success :
                    {
                        // select the new node + init it + execute it
                        n = boost::target(*ei, graph);
                        cmap[n].startExecution();
                        cmap[n].execute();
                        // reset all its edges for evaluation
                        for ( tie(ei, ei_end) = boost::out_edges( n, graph ); ei != ei_end; ++ei)
                            emap[*ei].reset();
                        // set ei and ei_end again correctly
                        tie(ei, ei_end) = boost::out_edges( n, graph );
                    }
                else
                    ++ei;
            }
	}

	void StateGraph::reset()
	{
        cmap = get(vertex_command, graph);
        emap = get(edge_condition, graph);
	}

    const StateGraph::Graph& StateGraph::getGraph() const
    {
        return graph;
    }

    void StateGraph::selectEntryNode()
    {
        current = mstate->entryNode();
    }

    void StateGraph::selectExitNode()
    {
        current = mstate->exitNode();
    }

    void StateGraph::selectHandleNode()
    {
        current = mstate->handleNode();
    }

    StateGraph::CommandNode StateGraph::addCommand( ConditionInterface* cond,  CommandInterface* com )
    {
        add_edge(current, next, EdgeCondition(cond), graph);
        current = next;
        setCommand(com);
        next    = add_vertex( graph );
        return next;
    }

    void StateGraph::addConditionEdge( ConditionInterface* cond, CommandNode vert )
    {
        add_edge(current, vert, EdgeCondition(cond), graph);
    }
        
    void StateGraph::closeConditionEdge( CommandNode vert, ConditionInterface* cond )
    {
        add_edge(vert, current, EdgeCondition(cond), graph);
    }

    StateGraph::CommandNode StateGraph::moveTo( CommandNode _current, CommandNode _next )
    {
        CommandNode old = current;
        current = _current;
        next    = _next;
        return old;
    }

    void StateGraph::setCommand( CommandInterface* comm )
    {
        this->setCommand(current, comm);
    }

    void StateGraph::setCommand(CommandNode cn, CommandInterface* comm )
    {
        // the map contains _references_ to all vertex_command properties
        boost::property_map<Graph, vertex_command_t>::type 
            cmap = get(vertex_command, graph);
        // access the one of current
        delete cmap[cn].setCommand( comm );
    }

    StateGraph::CommandNode StateGraph::proceedToNext( ConditionInterface* cond )
    {
        add_edge(current, next, EdgeCondition(cond), graph);
        return proceedToNext();
    }

    StateGraph::CommandNode StateGraph::proceedToNext()
    {
        current = next;
        next    = add_vertex( graph );
        return current;
    }

    void StateGraph::connectToNext( CommandNode v, ConditionInterface* cond )
    {
        add_edge( v, next, EdgeCondition(cond), graph);
    }

    StateDescription* StateGraph::newState()
    {
        state_list.push_back(new StateDescription( add_vertex( graph ), add_vertex( graph ), add_vertex( graph ), this ) );
        return state_list.back();
    }
    
    void StateGraph::startState(StateDescription* sd)
    {
        sd->init();
        mstate = sd;
    }
    
    void StateGraph::endState( ) {
    }

    StateGraph::CommandNode StateGraph::currentNode() const
    {
        return current;
    }

    size_t StateGraph::currentEdges() const
    {
        return out_degree( current, graph );
    }

    StateGraph::CommandNode StateGraph::nextNode() const
    {
        return next;
    }

}

