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
#include "execution/CommandComposite.hpp"
#include "execution/TaskAttribute.hpp"
//#include "execution/parse_exception.hpp"
#include "execution/FactoryExceptions.hpp"
#include "GraphCopier.hpp"

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
        : FunctionGraph(_name), graph( 0 )
    {
    }

    ProgramGraph::~ProgramGraph()
    {
    }

    FunctionGraph* ProgramGraph::startFunction(const std::string& fname)
    {
        // next node should be 'empty'/ not used here.

        // a function is to be constructed
        FunctionGraph* fn =  new FunctionGraph( fname );
        graph   = &fn->getGraph();
        build   = fn->startNode();
        next    = add_vertex( *graph );
        put( vertex_exec, *graph, next, VertexNode::normal_node );
        return fn;
    }

    void ProgramGraph::returnFunction( ConditionInterface* cond, FunctionGraph* fn)
    {
        // connect the build node to the exitNode under a condition,
        // for example, the build implicit term condition.
        add_edge(build, fn->exitNode(), EdgeCondition(cond), *graph);
    }

    void ProgramGraph::endFunction( FunctionGraph* fn)
    {
        // A return statement is obligatory, so the returnFunction has
        // already connected the build node to exitNode of the function.
        // the end of the statement will proceedToNext
        // remove the empty next nodes.
        remove_vertex( build, *graph );
        remove_vertex( next, *graph );
        fn->finish();
    }

    ProgramGraph::CommandNode ProgramGraph::addCommand( ConditionInterface* cond,  CommandInterface* com )
    {
        add_edge(build, next, EdgeCondition(cond), *graph);
        build = next;
        setCommand(com);
        next    = add_vertex( *graph );
        put( vertex_exec, *graph, next, VertexNode::normal_node );
        return next;
    }

    void ProgramGraph::addConditionEdge( ConditionInterface* cond, CommandNode vert )
    {
        add_edge(build, vert, EdgeCondition(cond), *graph);
    }

    void ProgramGraph::closeConditionEdge( CommandNode vert, ConditionInterface* cond )
    {
        add_edge(vert, build, EdgeCondition(cond), *graph);
    }

    ProgramGraph::CommandNode ProgramGraph::moveTo( CommandNode _build, CommandNode _next, int linenumber )
    {
        this->setLineNumber( linenumber );
        CommandNode old = build;
        build = _build;
        next    = _next;
        return old;
    }

    CommandInterface* ProgramGraph::getCommand( CommandNode cn )
    {
        // the map contains _references_ to all vertex_command properties
        boost::property_map<Graph, vertex_command_t>::type
            cmap = get(vertex_command, *graph);
        // access the one of build
        return cmap[cn].getCommand();
    }

    void ProgramGraph::setCommand( CommandInterface* comm )
    {
        this->setCommand(build, comm);
    }

    void ProgramGraph::setCommand(CommandNode cn, CommandInterface* comm )
    {
        // the map contains _references_ to all vertex_command properties
        boost::property_map<Graph, vertex_command_t>::type
            cmap = get(vertex_command, *graph);
        // access the one of build
        delete cmap[cn].setCommand( comm );
    }

    ProgramGraph::CommandNode ProgramGraph::proceedToNext( ConditionInterface* cond, int this_line )
    {
        add_edge(build, next, EdgeCondition(cond), *graph);
        return proceedToNext( this_line );
    }

    ProgramGraph::CommandNode ProgramGraph::proceedToNext( int this_line )
    {
        if ( this_line )
            this->setLineNumber( this_line );
        build = next;
        next    = add_vertex( *graph );
        put(vertex_exec, *graph, next, VertexNode::normal_node );
        return build;
    }

    void ProgramGraph::connectToNext( CommandNode v, ConditionInterface* cond )
    {
        add_edge( v, next, EdgeCondition(cond), *graph);
    }

    ProgramGraph::CommandNode ProgramGraph::buildNode() const
    {
        return build;
    }

    size_t ProgramGraph::buildEdges() const
    {
        return out_degree( build, *graph );
    }

    ProgramGraph::CommandNode ProgramGraph::nextNode() const
    {
        return next;
    }

    void ProgramGraph::setLineNumber( int line )
    {
        boost::property_map<Graph, vertex_command_t>::type
            cmap = get(vertex_command, *graph);
        cmap[build].setLineNumber( line );
    }

    ProgramGraph::CommandNode ProgramGraph::startProgram()
    {
        // we work now on the program.
        graph = &program;
        // the build node
        build = this->start;
        put(vertex_exec, *graph, build, VertexNode::prog_start_node );
        next    = add_vertex( *graph );
        put(vertex_exec, *graph, next, VertexNode::normal_node );

        //exit  = add_vertex( *graph );
        //put(vertex_exec, *graph, next, VertexNode::normal_node );
        //start = build;

        return build;
    }

    void ProgramGraph::returnProgram( ConditionInterface* cond )
    {
        add_edge(build, exit, EdgeCondition(cond), program );
    }

    void ProgramGraph::endProgram( CommandInterface* finalCommand ) {
        if ( !finalCommand )
            finalCommand = new CommandNOP();
        // End of all processing. return already linked to end.
        boost::property_map<Graph, vertex_command_t>::type
            cmap = get(vertex_command, program);
        delete cmap[exit].setCommand( finalCommand );
        put(vertex_exec, *graph, exit, VertexNode::prog_exit_node );

        assert( build != next );
        assert( out_degree(build, program) == 0 );
        assert( out_degree(next, program) == 0 );
        clear_vertex(build, program);
        clear_vertex(next, program);
        remove_vertex( build, program );
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
                // return : Is this the node with the given property ?
                return to_find.first[v] == to_find.second;
            }
    };

    ProgramGraph::CommandNode ProgramGraph::appendFunction( ConditionInterface* cond, FunctionGraph* fn,
                                                            std::vector<DataSourceBase*> fnargs )
    {
        /**
         * This can be used if a function must be called conditionally.
         */
        // Do not move build inhere !
        // copy FunctionGraph's graph into the current *graph, add an edge with
        // the condition. Connect the exitNode() of the function with
        // the next node.

        // The reason to reindex the vertexes of source graph is
        // to silence valgrinds warnings about uninitialised values
        // in the following piece of code. Technically, copy_graph
        // does not need it.
        boost::property_map<Graph, vertex_index_t>::type indexmap =
            get( vertex_index, *graph );
        boost::graph_traits<Graph>::vertex_iterator v, vend;
        int index = 0;
        for ( tie( v, vend ) = vertices( *graph ); v != vend; ++v )
            indexmap[*v] = index++;

        // check nb of arguments:
        std::vector<TaskAttributeBase*> origlist = fn->getArguments();
        if ( fnargs.size() != origlist.size() )
            throw wrong_number_of_args_exception( origlist.size(), fnargs.size() );
            
        // make a deep copy of the function :
        std::map<const DataSourceBase*, DataSourceBase*> replacementdss;
        std::vector<TaskAttributeBase*> newlist;
        for (unsigned int i=0; i < origlist.size(); ++i)
            newlist.push_back( origlist[i]->copy( replacementdss ) );
        // newlist contains the DS which need initialisations :

        // create commands that init all the args :
        CommandComposite* icom=  new CommandComposite();
        std::vector<DataSourceBase*>::const_iterator dit = fnargs.begin();
        std::vector<TaskAttributeBase*>::const_iterator tit =  newlist.begin();
        try {
            for (; dit != fnargs.end(); ++dit, ++tit)
                icom->add( (*tit)->assignCommand( *dit, true ) );
        }
        catch( const bad_assignment& e) {
            // cleanup allocated memory
            for (unsigned int i=0; i < newlist.size(); ++i)
                delete newlist[i];
            delete icom;
            int parnb = (dit - fnargs.begin());
            throw wrong_types_of_args_exception(parnb);
        }

        // set the init command on the build node 
        //assert( build not used by other than NOP )
        this->setCommand( icom );

        boost::copy_graph( fn->getGraph(), *graph,
                           boost::vertex_copy( GraphVertexCopier( fn->getGraph(), *graph, replacementdss ) ).
                           edge_copy( GraphEdgeCopier( fn->getGraph(), *graph, replacementdss ) ) );

        // the subgraph has been copied but is now 'floating' in the current graph.
        // we search func start and exit points and connect them to
        // the current graph.

        // domi: it would be cleaner if a function would keep a
        // reference to its enter and exit point, and we would use the
        // copy_graph orig_to_copy map to find the corresponding nodes
        // in the copy...
        graph_traits<Graph>::vertex_iterator v1,v2, vc;
        tie(v1,v2) = vertices(*graph);
        boost::property_map<Graph, vertex_exec_t>::type
            vmap = get(vertex_exec, *graph);

//         // test not initialised value.
//         vc = v1;
//         while (vc != v2) {
//             std::cerr << int( vmap[*vc] )<<std::endl;
//             ++vc;
//         }
//         std::cerr<<std::endl;
//         // test foo :
//         tie(v1,v2) = vertices(fn->getGraph());
//             vmap = get(vertex_exec, fn->getGraph());
//         vc = v1;
//         while (vc != v2) {
//             std::cerr << int( vmap[*vc] )<<std::endl;
//             ++vc;
//         }


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

    ProgramGraph::CommandNode ProgramGraph::setFunction( FunctionGraph* fn,
                                                         std::vector<DataSourceBase*> fnargs )
    {
        /**
         * This function must/should be used when build has no edges yet leaving.
         * Off course, edges can already be pointing to build of the previous
         * command. That's why build can not be overwritten with the first node
         * of the function, and we need to insert an extra edge from build to
         * funcStart. That is why we use (until better solution found) the append
         * Function.
         */
        return appendFunction( new ConditionTrue, fn, fnargs) ;
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
        //branch_stack.push( build );

        // add edge from build to next
        addConditionEdge( cond, next );
        // add edge from build to 'after_else_node'
        addConditionEdge( new ConditionTrue(), else_node );
        proceedToNext(linenumber);
    }

    void ProgramGraph::endIfBlock(int linenumber){
        // this is called after a proceedToNext of the last statement of
        // the if block.
        // Connect end of if block with after_else_node
        CommandNode after_else_node = branch_stack.top();
        addConditionEdge( new ConditionTrue(), after_else_node );
        branch_stack.pop();
        // make else_node build, next remains.
        moveTo( branch_stack.top(), next, linenumber );
        branch_stack.pop();
        // store again !
        branch_stack.push( after_else_node );
    }

    // Else : can be empty and is then a plain proceed to next.
    void ProgramGraph::endElseBlock(int linenumber) {
        // after_else_node is on top of stack
        CommandNode after_else_node = branch_stack.top();
        branch_stack.pop();
        addConditionEdge( new ConditionTrue(), after_else_node );
        // make after_else_node build
        moveTo( after_else_node, next, linenumber );
    }

    void ProgramGraph::startWhileStatement( ConditionInterface* cond, int linenumber )
    {
        // very analogous to the if statement, but there is no else part
        // and we stack the first commandnode to be able to close the loop.
        CommandNode after_while_node = add_vertex( *graph );
        put(vertex_exec, *graph, after_while_node, VertexNode::normal_node );
        branch_stack.push( after_while_node );
        branch_stack.push( build );
        // add edge from build to next if condition == true
        addConditionEdge( cond, next );
        // if condition fails, go from build to 'after_while_node'
        addConditionEdge( new ConditionTrue(), after_while_node );
        proceedToNext(linenumber);
    }

    void ProgramGraph::endWhileBlock(int linenumber)
    {
        CommandNode start_of_while = branch_stack.top();
        branch_stack.pop();
        // go from build back to start (and check there the condition).
        addConditionEdge( new ConditionTrue(), start_of_while );
        CommandNode after_while_node =  branch_stack.top();
        branch_stack.pop();
        moveTo( after_while_node, next, linenumber );
    }

    void ProgramGraph::prependCommand( CommandInterface* command, int line_nr )
    {
        CommandNode previousstart = start;
        start = add_vertex( program );
        boost::property_map<Graph, vertex_exec_t>::type
            vmap = get(vertex_exec, program);
        put( vmap, start, get( vmap, previousstart ) );
        put( vmap, previousstart, VertexNode::normal_node );
        boost::property_map<Graph, vertex_command_t>::type
            cmap = get( vertex_command, program );
        VertexNode vnode( command );
        vnode.setLineNumber( line_nr );
        put( cmap, start, vnode );
        add_edge(start, previousstart, EdgeCondition(new ConditionTrue), program);
        if ( build == previousstart )
            build = start;

        // now let's reindex the vertices...
        boost::property_map<Graph, vertex_index_t>::type indexmap =
            get( vertex_index, *graph );
        boost::graph_traits<Graph>::vertex_iterator v, vend;
        int index = 0;
        for ( tie( v, vend ) = vertices( *graph ); v != vend; ++v )
            indexmap[*v] = index++;
    }

}

