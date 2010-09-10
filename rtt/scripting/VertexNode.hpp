/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  VertexNode.hpp

                        VertexNode.hpp -  description
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

#ifndef VERTEXNODE_HPP
#define VERTEXNODE_HPP

#include "rtt-scripting-config.h"
#include <map>
#include "../base/ActionInterface.hpp"

// adjacency_list has some very short template parameter names,
// which may be defined as macros on some OS's. So undef here.
#undef DS
#undef OEL
#undef VL
#undef VP
#undef EP
#undef GP
#undef EL
#define BOOST_NO_HASH
#include <boost/graph/adjacency_list.hpp>

namespace RTT
{ namespace scripting {
    enum vertex_command_t { vertex_command };
    enum vertex_exec_t { vertex_exec };
}}

namespace boost {
    using RTT::scripting::vertex_exec_t;
    using RTT::scripting::vertex_command_t;
    BOOST_INSTALL_PROPERTY(vertex, command);
    BOOST_INSTALL_PROPERTY(vertex, exec);
}

namespace RTT
{ namespace scripting {

	/**
	 * This class represents elements in a program tree.
	 */
  class RTT_SCRIPTING_API VertexNode
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
      explicit VertexNode(base::ActionInterface* cmd);

      /**
       * The copy constructor creates a shallow copy.
       */
      VertexNode( const VertexNode& orig );

      VertexNode copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& rdss ) const;

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
         base::ActionInterface* setCommand(base::ActionInterface* c);

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
         base::ActionInterface* getCommand() const;

        /**
         * Returns the program line number currently associated
         * with this program node (default = zero).
         */
         int getLineNumber() const;
    private:

        /**
         * The command to be executed in this node.
         */
        base::ActionInterface* command;

        /**
         * The line number associated with this node
         * (default = null, also when line numbers are
         *  not employed).
         */
        int lineNumber;

	};
}}

#endif



