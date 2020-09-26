/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  FunctionGraph.cxx

                        FunctionGraph.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

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


#include "rtt/scripting/FunctionGraph.hpp"
#include "rtt/scripting/GraphCopier.hpp"
#include "rtt/base/AttributeBase.hpp"
#include "rtt/scripting/ProgramService.hpp"
#include "rtt/TaskContext.hpp"
#include "rtt/Service.hpp"

#include "rtt/scripting/CommandNOP.hpp"
#include "rtt/scripting/ConditionFalse.hpp"
#include "rtt/scripting/ConditionTrue.hpp"
#include <boost/graph/copy.hpp>
#include <utility>

namespace RTT {
    using namespace detail;
    using namespace boost;
    using namespace std;



    FunctionGraph::FunctionGraph(const std::string& _name, bool unload_on_stop)
        : myName(_name), retn(0), pausing(false), mstep(false), munload_on_stop(unload_on_stop)
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
        std::vector<AttributeBase*> argsvect = orig.getArguments();
        std::vector<AttributeBase*>::iterator ita = argsvect.begin();
        for ( ; ita != argsvect.end(); ++ita)
            this->args.push_back( (*ita)->clone() );
        if (orig.retn)
            retn = orig.retn->clone();
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
        //log(Debug) << "Destroying program '" << getName() << "'" <<endlog();
        if ( this->isLoaded() ){
            getEngine()->removeFunction(this);
        }
        std::vector<AttributeBase*>::iterator it = args.begin();
        for ( ; it != args.end(); ++it)
            delete *it;

    }

    void FunctionGraph::setProgramService(Service::shared_ptr myservice)
    {
        context = myservice;
    }

    void FunctionGraph::setUnloadOnStop(bool unload_on_stop)
    {
        munload_on_stop = unload_on_stop;
    }

    void FunctionGraph::loading()
    {
        // we need to auto-start, or we would be unloaded right away.
        if (munload_on_stop)
            this->start();
    }

    void FunctionGraph::unloading()
    {
        // this function is called in a real-time context when execute() returns false.
        // for functions that have a context object, these should never return false
        // in execute(). See the munload_on_stop flag.
        if ( !context )
            return; // plain function
        // The case for program scripts: they are managed by the ScriptingService, which will
        // take care of unloading.
        if (context->getParent() ) {
            context->getParent()->removeService(context->getName());
        }
        context.reset();
    }


    bool FunctionGraph::start()
    {
        if ( !isLoaded() )
            return false;
        if ( pStatus == Status::stopped ) {
            this->reset();
        }
        if ( pStatus == Status::stopped || pStatus == Status::paused) {
            pStatus = Status::running;
            return true;
        }
        return false;
    }

    bool FunctionGraph::pause()
    {
        if ( isLoaded() ) {
            if ( pStatus == Status::stopped ) {
                this->reset();
            }
            pausing = true;
            return true;
        }
        return false;
    }

    bool FunctionGraph::step()
    {
        if ( isLoaded() && (pStatus == Status::paused) && mstep == false) {
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
                this->executeStep();
                return true;
            } else
                return true;
            break;
        case Status::error:
        case Status::unknown:
        case Status::stopped:
            return !munload_on_stop;
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
                    try {
                        cmap[current].startExecution();
                    } catch(...) {
                        pStatus = Status::error;
                        return false;
                    }
                }

            // initial conditions :
            previous = current;
            // execute the current command.
            try {
                cmap[current].execute();
            } catch(...) {
                pStatus = Status::error;
                return false;
            }

            // Branch selecting Logic :
            if ( cmap[current].isValid() ) {
                for ( tie(ei, ei_end) = boost::out_edges( current, program ); ei != ei_end; ++ei) {
                    try {
                        if ( emap[*ei].evaluate() ) {
                            current = boost::target(*ei, program);
                            // a new node has been found ...
                            // so continue
                            break; // exit from for loop.
                        }
                    } catch(...) {
                        pStatus = Status::error;
                        return false;
                    }
                }
            }
        } while ( previous != current && pStatus == Status::running && !pausing); // keep going if we found a new node

        // check finished state
        if (current == exitv) {
            this->stop();
            return !munload_on_stop;
        }
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
            try {
                cmap[current].startExecution();
            } catch(...) {
                pStatus = Status::error;
                return false;
            }
            previous = current;
        }

        // execute the current command.
        try {
            cmap[current].execute();
        } catch(...) {
            pStatus = Status::error;
            return false;
        }

        // Branch selecting Logic :
        if ( cmap[current].isValid() ) {
            for ( tie(ei, ei_end) = boost::out_edges( current, program ); ei != ei_end; ++ei) {
                try {
                    if ( emap[*ei].evaluate() ) {
                        current = boost::target(*ei, program);
                        if (current == exitv)
                            this->stop();
                        // a new node has been found ...
                        // it will be executed in the next step.
                        return true;
                    }
                } catch(...) {
                    pStatus = Status::error;
                    return false;
                }
            }
        }
        // check finished state
        if (current == exitv)
            this->stop();
        return true; // no new branch found yet !
    }

    void FunctionGraph::reset() {
        current = startv;
        previous = exitv;
        this->stop();
    }

    bool FunctionGraph::stop()
    {
        // stop even works if no pp is present
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
        FunctionGraph* ret = new FunctionGraph( getName(), munload_on_stop );

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
        // thus we can rebuild a vector<AttributeBase*>, which will be automagically be
        // found by copy_graph.
        // func args are never instantiated, so that we can keep making copies.
        for (unsigned int i=0; i < args.size(); ++i)
            ret->addArgument( args[i]->copy( replacementdss, false ) );
        if (retn)
            ret->setResult( retn->copy(replacementdss, false) );

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

//         std::cerr << "Resulted in :" <<std::endl;
//         ret->debugPrintout();

        return ret;
    }

    FunctionGraph* FunctionGraph::clone() const
    {
        return new FunctionGraph(*this);
    }

    void FunctionGraph::debugPrintout() const {
#if 0
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
            ActionInterface* cmd = get( cmap, *v ).getCommand();
            if ( cmd )
                std::cerr << " " << index << " " << typeid( *cmd ).name() << std::endl;
            else
                std::cerr << " " << index << " (null)" << std::endl;
        }
#endif
    }

    void FunctionGraph::clearArguments() {
        for (std::vector<AttributeBase*>::iterator it = args.begin(); it != args.end(); ++it)
            delete *it;
        args.clear();
    }

}

