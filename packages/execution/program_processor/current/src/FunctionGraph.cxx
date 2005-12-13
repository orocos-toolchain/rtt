/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  FunctionGraph.cxx 

                        FunctionGraph.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#include "execution/FunctionGraph.hpp"
#include "GraphCopier.hpp"
#include "execution/TaskAttribute.hpp"
#include "execution/ProgramTask.hpp"

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

    FunctionGraph::FunctionGraph(const std::string& _name)
        : myName(_name), pausing(false), mstep(false), context(0)
    {
        // the start vertex of our function graph
        startv = add_vertex( program );
        put(vertex_exec, program, startv, VertexNode::normal_node );
        exitv = add_vertex( program );
        put(vertex_exec, program, exitv, VertexNode::normal_node);
    }

    FunctionGraph::FunctionGraph( const FunctionGraph& orig )
        :  program( orig.getGraph() ), myName( orig.getName() )
    {
        // The nodes are copied, which causes a clone of their contents.
        graph_traits<Graph>::vertex_iterator v1,v2, it;
        tie(v1,v2) = vertices(program);
        for ( it=v1; it != v2; ++it)
            if ( get( vertex_exec, program, *it) == VertexNode::func_start_node )
                break;
        startv = *v1;
        for ( it=v1; it != v2; ++it)
            if ( get( vertex_exec, program, *it) == VertexNode::func_exit_node )
                break;
        exitv = *v1;

        // Copy-clone over the TAB pointers.
        std::vector<TaskAttributeBase*> argsvect = orig.getArguments();
        std::vector<TaskAttributeBase*>::iterator ita = argsvect.begin();
        for ( ; ita != argsvect.end(); ++ita)
            this->args.push_back( (*ita)->clone() );
        this->finish();
    }

    void FunctionGraph::finish()
    {
        put(vertex_exec, program, startv, VertexNode::func_start_node );
        put(vertex_exec, program, exitv, VertexNode::func_exit_node);

        // Because we use listS, we need to re-index the map :-(
        // If we do not do this, it can not be copied by the copy_graph
        // function.
        property_map<Graph, vertex_index_t>::type
            index = get(vertex_index, program);

        // initialize the vertex_index property values
        // so that it can be copied into other graphs.
        graph_traits<Graph>::vertex_iterator vi, vend;
        graph_traits<Graph>::vertices_size_type cnt = 0;
        for(tie(vi,vend) = vertices(program); vi != vend; ++vi)
            put(index, *vi, cnt++);
        this->reset();
    }

    FunctionGraph::~FunctionGraph()
    {
        std::vector<TaskAttributeBase*>::iterator it = args.begin();
        for ( ; it != args.end(); ++it)
            delete *it;
        this->handleUnload();
    }

    void FunctionGraph::setProgramTask(ProgramTask* mytask)
    {
        context = mytask;
    }

    void FunctionGraph::handleUnload()
    {
        // just kill off the interface.
        if ( context && context->getPeer("programs") )
            context->getPeer("programs")->removePeer( context->getName() );
        delete context;
        context = 0;
    }


    bool FunctionGraph::start()
    {
        if ( !pp || !pp->getTask() || !pp->getTask()->isRunning() )
            return false;
        if ( pStatus == Status::stopped || pStatus == Status::paused) {
            pStatus = Status::running;
            return true;
        }
        return false;
    }

    bool FunctionGraph::pause()
    {
        if ( pp ) {
            pausing = true;
            return true;
        }
        return false;
    }

    bool FunctionGraph::step()
    {
        if ( pp && (pStatus == Status::paused) && mstep == false) {
            mstep = true;
            return true;
        }
        return false;
    }

    bool FunctionGraph::stepDone() const
    {
        return mstep == false;
    }

    bool FunctionGraph::execute()
    {
        if (pausing) {
            pStatus = Status::paused;
            pausing = false;
            return true;
        }
        switch (pStatus) {
        case Status::running:
            return this->executeUntil();
            break;
        case Status::paused:
            if (mstep) {
                mstep = false;
                return this->executeStep();
            } else
                return true;
            break;
        case Status::error:
        case Status::unloaded:
            return false;
            break;
        case Status::stopped:
            return true;
            break;
        }
        return false;
    }
            

    bool FunctionGraph::executeUntil()
    {
        graph_traits<Graph>::out_edge_iterator ei, ei_end;
        // the map contains _references_ to all vertex_command properties
        boost::property_map<Graph, vertex_command_t>::type
            cmap = get(vertex_command, program);
        boost::property_map<Graph, edge_condition_t>::type
            emap = get(edge_condition, program);

        do {
            // Check this always on entry of executeUntil :
            // initialise current node if needed and reset all its out_edges
            // if previous == current, we DO NOT RESET, because we want to check
            // if previous command has completed !
            if ( previous != current )
                {
                    for ( tie(ei, ei_end) = boost::out_edges( current, program ); ei != ei_end; ++ei)
                        emap[*ei].reset();
                    cmap[current].startExecution();
                }

            // initial conditions :
            previous = current;
            // execute the current command.
            if ( !cmap[current].execute() ) {
                pStatus = Status::error;
                return false;
            }
        
            // Branch selecting Logic :
            for ( tie(ei, ei_end) = boost::out_edges( current, program ); ei != ei_end; ++ei)
                {
                    if ( emap[*ei].evaluate() )
                        {
                            current = boost::target(*ei, program);
                            // a new node has been found ...
                            // so continue
                            break; // exit from for loop.
                        }
                }
        } while ( previous != current ); // keep going if we found a new node

        // check finished state
        if (current == exitv)
            this->stop();
        return true; // we need to wait.
    }

    bool FunctionGraph::executeStep()
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
            previous = current;
        }

        // execute the current command.
        if ( !cmap[current].execute() ) {
            pStatus = Status::error;
            return false;
        }

        // Branch selecting Logic :
        for ( tie(ei, ei_end) = boost::out_edges( current, program ); ei != ei_end; ++ei)
        {
            if ( emap[*ei].evaluate() )
            {
                current = boost::target(*ei, program);
                if (current == exitv)
                    this->stop();
                // a new node has been found ...
                // it will be executed in the next step.
                return true;
            }
        } 
        // check finished state
        if (current == exitv)
            this->stop();
        return true; // no new branch found yet !
    }

    void FunctionGraph::reset() {
        this->stop();
    }

    bool FunctionGraph::stop()
    {
        // stop even works if no pp is present
        current = startv;
        previous = exitv;
        pStatus = Status::stopped;
        return true;
    }

    const std::string& FunctionGraph::getName() const
    {
        return myName;
    }

    void FunctionGraph::setName(const std::string& name)
    {
        myName = name;
    }

    std::string FunctionGraph::getText() const
    {
        return _text;
    }

    void FunctionGraph::setText(const std::string& text)
    {
        _text = text;
    }

    int FunctionGraph::getLineNumber() const
    {
        return get(vertex_command, program)[current].getLineNumber();
    }

    FunctionGraph* FunctionGraph::copy( std::map<const DataSourceBase*, DataSourceBase*>& replacementdss ) const
    {
        typedef boost::property_map<Graph, vertex_index_t>::const_type indexmap_t;
        typedef boost::graph_traits<Graph>::vertex_descriptor vd_t;
        typedef std::vector<vd_t> o2cvect_t;
        typedef boost::iterator_property_map<o2cvect_t::iterator, indexmap_t, vd_t, vd_t&> o2cmap_t;
        FunctionGraph* ret = new FunctionGraph( getName() );

        // clear out unneccessary vertices ( we will copy new ones below )
        remove_vertex( ret->startv, ret->program );
        remove_vertex( ret->exitv, ret->program );

        indexmap_t indexmap = get( vertex_index, program );
        // here we assume that the indexing of program is set properly...
        o2cvect_t o2cvect( num_vertices( program ) );
        o2cmap_t o2cmap( o2cvect.begin(), indexmap );

//         std::cerr << "Start copy of " <<std::endl;
//         this->debugPrintout();
//         std::cerr << "Empty ret: " <<std::endl;
//         ret->debugPrintout();
        // The replacementdss map contains mappings from this->datasource to copy->datasource, 
        // thus we can rebuild a vector<TaskAttributeBase*>, which will be automagically be
        // found by copy_graph.
        for (unsigned int i=0; i < args.size(); ++i)
            ret->addArgument( args[i]->copy( replacementdss, false ) ); // func args are never instantiated, always local.

        boost::copy_graph( program, ret->program,
                           boost::vertex_copy( GraphVertexCopier( program, ret->program, replacementdss ) ).
                           edge_copy( GraphEdgeCopier( program, ret->program, replacementdss ) ).
                           orig_to_copy( o2cmap ) );

        ret->startv = o2cmap[startv];
        ret->exitv = o2cmap[exitv];
        ret->current = o2cmap[current];
        ret->previous = o2cmap[previous];

        // so that ret itself can be copied again :
        ret->finish();

        // copy progproc state also
        if (this->pp)
            ret->setProgramProcessor(pp);
//         std::cerr << "Resulted in :" <<std::endl;
//         ret->debugPrintout();

        return ret;
    }

    FunctionGraph* FunctionGraph::clone() const
    {
        return new FunctionGraph(*this);
    }

    void FunctionGraph::debugPrintout() const {
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

    void FunctionGraph::clearArguments() {
        for (std::vector<TaskAttributeBase*>::iterator it = args.begin(); it != args.end(); ++it)
            delete *it;
        args.clear();
    }

}

