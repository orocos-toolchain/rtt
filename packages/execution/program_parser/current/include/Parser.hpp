/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  Parser.hpp

                        Parser.hpp -  description
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

#ifndef PROGRAMPARSER_HPP
#define PROGRAMPARSER_HPP

#include <iosfwd>
#include <utility>
#include <map>
#include <string>
#include "parse_exception.hpp"

namespace ORO_CoreLib
{
    class StateContext;
}

namespace ORO_Execution
{
    class ProgramGraph;
    class TaskContext;
    class ParsedStateContext;
}

namespace ORO_CoreLib
{
  class PropertyBase;
  class CommandInterface;
  class ConditionInterface;
}

namespace ORO_Execution
{
  using ORO_CoreLib::StateContext;
  using ORO_CoreLib::PropertyBase;
  using ORO_CoreLib::CommandInterface;
  using ORO_CoreLib::ConditionInterface;
  /**
   * @brief This class is the public interface to the
   * Orocos Program Parser Framework.
   */
  class Parser
  {
  public:
    /**
     * @brief Reads out the stream, parses it, and returns a new @ref
     * ProgramGraph..
     * @throw parse_exception Throws exceptions of type parse_exception.
     */
      std::vector<ProgramGraph*> parseProgram( std::istream& s, TaskContext* );

    /**
     * @brief Reads out the stream, parses it, and returns a new @ref
     * StateContext ..
     * @throw file_parse_exception Throws exceptions of type file_parse_exception.
     */
      std::vector<ParsedStateContext*> parseStateContext(std::istream& s, const std::string& filename, TaskContext* );

    /**
     * @brief Parses the string as a condition, and returns a new
     * ConditionInterface..  Will throw parse_exception on
     * failure... Use ext to get the data from the components..
     * @throw parse_exception
     */
    ORO_CoreLib::ConditionInterface* parseCondition(
      std::string& s, TaskContext* );

      /**
       * @brief Parses the command in s.
       * @throw parse_exception Throws exceptions of type parse_exception.
       */
      std::pair<CommandInterface*,ConditionInterface*>
      parseCommand( const std::string&s, TaskContext* );
  };
};
#endif
