/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:36 CEST 2004  ProgramGraph.cxx 

                        ProgramGraph.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
#include "execution/ProgramGraph.hpp"
#include "execution/FunctionGraph.hpp"

#include "execution/CommandStopProgram.hpp"

#include "corelib/CommandNOP.hpp"
#include "corelib/ConditionFalse.hpp"
#include "corelib/ConditionTrue.hpp"
#include <boost/graph/copy.hpp>
#include <utility>

#include <iostream>

namespace ORO_Execution
{
    using namespace boost;
    using namespace std;
    using ORO_CoreLib::CommandNOP;
    using ORO_CoreLib::ConditionTrue;

	ProgramGraph::ProgramGraph(const std::string& _name)
        : myName(_name)
	{
	}

	ProgramGraph::~ProgramGraph()
	{
	}

	void ProgramGraph::execute()
	{ 
        graph_traits<Graph>::out_edge_iterator ei, ei_end;
        // the map contains _references_ to all vertex_command properties
        boost::property_map<Graph, vertex_command_t>::type 
            cmap = get(vertex_command, program);
        boost::property_map<Graph, edge_condition_t>::type 
            emap = get(edge_condition, program);

        // initialise current node if needed and reset all its out_edges
        if ( previous != current )
            {
                for ( tie(ei, ei_end) = boost::out_edges( current, program ); ei != ei_end; ++ei)
                    emap[*ei].reset();
                cmap[current].startExecution();
            }
                
        previous = current;

        // execute the current command.
        cmap[current].execute();

        // Branch selecting Logic :
        for ( tie(ei, ei_end) = boost::out_edges( current, program ); ei != ei_end; ++ei)
            {
            if ( emap[*ei].evaluate() )
                {
                    current = boost::target(*ei, program);
                    // a new node has been found ...
                    // it will be executed in the next step.
                    return;
                }
            }
	}

	void ProgramGraph::reset()
	{
		current = root;
		previous = end;
	}

    const ProgramGraph::Graph& ProgramGraph::getGraph() const
    {
        return program;
    }
    
    FunctionGraph* ProgramGraph::startFunction()
    {
        // next node should be 'empty'/ not used here.

        // a function is to be constructed
        FunctionGraph* fn =  new FunctionGraph( );
        graph   = &fn->getGraph();
        current = fn->startNode();
        next    = add_vertex( *graph );
        put( vertex_exec, *graph, next, VertexNode::normal_node );
        return fn;
    }
    
    void ProgramGraph::returnFunction( ConditionInterface* cond, FunctionGraph* fn)
    {
        // connect the current node to the exitNode under a condition,
        // for example, the current implicit term condition.
        add_edge(current, fn->exitNode(), EdgeCondition(cond), *graph);
    }

    void ProgramGraph::endFunction( FunctionGraph* fn)
    {
        // A return statement is obligatory, so the returnFunction has
        // already connected the current node to exitNode of the function.
        // the end of the statement will proceedToNext
        // remove the empty next nodes.
        remove_vertex( current, *graph );
        remove_vertex( next, *graph );
        fn->finish();
    }

    ProgramGraph::CommandNode ProgramGraph::addCommand( ConditionInterface* cond,  CommandInterface* com )
    {
        add_edge(current, next, EdgeCondition(cond), *graph);
        current = next;
        setCommand(com);
        next    = add_vertex( *graph );
        put( vertex_exec, *graph, next, VertexNode::normal_node );
        return next;
    }

    void ProgramGraph::addConditionEdge( ConditionInterface* cond, CommandNode vert )
    {
        add_edge(current, vert, EdgeCondition(cond), *graph);
    }
        
    void ProgramGraph::closeConditionEdge( CommandNode vert, ConditionInterface* cond )
    {
        add_edge(vert, current, EdgeCondition(cond), *graph);
    }

    ProgramGraph::CommandNode ProgramGraph::moveTo( CommandNode _current, CommandNode _next )
    {
        CommandNode old = current;
        current = _current;
        next    = _next;
        return old;
    }

    void ProgramGraph::setCommand( CommandInterface* comm )
    {
        this->setCommand(current, comm);
    }

    void ProgramGraph::setCommand(CommandNode cn, CommandInterface* comm )
    {
        // the map contains _references_ to all vertex_command properties
        boost::property_map<Graph, vertex_command_t>::type 
            cmap = get(vertex_command, *graph);
        // access the one of current
        delete cmap[cn].setCommand( comm );
    }

    ProgramGraph::CommandNode ProgramGraph::proceedToNext( ConditionInterface* cond, int this_line )
    {
        add_edge(current, next, EdgeCondition(cond), *graph);
        return proceedToNext( this_line );
    }

    ProgramGraph::CommandNode ProgramGraph::proceedToNext( int this_line )
    {
        if ( this_line )
            this->setLineNumber( this_line );
        current = next;
        next    = add_vertex( *graph );
        put(vertex_exec, *graph, next, VertexNode::normal_node );
        return current;
    }

    void ProgramGraph::connectToNext( CommandNode v, ConditionInterface* cond )
    {
        add_edge( v, next, EdgeCondition(cond), *graph);
    }

