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

#include "ProgramInterface.hpp"
#include "ProcessorInterface.hpp"

#include "VertexNode.hpp"
#include "EdgeCondition.hpp"

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

    class FunctionGraph;
    class DataSourceBase;

    /**
     * @brief This class represents a program consisting of
     * data contained in a program graph tree, based on the
     * Boost Graph Library.
     */
    class ProgramGraph
        : public ProgramInterface
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
        typedef Vertex CommandNode ;

        /**
         * A ConditionEdge serves as a token to construct
         * an edge, containing a condition.
         */
        typedef Edge ConditionEdge ;

        /**
         * Constructs an empty program (NOPprogram).
         *
         */
        ProgramGraph(const std::string& _name="Default");

        void debugPrintout() const;

        virtual ~ProgramGraph();

        virtual void execute();

        virtual void executeToStop();

        virtual void reset();

        virtual bool isFinished() const;

        /**
         * Clone this ProgramGraph.  This will produce a completely
         * new ProgramGraph, that has nothing in common with this one.
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
        ProgramGraph* copy( std::map<const DataSourceBase*, DataSourceBase*>& replacementdss ) const;

        virtual int  getLineNumber() const;

        virtual const std::string& getName() const;

        /**
         * Set the name of this program.
         * Only valid before endProgram() is called.
         */
        void setName(const std::string& _name);

        /**
         * Set the line number of the current command node.
         * @param ln The line number.
         */
        void setLineNumber( int ln );

        /**
         * Set the program text.
         */
        void setText( const std::string& t);

        std::string getText() const;

        /**
         * Returns the Graph of the program.
         */
        const Graph& getGraph() const;

        /**
         * Start a new function. Current is the start of the
         * function.
         */
        FunctionGraph* startFunction();

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
        void endFunction( FunctionGraph* fn);

        void startIfStatement( ConditionInterface* cond, int linenumber );
        void endIfBlock(int linenumber);
        void endElseBlock(int linenumber);

        void startWhileStatement( ConditionInterface* cond, int linenumber );
        void endWhileBlock(int linenumber);

        /**
         * Add a new command from the current CommandNode under a condition.
         *
         * @return the 'new' current CommandNode.
         */
        CommandNode addCommand( ConditionInterface* cond,  CommandInterface* com );

        /**
         * Add an edge from the current CommandNode to the given CommandNode
         * without changing the current CommandNode.
         *
         */
        void addConditionEdge( ConditionInterface* cond, CommandNode vert );

        /**
         * Add an edge between the given CommandNode and the current CommandNode.
         *
         */
        void closeConditionEdge( CommandNode vert, ConditionInterface* cond );

        /**
         * Select an already added CommandNode.
         *
         * @return the previous CommandNode.
         */
        CommandNode moveTo( CommandNode _current, CommandNode _next, int linenr );

        /**
         * Sets a new Command on the current CommandNode.
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
        void endProgram( CommandInterface* finalCommand = 0);

        /**
         * Append a function to the current CommandNode.
         *
         * @param fn   The Function to append from the current CommandNode
         * @param cond The 'enter' condition
         *
         * @return the last CommandNode of the appended function.
         */
        CommandNode appendFunction( ConditionInterface* cond, FunctionGraph* fn);

        /**
         * Put a function in the current CommandNode.
         *
         * @param fn   The Function to append from the current CommandNode
         *
         * @return the last CommandNode of the appended function.
         */
        CommandNode setFunction( FunctionGraph* fn);

        /**
         * Proceed to the 'next' CommandNode.
         *
         * @param line_nr The line number of the 'current' command.
         * @return The new current CommandNode.
         */
        CommandNode proceedToNext( int line_nr = 0 );

        /**
         * Proceed to the 'next' CommandNode and add an edge
         * with a condition.
         *
         * @param cond The condition under which to proceed to the next node.
         * @param line_nr The line number of the 'current' command.
         * @return The new current CommandNode.
         */
        CommandNode proceedToNext( ConditionInterface* cond, int line_nr = 0 );

        /**
         * Connect the given CommandNode to the 'next' CommandNode.
         */
        void connectToNext( CommandNode v, ConditionInterface* cond );

        /**
         * Insert the given command at the front of this program,
         * replacing the current root command, and having it pass to
         * the current root command with a ConditionTrue.
         */
        void prependCommand( CommandInterface* command, int line_nr = 0 );

        /**
         * Return the current CommandNode.
         */
        CommandNode currentNode() const;

        /**
         * Return the number of edges of the current CommandNode.
         */
        size_t currentEdges() const;

        /**
         * Return the next CommandNode.
         */
        CommandNode nextNode() const;

        void setLabel( const std::string& str );

        bool hasLabel( const std::string& str ) const;

        std::pair<CommandNode,bool> findNode( const std::string& str ) const;

    private:
        /**
         * The graph containing all the program nodes.
         */
        Graph program;

        /**
         * The graph currently working on.
         */
        Graph* graph;

        /**
         * The node which is built now
         */
        CommandNode current;

        /**
         * The node which will be built next.
         */
        CommandNode next;

        /**
         * The node that was run before this one.
         */
        CommandNode previous;

        /**
         * The r00t of all evil
         * (thus the start of the program).
         */
        CommandNode root;

        /**
         * The end of all evil
         * (thus the end of the program).
         */
        CommandNode end;

        /**
         * The (unique) name of this program.
         */
        std::string myName;

        /**
         * Program text.
         */
        std::string _text;

        /**
         * @brief A stack which keeps track of branch points.
         *
         * Each if statement pushes three nodes on the stack :
         * 1st: next, 2nd: else, 3rd: current.
         * Each consequtive if statement places these three
         * and they are popped on endIfBlock and endElseBlock.
         */
        std::stack<CommandNode> branch_stack;
    };
}

#endif


