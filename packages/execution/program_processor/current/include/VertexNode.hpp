/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  VertexNode.hpp

                        VertexNode.hpp -  description
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

#ifndef VERTEXNODE_HPP
#define VERTEXNODE_HPP

#include <map>

namespace ORO_CoreLib
{
    class CommandInterface;
    class DataSourceBase;
}


// adjacency_list has some very short template parameter names,
// which may be defined as macros on some OS's. So undef here.
#undef DS	
#undef OEL
#undef VL
#undef VP
#undef EP
#undef GP
#undef EL
#include <boost/graph/adjacency_list.hpp>

namespace ORO_Execution {
    enum vertex_command_t { vertex_command };
    enum vertex_exec_t { vertex_exec };
}

namespace boost {
    using ORO_Execution::vertex_command_t;
    using ORO_Execution::vertex_exec_t;
    BOOST_INSTALL_PROPERTY(vertex, command);
    BOOST_INSTALL_PROPERTY(vertex, exec);
}

namespace ORO_Execution
{
	class ProcessorControlInterface;
	class ProcessorInterface;
    class StateDescription;

	/**
	 * This class represents elements in a program tree.
	 */
  class VertexNode
  {
  public:
      /**
       * The vertex_exec_t property values.
       */
      enum NodeTypes { normal_node, prog_start_node, prog_exit_node, func_start_node, func_exit_node };
      typedef boost::property<vertex_command_t, VertexNode,
                       boost::property<vertex_exec_t, int,
                                boost::property<boost::vertex_index_t, int> > > VertProperty;

      /**
       * Construct an empty program node with no command,
       * no conditional branches and line number 0.
       */
      VertexNode();

      /**
       * Construct a program node with given command,
       * no conditional branches and line number 0.
       */
      explicit VertexNode(ORO_CoreLib::CommandInterface* cmd);

      /**
       * The copy constructor creates a shallow copy.
       */
      VertexNode( const VertexNode& orig );

      VertexNode copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& rdss ) const;

      VertexNode& operator=( const VertexNode& orig );

  public:
      /**
       * Returns a special program node that contains no clauses
       * and a special stopExecution command.
       * (all the leaves of a program tree are this type of program node.
       *
       * @param proc The engine executing this node (which will be stopped)
       * @return A program node with no conditional branches and that
       * contains a stop execution command.
       */
//         static VertexNode* leafNode( ProcessorInterface* proc)
//         {
//             CommandStopExecution* cmd = new CommandStopExecution( proc );
//             return new VertexNode( cmd );
//         }

      ~VertexNode();

        /**
         * This means that the execution engine has just
         * started to execute this node, after some other
         * node has sent it to this one.  If the node is
         * simply executed again, because it returned
         * itself from its execute() method, then this
         * method will not be called.  The reason this is
         * here is because the node has to reset its
         * termination conditions at this point.  See the
         * documentation of ConditionInterface::reset()
         * for more information about this..
         */
         void startExecution();

        /**
         * Executes the command currently associated with this
         * node.
         */
         bool execute();

         /**
          * Returns true if the node is in a valid state to be left.
          */
         bool isValid() const;
        /**
         * Set the command associated with this program to the given command.
         *
         * @param c The new command.
         *
         * @return The previous command (can be null).
         */
         ORO_CoreLib::CommandInterface* setCommand(ORO_CoreLib::CommandInterface* c);

        /**
         * Set line number of this program node to given line number.
         *
         * @param ln The new line number for this node
         * @post The line number of this node equals the given integer.
         */
         void setLineNumber(int ln);

        /**
         * Returns the command currently associated with
         * this node.
         *
         * @return The command currently associated with
         * this node
         */
         ORO_CoreLib::CommandInterface* getCommand() const;

        /**
         * Returns the program line number currently associated
         * with this program node (default = zero).
         */
         int getLineNumber() const;
    private:

        /**
         * The command to be executed in this node.
         */
        ORO_CoreLib::CommandInterface* command;

        /**
         * The line number associated with this node
         * (default = null, also when line numbers are
         *  not employed).
         */
        int lineNumber;

	};
}

#endif