    ProgramGraph::CommandNode ProgramGraph::startProgram()
    {
        // we work now on the program.
        graph = &program;
        // the current node
        current = add_vertex( *graph );
        put(vertex_exec, *graph, current, VertexNode::prog_start_node );
        next    = add_vertex( *graph );
        put(vertex_exec, *graph, next, VertexNode::normal_node );

        end  = add_vertex( *graph );
        root = current;

        return current;
    }

    const std::string& ProgramGraph::getName() const
    {
        return myName;
    }

    void ProgramGraph::setName(const std::string& _name)
    {
        myName = _name;
    }
    
    void ProgramGraph::returnProgram( ConditionInterface* cond )
    {
        add_edge(current, end, EdgeCondition(cond), program );
    }

    void ProgramGraph::endProgram( ProcessorInterface* pci ) {
        // End of all processing. return already linked to end.
        boost::property_map<Graph, vertex_command_t>::type 
            cmap = get(vertex_command, program);
        delete cmap[end].setCommand( new CommandStopProgram( pci, myName ) );
        put(vertex_exec, *graph, end, VertexNode::prog_exit_node );

        assert( current != next );
        assert( out_degree(current, program) == 0 );
        assert( out_degree(next, program) == 0 );
        clear_vertex(current, program);
        clear_vertex(next, program);
        remove_vertex( current, program );
        remove_vertex( next, program );
    }

    template< class _Map >
    struct finder {
        typedef ProgramGraph::Vertex first_argument_type ;
        typedef std::pair<_Map, int> second_argument_type ;
        typedef bool result_type;
        bool operator()(const ProgramGraph::Vertex& v,
                        const std::pair<_Map, int>& to_find  ) const
        {
            return to_find.first[v] == to_find.second;
        }
    };

    ProgramGraph::CommandNode ProgramGraph::appendFunction( ConditionInterface* cond, FunctionGraph* fn )
    {
        /**
         * This can be used if a function must be called conditionally.
         */
        // Do not move current inhere !
        // copy FunctionGraph's graph into the current *graph, add an edge with
        // the condition. Connect the exitNode() of the function with the next node.
        boost::copy_graph(fn->getGraph(), *graph);
        // the subgraph has been copied but is now 'floating' in the current graph.
        // we search func start and exit points and connect them to the current graph.
        graph_traits<Graph>::vertex_iterator v1,v2;
        tie(v1,v2) = vertices(*graph);
        boost::property_map<Graph, vertex_exec_t>::type 
            vmap = get(vertex_exec, *graph);

        Vertex funcStart=*
            find_if(v1, v2,
                    bind2nd( finder<boost::property_map<Graph, vertex_exec_t>::type>() ,
                             std::make_pair( vmap, int(VertexNode::func_start_node)) ) );
        Vertex funcExit=*
            find_if(v1, v2,
                    bind2nd( finder<boost::property_map<Graph, vertex_exec_t>::type>() ,
                             std::make_pair( vmap, int(VertexNode::func_exit_node)) ) );

        // reset their special meanings.
        vmap[funcStart] = VertexNode::normal_node;
        vmap[funcExit]  = VertexNode::normal_node;
        // connect the graph.
        addConditionEdge( cond, funcStart );
        connectToNext( funcExit, new ConditionTrue );

        return funcExit;
    }
    
    ProgramGraph::CommandNode ProgramGraph::setFunction( FunctionGraph* fn )
    {
        /**
         * This function must/should be used when current has no edges yet leaving.
         * Off course, edges can already be pointing to current of the previous 
         * command. That's why current can not be overwritten with the first node
         * of the function, and we need to insert an extra edge from current to
         * funcStart. That is why we use (until better solution found) the append
         * Function.
         */
        return appendFunction( new ConditionTrue, fn) ;
    }
    
    ProgramGraph::CommandNode ProgramGraph::currentNode() const
    {
        return current;
    }

    size_t ProgramGraph::currentEdges() const
    {
        return out_degree( current, *graph );
    }

    ProgramGraph::CommandNode ProgramGraph::nextNode() const
    {
        return next;
    }

    int ProgramGraph::getLineNumber() const
    {
        return get(vertex_command, program)[current].getLineNumber();
    }

    void ProgramGraph::setLineNumber( int line)
    {
        boost::property_map<Graph, vertex_command_t>::type 
            cmap = get(vertex_command, program);
        cmap[current].setLineNumber( line );
    }

    void ProgramGraph::setLabel( const std::string& str )
    {
        boost::property_map<Graph, vertex_command_t>::type 
            cmap = get(vertex_command, *graph);
        cmap[current].setLabel( str );
    }

    bool ProgramGraph::hasLabel( const std::string& str ) const
    {
        return findNode(str).second;
    }

    std::pair<ProgramGraph::CommandNode, bool> ProgramGraph::findNode( const std::string& str ) const
    {
        graph_traits<Graph>::vertex_iterator v1,v2;
        tie(v1,v2) = vertices(*graph);
        boost::property_map<Graph, vertex_command_t>::type 
            cmap = get(vertex_command, *graph);
        for ( ; v1 != v2; ++v1)
            if ( cmap[*v1].getLabel() == str )
                return std::make_pair(*v1, true);
        return std::make_pair(*v2, false);
    }
}

