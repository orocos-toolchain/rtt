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
#include "execution/CommandProgramEndToken.hpp"

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

    namespace {
        // these two are used as policy parameters for the
        // boost::copy_graph function.
        class ProgramGraphVertexCopier {
            typedef ProgramGraph::Graph Graph;
            typedef graph_traits<Graph>::vertex_descriptor VertexDesc;
            std::map<const DataSourceBase*, DataSourceBase*>& rdss;
            property_map<Graph, vertex_command_t>::const_type commandmap1;
            property_map<Graph, vertex_all_t>::const_type allmap1;
            property_map<Graph, vertex_command_t>::type commandmap2;
            property_map<Graph, vertex_all_t>::type allmap2;
        public:
            ProgramGraphVertexCopier( const Graph& g1, Graph& g2,
                                      std::map<const DataSourceBase*, DataSourceBase*>& replacementdss )
                : rdss( replacementdss ),
                  commandmap1( get( vertex_command, g1 ) ), allmap1( get( vertex_all, g1 ) ),
                  commandmap2( get( vertex_command, g2 ) ), allmap2( get( vertex_all, g2 ) )
                {
                }
            void operator()( const VertexDesc& a, VertexDesc& b )
                {
                    put( allmap2, b, get( allmap1, a ) );
                    put( commandmap2, b, get( commandmap1, a ).copy( rdss ) );
                }
        };

        class ProgramGraphEdgeCopier {
            typedef ProgramGraph::Graph Graph;
            typedef graph_traits<Graph>::edge_descriptor EdgeDesc;
            std::map<const DataSourceBase*, DataSourceBase*>& rdss;
            property_map<Graph, edge_condition_t>::const_type conditionmap1;
            property_map<Graph, edge_condition_t>::type conditionmap2;
            property_map<Graph, edge_all_t>::const_type allmap1;
            property_map<Graph, edge_all_t>::type allmap2;
        public:
            ProgramGraphEdgeCopier( const Graph& g1, Graph& g2,
                                    std::map<const DataSourceBase*, DataSourceBase*>& replacementdss )
                : rdss( replacementdss ),
                  conditionmap1( get( edge_condition, g1 ) ), conditionmap2( get( edge_condition, g2 ) ),
                  allmap1( get( edge_all, g1 ) ), allmap2( get( edge_all, g2 ) )
                {
                }
            void operator()( const EdgeDesc& a, const EdgeDesc& b )
                {
                    put( allmap2, b, get( allmap1, a ) );
                    put( conditionmap2, b, get( conditionmap1, a ).copy( rdss ) );
                }
        };
    }

    ProgramGraph::ProgramGraph(const std::string& _name)
        : graph( 0 ), myName(_name)
    {
    }

    ProgramGraph::~ProgramGraph()
    {
    }

    void ProgramGraph::executeToStop()
    {
        static const int maxsteps = 5000;
        int count = 0;

        boost::property_map<Graph, vertex_command_t>::type
            cmap = get(vertex_command, program);
        while ( ! dynamic_cast<CommandProgramEndToken*>( cmap[current].getCommand() ) && count++ <= maxsteps )
            execute();
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

    CommandInterface* ProgramGraph::getCommand( CommandNode cn )
    {
        // the map contains _references_ to all vertex_command properties
        boost::property_map<Graph, vertex_command_t>::type
            cmap = get(vertex_command, *graph);
        // access the one of current
        return cmap[cn].getCommand();
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

    void ProgramGraph::setName(const std::string& name)
    {
        myName = name;
    }

    std::string ProgramGraph::getText() const
    {
        return _text;
    }

    void ProgramGraph::setText(const std::string& text)
    {
        _text = text;
    }

    void ProgramGraph::returnProgram( ConditionInterface* cond )
    {
        add_edge(current, end, EdgeCondition(cond), program );
    }

    void ProgramGraph::endProgram( ProcessorInterface* pci, CommandInterface* finalCommand ) {
        if ( !finalCommand )
            finalCommand = new CommandStopProgram( pci, myName );
        // End of all processing. return already linked to end.
        boost::property_map<Graph, vertex_command_t>::type
            cmap = get(vertex_command, program);
        delete cmap[end].setCommand( finalCommand );
        put(vertex_exec, *graph, end, VertexNode::prog_exit_node );

        assert( current != next );
        assert( out_degree(current, program) == 0 );
        assert( out_degree(next, program) == 0 );
        clear_vertex(current, program);
        clear_vertex(next, program);
        remove_vertex( current, program );
        remove_vertex( next, program );

        // now let's reindex the vertices ( this is necessary for the
        // copy function to work... )
        boost::property_map<Graph, vertex_index_t>::type indexmap =
            get( vertex_index, program );
        boost::graph_traits<Graph>::vertex_iterator v, vend;
        int index = 0;
        for ( tie( v, vend ) = vertices( program ); v != vend; ++v )
            indexmap[*v] = index++;
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
        // the condition. Connect the exitNode() of the function with
        // the next node.

        // first reindex the vertices...
        boost::property_map<Graph, vertex_index_t>::type indexmap =
            get( vertex_index, *graph );
        boost::graph_traits<Graph>::vertex_iterator v, vend;
        int index = 0;
        for ( tie( v, vend ) = vertices( *graph ); v != vend; ++v )
            indexmap[*v] = index++;

        std::map<const DataSourceBase*, DataSourceBase*> replacementdss;
        boost::copy_graph( fn->getGraph(), *graph,
                           boost::vertex_copy( ProgramGraphVertexCopier( fn->getGraph(), *graph, replacementdss ) ).
                           edge_copy( ProgramGraphEdgeCopier( fn->getGraph(), *graph, replacementdss ) ) );
        // the subgraph has been copied but is now 'floating' in the current graph.
        // we search func start and exit points and connect them to
        // the current graph.

        // domi: it would be cleaner if a function would keep a
        // reference to its enter and exit point, and we would use the
        // copy_graph orig_to_copy map to find the corresponding nodes
        // in the copy...
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

    void ProgramGraph::startIfStatement( ConditionInterface* cond, int linenumber )
    {
        // push all relevant nodes on the branch_stack.
        // endIf and endElse will pop them

        // * next will become the first node of the succeeding if statement
        // * after_else_node will become the node after the if block is finished
        // and the else block is finished.
        // * else_node is the first node of a failing if statement (this may be _any_statement, thus also an if)
        CommandNode else_node = add_vertex( *graph );
        put(vertex_exec, *graph, else_node, VertexNode::normal_node );
        branch_stack.push( else_node );
        CommandNode after_else_node =  add_vertex( *graph );
        put(vertex_exec, *graph, after_else_node, VertexNode::normal_node );
        branch_stack.push( after_else_node );
        //branch_stack.push( current );

        // add edge from current to next
        addConditionEdge( cond, next );
        // add edge from current to 'after_else_node'
        addConditionEdge( new ConditionTrue(), else_node );
        proceedToNext(linenumber);
    }

    void ProgramGraph::endIfBlock(){
        // this is called after a proceedToNext of the last statement of
        // the if block.
        // Connect end of if block with after_else_node
        CommandNode after_else_node = branch_stack.top();
        addConditionEdge( new ConditionTrue(), after_else_node );
        branch_stack.pop();
        // make else_node current, next remains.
        moveTo( branch_stack.top(), next );
        branch_stack.pop();
        // store again !
        branch_stack.push( after_else_node );
    }

    // Else : can be empty and is then a plain proceed to next.
    void ProgramGraph::endElseBlock() {
        // after_else_node is on top of stack
        CommandNode after_else_node = branch_stack.top();
        branch_stack.pop();
        addConditionEdge( new ConditionTrue(), after_else_node );
        // make after_else_node current
        moveTo( after_else_node, next );
    }

    void ProgramGraph::startWhileStatement( ConditionInterface* cond, int linenumber )
    {
        // very analogous to the if statement, but there is no else part
        // and we stack the first commandnode to be able to close the loop.
        CommandNode after_while_node = add_vertex( *graph );
        put(vertex_exec, *graph, after_while_node, VertexNode::normal_node );
        branch_stack.push( after_while_node );
        branch_stack.push( current );
        // add edge from current to next if condition == true
        addConditionEdge( cond, next );
        // if condition fails, go from current to 'after_while_node'
        addConditionEdge( new ConditionTrue(), after_while_node );
        proceedToNext(linenumber);
    }

    void ProgramGraph::endWhileBlock()
    {
        CommandNode start_of_while = branch_stack.top();
        branch_stack.pop();
        // go from current back to start (and check there the condition).
        addConditionEdge( new ConditionTrue(), start_of_while );
        CommandNode after_while_node =  branch_stack.top();
        branch_stack.pop();
        moveTo( after_while_node, next );
    }

    ProgramGraph* ProgramGraph::copy( std::map<const DataSourceBase*, DataSourceBase*>& replacementdss ) const
    {
        typedef boost::property_map<Graph, vertex_index_t>::const_type indexmap_t;
        typedef boost::graph_traits<Graph>::vertex_descriptor vd_t;
        typedef std::vector<vd_t> o2cvect_t;
        typedef boost::iterator_property_map<o2cvect_t::iterator, indexmap_t, vd_t, vd_t&> o2cmap_t;
        ProgramGraph* ret = new ProgramGraph( getName() );
        indexmap_t indexmap = get( vertex_index, program );
        // here we assume that the indexing of program is set properly...
        o2cvect_t o2cvect( num_vertices( program ) );
        o2cmap_t o2cmap( o2cvect.begin(), indexmap );

        boost::copy_graph( program, ret->program,
                           boost::vertex_copy( ProgramGraphVertexCopier( program, ret->program, replacementdss ) ).
                           edge_copy( ProgramGraphEdgeCopier( program, ret->program, replacementdss ) ).
                           orig_to_copy( o2cmap ) );

        ret->root = o2cmap[root];
        ret->end = o2cmap[end];
        ret->current = o2cmap[current];
        ret->previous = o2cmap[previous];

        return ret;
    }

    void ProgramGraph::prependCommand( CommandInterface* command, int line_nr )
    {
        CommandNode previousroot = root;
        root = add_vertex( program );
        boost::property_map<Graph, vertex_exec_t>::type
            vmap = get(vertex_exec, program);
        put( vmap, root, get( vmap, previousroot ) );
        put( vmap, previousroot, VertexNode::normal_node );
        boost::property_map<Graph, vertex_command_t>::type
            cmap = get( vertex_command, program );
        VertexNode vnode( command );
        vnode.setLineNumber( line_nr );
        put( cmap, root, vnode );
        add_edge(root, previousroot, EdgeCondition(new ConditionTrue), program);
        if ( current == previousroot )
            current = root;

        // now let's reindex the vertices...
        boost::property_map<Graph, vertex_index_t>::type indexmap =
            get( vertex_index, *graph );
        boost::graph_traits<Graph>::vertex_iterator v, vend;
        int index = 0;
        for ( tie( v, vend ) = vertices( *graph ); v != vend; ++v )
            indexmap[*v] = index++;
    }

    void ProgramGraph::debugPrintout() const {
        graph_traits<Graph>::vertex_iterator v,vend;
        tie(v,vend) = vertices(program);
        boost::property_map<Graph, vertex_command_t>::const_type
            cmap = get(vertex_command, program);
        boost::property_map<Graph, vertex_index_t>::const_type
            imap = get(vertex_index, program);
        std::cerr << "program " << getName() << std::endl;
        std::cerr << " number of vertices: " << boost::num_vertices(program) << std::endl;
        for ( ; v != vend; ++v )
        {
            int index = get( imap, *v );
            CommandInterface* cmd = get( cmap, *v ).getCommand();
            if ( cmd )
                std::cerr << " " << index << " " << typeid( *cmd ).name() << std::endl;
            else
                std::cerr << " " << index << " (null)" << std::endl;
        }
    }
}

