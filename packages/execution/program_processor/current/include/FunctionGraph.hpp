/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  FunctionGraph.hpp 

                        FunctionGraph.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
#ifndef FUNCTION_GRAPH_HPP
#define FUNCTION_GRAPH_HPP

#include "VertexNode.hpp"
#include "EdgeCondition.hpp"
#include <corelib/CommandNOP.hpp>
#include "ProgramInterface.hpp"

namespace ORO_Execution
{
    using boost::tie;
    using boost::graph_traits;
    using boost::property_map;
    using std::find_if;
    using std::bind2nd;
    using boost::get;
    using boost::put;
    using ORO_CoreLib::CommandNOP;

    class TaskAttributeBase; // defined in TaskContext...

    /**
     * This class represents a function. It has
     * much in common with a program but is only
     * used for storing a Graph.
     */
    class FunctionGraph
        :public ProgramInterface
    {
    public:
        typedef EdgeCondition::EdgeProperty EdgeProperty;
        typedef VertexNode::VertProperty    VertProperty;

        typedef boost::adjacency_list<boost::vecS,
                                      boost::listS,
                                      boost::directedS,
                                      VertProperty,
                                      EdgeProperty> Graph;
        typedef graph_traits<Graph>::vertex_descriptor Vertex;
        typedef graph_traits<Graph>::edge_descriptor Edge;

    private:
        /**
         * The node which is executed now
         */
        Vertex current;

        /**
         * The node that was run before this one.
         */
        Vertex previous;

    protected:
        /**
         * The graph containing this function.
         */
        Graph program;

        Vertex start;
        Vertex exit;

        /**
         * The (unique) name of this program.
         */
        std::string myName;

        /**
         * Program text.
         */
        std::string _text;

        /**
         * Ordered arguments (are also in the repository).
         */
        std::vector<TaskAttributeBase*> args;
    public:
        /**
         * Create a FunctionGraph with a given name.
         */
        FunctionGraph( const std::string& _name );

        /**
         * Copy a FunctionGraph.
         */
        FunctionGraph( const FunctionGraph& orig );

        ~FunctionGraph();

        /**
         * To be called after a function is constructed.
         */
        void finish();

        virtual bool executeStep();

        virtual bool executeAll();

        virtual bool executeUntil();

        virtual void reset();

        /**
         * Clone this FunctionGraph.  This will produce a completely
         * new FunctionGraph, that has nothing in common with this one.
         * It takes care to properly map identical DataSources to
         * identical DataSources.
         *
         * @param alreadyMappedData A map of some DataSources used in
         *   this program to new DataSources that should replace them
         *   in the new Program.  This is provided, because in some
         *   cases the outside world also keeps references to
         *   datasources used somewhere in this programgraph.  It is
         *   then important that when this Program is copied, the
         *   outside world has a way to get a reference to the
         *   corresponding datasources in the new program.  We do this
         *   by allowing it to map some datasources itself, and simply
         *   provide us a list of its mappings.
         */
        FunctionGraph* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacementdss ) const;

        FunctionGraph* clone() const;

        virtual int  getLineNumber() const;

        virtual const std::string& getName() const;

        /**
         * Set the name of this program.
         * Only valid before endProgram() is called.
         */
        void setName(const std::string& _name);

        /**
         * Set the program text.
         */
        void setText( const std::string& t);

        std::string getText() const;

        void debugPrintout() const;

        Vertex startNode() const
        {
            return start;
        }

        Vertex exitNode() const
        {
            return exit;
        }

        const Graph& getGraph() const
        {
            return program;
        }

        Graph& getGraph()
        {
            return program;
        }

        /**
         * Return an ordered list of this funcion's arguments.
         */
        std::vector<TaskAttributeBase*> getArguments() const {
            return args;
        }

        void addArgument( TaskAttributeBase* a) {
            args.push_back(a);
        }

        /**
         * Clear the arguments vector and release all TaskAttributeBase resources.
         */
        void clearArguments();
    };


}

#endif
