/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  GraphCopier.hpp

                        GraphCopier.hpp -  description
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


#include "FunctionGraph.hpp"

    namespace {

        using namespace RTT;
        using namespace RTT::detail;
        using namespace boost;

        // these two are used as policy parameters for the
        // boost::copy_graph function.
        class GraphVertexCopier {
            typedef FunctionGraph::Graph Graph;
            typedef graph_traits<Graph>::vertex_descriptor VertexDesc;
            std::map<const base::DataSourceBase*, base::DataSourceBase*>& rdss;
            property_map<Graph, vertex_command_t>::const_type commandmap1;
            property_map<Graph, vertex_exec_t>::const_type allmap1;
            property_map<Graph, vertex_command_t>::type commandmap2;
            property_map<Graph, vertex_exec_t>::type allmap2;
        public:
            GraphVertexCopier( const Graph& g1, Graph& g2,
                                      std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacementdss )
                : rdss( replacementdss ),
                  commandmap1( get( vertex_command, g1 ) ), allmap1( get( vertex_exec, g1 ) ),
                  commandmap2( get( vertex_command, g2 ) ), allmap2( get( vertex_exec, g2 ) )
                {
                }
            void operator()( const VertexDesc& a, VertexDesc& b )
                {
                    // copy exec prop first
                    put( allmap2, b, get( allmap1, a ) );
                    // copy the commands, using the 'copy' semantics of VertexNode.
                    put( commandmap2, b, get( commandmap1, a ).copy( rdss ) );
                }
        };

        class GraphEdgeCopier {
            typedef FunctionGraph::Graph Graph;
            typedef graph_traits<Graph>::edge_descriptor EdgeDesc;
            std::map<const base::DataSourceBase*, base::DataSourceBase*>& rdss;
            property_map<Graph, edge_condition_t>::const_type conditionmap1;
            property_map<Graph, edge_condition_t>::type conditionmap2;
//             property_map<Graph, edge_exec_t>::const_type allmap1;
//             property_map<Graph, edge_exec_t>::type allmap2;
        public:
            GraphEdgeCopier( const Graph& g1, Graph& g2,
                                    std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacementdss )
                : rdss( replacementdss ),
                  conditionmap1( get( edge_condition, g1 ) ), conditionmap2( get( edge_condition, g2 ) )
//                   allmap1( get( edge_all, g1 ) ), allmap2( get( edge_all, g2 ) )
                {
                }
            void operator()( const EdgeDesc& a, const EdgeDesc& b )
                {
//                     put( allmap2, b, get( allmap1, a ) );
                    put( conditionmap2, b, get( conditionmap1, a ).copy( rdss ) );
                }
        };
    }

