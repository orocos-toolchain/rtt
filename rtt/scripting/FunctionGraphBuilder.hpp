/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ProgramGraph.hpp

                        ProgramGraph.hpp -  description
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

#ifndef PROGRAMGRAPH_HPP
#define PROGRAMGRAPH_HPP

#include "rtt/scripting/FunctionGraph.hpp"
#include "rtt/base/DataSourceBase.hpp"

#include <utility>                   // for std::pair
#include <stack>
#include <map>
#include <string>
#include <boost/graph/graph_traits.hpp>

namespace RTT
{ namespace scripting {


    /**
     * @brief This class builds a program consisting of
     * data contained in a program graph tree, based on the
     * Boost Graph Library.
     */
    class RTT_SCRIPTING_API FunctionGraphBuilder
    {
    public:

        typedef EdgeCondition::EdgeProperty EdgeProperty;
        typedef VertexNode::VertProperty    VertProperty;

        typedef boost::adjacency_list<boost::vecS, boost::listS, boost::directedS, VertProperty, EdgeProperty> Graph;
        typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
        typedef boost::graph_traits<Graph>::edge_descriptor Edge;

        /**
         * A CommandNode serves as a token to construct
         * a vertex or node, containing a command.
         */
        typedef FunctionGraph::Vertex CommandNode ;

        /**
         * A ConditionEdge serves as a token to construct
         * an edge, containing a condition.
         */
        typedef FunctionGraph::Edge ConditionEdge ;

        /**
         * Constructs a Function graph builder.
         */
        FunctionGraphBuilder();

        ~FunctionGraphBuilder();

        /**
         * Set the line number of the build command node.
         * @param ln The line number.
         */
        void setLineNumber( int ln );

        void setName(const std::string& _name);

        /**
         * Start building a new function.
         */
        FunctionGraphPtr startFunction( const std::string& fname );

        /**
         * Function return is detected inside the function.
         * Do not confuse with endFunction().
         *
         * @param cond The Condition under which to return.
         * @param line The line number of that line in the function.
         */
        void returnFunction( ConditionInterface* cond, int line );

        /**
         * Finish building the function and return the result.
         * @return the finished function.
         */
        FunctionGraphPtr endFunction( int line = 0 );

        /**
         * Return a pointer to the function being built, zero if none.
         */
        FunctionGraphPtr getFunction();

        void startIfStatement( ConditionInterface* cond, int linenumber );
        void endIfBlock(int linenumber);
        void endElseBlock(int linenumber);

        void startWhileStatement( ConditionInterface* cond, int linenumber );
        void endWhileBlock(int linenumber);

        bool inLoop();
        bool breakLoop();

        /**
         * Add a new command from the build CommandNode under a condition.
         *
         * @return the 'new' build CommandNode.
         */
        CommandNode addCommand( ConditionInterface* cond,  base::ActionInterface* com );

        /**
         * Add an edge from the build CommandNode to the given CommandNode
         * without changing the build CommandNode.
         *
         */
        void addConditionEdge( ConditionInterface* cond, CommandNode vert );

        /**
         * Add an edge between the given CommandNode and the build CommandNode.
         *
         */
        void closeConditionEdge( CommandNode vert, ConditionInterface* cond );

        /**
         * Select an already added CommandNode.
         *
         * @return the previous CommandNode.
         */
        CommandNode moveTo( CommandNode _build, CommandNode _next, int linenr );

        /**
         * Sets a new Command on the build CommandNode.
         *
         */
        void setCommand( base::ActionInterface* comm );

        /**
         * Get the Command on a given CommandNode.
         *
         */
        base::ActionInterface* getCommand( CommandNode cn );

        /**
         * Sets a (new) command on a given CommandNode.
         *
         * @param vert The CommandNode to be adapted
         * @param comm The new Command to be executed in that node.
         *
         */
        void setCommand( CommandNode vert, base::ActionInterface* comm);

        /**
         * Append a function to the build CommandNode.
         *
         * @param fn   The Function to append from the build CommandNode
         * @param cond The 'enter' condition
         *
         * @return the last CommandNode of the appended function.
         */
        CommandNode appendFunction( ConditionInterface* cond, FunctionGraphPtr fn, std::vector<base::DataSourceBase::shared_ptr> fnargs);

        /**
         * Put a function in the build CommandNode.
         *
         * @param fn   The Function to append from the build CommandNode
         *
         * @return the last CommandNode of the appended function.
         */
        CommandNode setFunction( FunctionGraphPtr fn, std::vector<base::DataSourceBase::shared_ptr> fnargs);

        /**
         * Proceed to the 'next' CommandNode.
         *
         * @param line_nr The line number of the 'build' command.
         * @return The new build CommandNode.
         */
        CommandNode proceedToNext( int line_nr = 0 );

        /**
         * Proceed to the 'next' CommandNode and add an edge
         * with a condition.
         *
         * @param cond The condition under which to proceed to the next node.
         * @param line_nr The line number of the 'build' command.
         * @return The new build CommandNode.
         */
        CommandNode proceedToNext( ConditionInterface* cond, int line_nr = 0 );

        /**
         * Connect the given CommandNode to the 'next' CommandNode.
         */
        void connectToNext( CommandNode v, ConditionInterface* cond );

        /**
         * Return the build CommandNode.
         */
        CommandNode buildNode() const;

        /**
         * Return the number of edges of the build CommandNode.
         */
        size_t buildEdges() const;

        /**
         * Return the next CommandNode.
         */
        CommandNode nextNode() const;

        /**
         * The node which will be built next.
         */
        CommandNode build;

        /**
         * The node which will be built next.
         */
        CommandNode next;

    private:

        FunctionGraphPtr func;

        /**
         * The graph currently working on.
         */
        Graph* graph;

        /**
         * @brief A stack which keeps track of branch points.
         *
         * Each if statement pushes three nodes on the stack :
         * 1st: next, 2nd: else, 3rd: build.
         * Each consequtive if statement places these three
         * and they are popped on endIfBlock and endElseBlock.
         */
        std::stack<CommandNode> branch_stack;

        /**
         * Save the after-while nodes in a break stack to
         * support the break statement.
         */
        std::stack<CommandNode> break_stack;
    };
}}

#endif


