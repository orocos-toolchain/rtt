#include <execution/FunctionGraph.hpp>

    namespace {
        using namespace ORO_Execution;
        using namespace ORO_CoreLib;
        using namespace boost;

        // these two are used as policy parameters for the
        // boost::copy_graph function.
        class GraphVertexCopier {
            typedef FunctionGraph::Graph Graph;
            typedef graph_traits<Graph>::vertex_descriptor VertexDesc;
            std::map<const DataSourceBase*, DataSourceBase*>& rdss;
            property_map<Graph, vertex_command_t>::const_type commandmap1;
            property_map<Graph, vertex_all_t>::const_type allmap1;
            property_map<Graph, vertex_command_t>::type commandmap2;
            property_map<Graph, vertex_all_t>::type allmap2;
        public:
            GraphVertexCopier( const Graph& g1, Graph& g2,
                                      std::map<const DataSourceBase*, DataSourceBase*>& replacementdss )
                : rdss( replacementdss ),
                  commandmap1( get( vertex_command, g1 ) ), allmap1( get( vertex_all, g1 ) ),
                  commandmap2( get( vertex_command, g2 ) ), allmap2( get( vertex_all, g2 ) )
                {
                }
            void operator()( const VertexDesc& a, VertexDesc& b )
                {
                    // my guess : copy all props first
                    put( allmap2, b, get( allmap1, a ) );
                    // re-copy the commands, using the 'copy' semantics of VertexNode.
                    put( commandmap2, b, get( commandmap1, a ).copy( rdss ) );
                }
        };

        class GraphEdgeCopier {
            typedef FunctionGraph::Graph Graph;
            typedef graph_traits<Graph>::edge_descriptor EdgeDesc;
            std::map<const DataSourceBase*, DataSourceBase*>& rdss;
            property_map<Graph, edge_condition_t>::const_type conditionmap1;
            property_map<Graph, edge_condition_t>::type conditionmap2;
            property_map<Graph, edge_all_t>::const_type allmap1;
            property_map<Graph, edge_all_t>::type allmap2;
        public:
            GraphEdgeCopier( const Graph& g1, Graph& g2,
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

