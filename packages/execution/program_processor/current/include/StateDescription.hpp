/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  StateDescription.hpp 

                        StateDescription.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
#ifndef STATE_DESCRIPTION_HPP
#define STATE_DESCRIPTION_HPP

#include "VertexNode.hpp"
#include "EdgeCondition.hpp"
#include "corelib/StateInterface.hpp"
#include "StateGraph.hpp"

namespace ORO_Execution
{
    using boost::tie;
    using boost::graph_traits;
    using boost::property_map;
    using std::find_if;
    using std::bind2nd;
    using boost::get;
    using boost::put;

    /**
     * @brief This class represents a state with all actions stored
     * in an external StateGraph.
     *
     * It keeps track of the entry node,
     * handler node and exit node of a state in a
     * StateGraph.
     */
    class StateDescription
        : public StateInterface
    {
        typedef EdgeCondition::EdgeProperty EdgeProperty;
        typedef VertexNode::VertProperty    VertProperty;

        typedef boost::adjacency_list<boost::vecS,
                                      boost::listS,
                                      boost::directedS,
                                      VertProperty,
                                      EdgeProperty> Graph;
        typedef graph_traits<Graph>::vertex_descriptor Vertex;
        typedef graph_traits<Graph>::edge_descriptor Edge;

        Vertex mentry;
        Vertex mexit;
        Vertex mhandle;
        bool inited;
        StateGraph* sg;
        std::string name;
    public:
        /**
         * Construct a new State with entry, exit and handle nodes.
         * The StateGraph owning the nodes is needed for processing each state.
         */
        StateDescription(const std::string& _name, Vertex _entry, Vertex _exit, Vertex _handle, StateGraph* _sg ) 
            : mentry(_entry), mexit(_exit), mhandle(_handle), inited(false), sg(_sg), name(_name)
        {
        }

//         StateDescription( const StateDescription& orig )
//             : mentry( orig.entryNode() ),
//               mexit( orig.exitNode() ),
//               mhandle( orig.handleNode() ),
//               inited( orig.isDefined() ),
//               sg( orig.context() )
//         {
//         }

        const std::string& getName() { return name; }

        virtual void onEntry() 
        {
            sg->process( mentry );
        }

        virtual void handle()
        {
            sg->process( mhandle );
        }

        virtual void onExit()
        {
            sg->process( mexit );
        }
            

        bool isDefined() const
        {
            return inited;
        }

        void init() 
        {
            inited = true;
        }

        Vertex entryNode() const
        {
            return mentry;
        }

        Vertex exitNode() const
        {
            return mexit;
        }

        Vertex handleNode() const
        {
            return mhandle;
        }

        StateGraph* context() const
        {
            return sg;
        }
    };


}

#endif
