/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  FunctionGraph.hpp

                        FunctionGraph.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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

#ifndef FUNCTION_GRAPH_HPP
#define FUNCTION_GRAPH_HPP

#include "VertexNode.hpp"
#include "EdgeCondition.hpp"
#include "CommandNOP.hpp"
#include "../rtt-config.h"
#include "../base/AttributeBase.hpp"
#include "ProgramInterface.hpp"

namespace RTT
{ namespace scripting {

    typedef boost::shared_ptr<FunctionGraph> FunctionGraphPtr;
    typedef boost::weak_ptr<FunctionGraph> FunctionGraphWPtr;

    /**
     * This class represents a function. It has
     * much in common with a program but is only
     * used for storing a Graph.
     */
    class RTT_API FunctionGraph
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
        typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
        typedef boost::graph_traits<Graph>::edge_descriptor Edge;

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

        Vertex startv;
        Vertex exitv;

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
        std::vector<base::AttributeBase*> args;

        bool pausing;
        bool mstep;

        bool executeUntil();
        bool executeStep();

        interface::ServiceProviderPtr context;
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

        void setProgramTask(interface::ServiceProviderPtr mytask);

        /**
         * To be called after a function is constructed.
         */
        void finish();

        virtual bool start();

        virtual bool execute();

        virtual void unloading();

        virtual bool stop();

        virtual bool pause();

        virtual bool step();

        virtual bool stepDone() const;

        /**
         * Identical to \a stop();
         */
        virtual void reset();

        virtual int  getLineNumber() const;

        virtual const std::string& getName() const;

        virtual FunctionGraph* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& replacementdss ) const;

        virtual FunctionGraph* clone() const;

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
            return startv;
        }

        Vertex exitNode() const
        {
            return exitv;
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
        std::vector<base::AttributeBase*> getArguments() const {
            return args;
        }

        void addArgument( base::AttributeBase* a) {
            args.push_back(a);
        }

        /**
         * Clear the arguments vector and release all base::AttributeBase resources.
         */
        void clearArguments();
    };


}}

#endif
