/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  ProgramGraph.hpp

                        ProgramGraph.hpp -  description
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

#ifndef PROGRAMGRAPH_HPP
#define PROGRAMGRAPH_HPP

#include "Processor.hpp"
#include "FunctionGraph.hpp"

#include <utility>                   // for std::pair
#include <stack>
#include <map>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp> // the type of our graph


namespace ORO_Execution
{
    using boost::adjacency_list;
    using boost::property;
    using boost::graph_traits;

    class DataSourceBase;
    class ProgramCommands;
    class TaskContext;

    /**
     * @brief This class represents a program consisting of
     * data contained in a program graph tree, based on the
     * Boost Graph Library.
     */
    class ProgramGraph
        : public FunctionGraph
    {
    public:

        typedef EdgeCondition::EdgeProperty EdgeProperty;
        typedef VertexNode::VertProperty    VertProperty;

        typedef adjacency_list<boost::vecS, boost::listS, boost::directedS, VertProperty, EdgeProperty> Graph;
        typedef graph_traits<Graph>::vertex_descriptor Vertex;
        typedef graph_traits<Graph>::edge_descriptor Edge;

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
         * Constructs an empty program (NOPprogram).
         *
         */
        ProgramGraph(const std::string& _name, TaskContext* tc);

        virtual ~ProgramGraph();

        /**
         * Set the line number of the build command node.
         * @param ln The line number.
         */
        void setLineNumber( int ln );

        void setName(const std::string& _name);

        TaskContext* getTaskContext() const;

        /**
         * Start a new function. Build is the start of the
         * function.
         */
        FunctionGraph* startFunction( const std::string& fname );

        /**
         * Function return is detected inside the function.
         *
         *
         * @param fn The FunctionGraph created earlier with
         *        startFunction().
         */
        void returnFunction( ConditionInterface* cond, FunctionGraph* fn);

        void returnProgram( ConditionInterface* cond );
        /**
         * Function end is detected.
         *
         * @param fn The FunctionGraph created earlier with
         *        startFunction().
         */
        void endFunction( FunctionGraph* fn, int line);

        void startIfStatement( ConditionInterface* cond, int linenumber );
        void endIfBlock(int linenumber);
        void endElseBlock(int linenumber);

        void startWhileStatement( ConditionInterface* cond, int linenumber );
        void endWhileBlock(int linenumber);

        /**
         * Add a new command from the build CommandNode under a condition.
         *
         * @return the 'new' build CommandNode.
         */
        CommandNode addCommand( ConditionInterface* cond,  CommandInterface* com );

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
        void setCommand( CommandInterface* comm );

        /**
         * Get the Command on a given CommandNode.
         *
         */
        CommandInterface* getCommand( CommandNode cn );

        /**
         * Sets a (new) command on a given CommandNode.
         *
         * @param vert The CommandNode to be adapted
         * @param comm The new Command to be executed in that node.
         *
         */
        void setCommand( CommandNode vert, CommandInterface* comm);

        /**
         * A new program is started.
         *
         */
        CommandNode startProgram();

        /**
         * Program end is detected. After this method is called,
         * no more build methods may be called.
         * @param finalCommand The command to run when the program is
         *        finished.  If you pass 0 for this argument ( the
         *        default ), the value new CommandNOP()
         *        will be used.
         */
        void endProgram( CommandInterface* finalCommand, int line);

        /**
         * Append a function to the build CommandNode.
         *
         * @param fn   The Function to append from the build CommandNode
         * @param cond The 'enter' condition
         *
         * @return the last CommandNode of the appended function.
         */
        CommandNode appendFunction( ConditionInterface* cond, FunctionGraph* fn, std::vector<DataSourceBase*> fnargs);

        /**
         * Put a function in the build CommandNode.
         *
         * @param fn   The Function to append from the build CommandNode
         *
         * @return the last CommandNode of the appended function.
         */
        CommandNode setFunction( FunctionGraph* fn, std::vector<DataSourceBase*> fnargs);

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
         * Insert the given command at the front of this program,
         * replacing the build root command, and having it pass to
         * the build root command with a ConditionTrue.
         */
        void prependCommand( CommandInterface* command, int line_nr = 0 );

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

        /**
         * The graph currently working on.
         */
        Graph* graph;

        TaskContext* progcontext;

        ProgramCommands* comms;
        /**
         * @brief A stack which keeps track of branch points.
         *
         * Each if statement pushes three nodes on the stack :
         * 1st: next, 2nd: else, 3rd: build.
         * Each consequtive if statement places these three
         * and they are popped on endIfBlock and endElseBlock.
         */
        std::stack<CommandNode> branch_stack;

    };
}

#endif


