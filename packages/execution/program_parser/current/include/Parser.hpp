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

namespace ORO_CoreLib
{
    class StateContext;
}

namespace ORO_Execution
{
    class ProgramGraph;
    class Processor;
    class GlobalFactory;
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
   * This class is the public interface
   */
  class Parser
  {
  public:
    /**
     * Reads out the stream, parses it, and returns a new @ref ProgramGraph ..
     */
      ProgramGraph* parseProgram( std::istream& s, Processor*,
                                  GlobalFactory* );

    /**
     * Reads out the stream, parses it, and returns a new @ref StateContext ..
     */
      std::vector<ParsedStateContext*> parseStateContext( std::istream& s, const std::string& filename, Processor*,
                                                          GlobalFactory*, std::ostream& errorstream );

    /**
     * parses the string as a condition, and returns a new
     * ConditionInterface or 0 on parse failure..  Use ext to get the
     * data from the components..
     */
    ORO_CoreLib::ConditionInterface* parseCondition(
      std::string& s, GlobalFactory* ext );

      /**
       * Parses the command in s. errors are sent down errorstream.
       */
      std::pair<CommandInterface*,ConditionInterface*>
      parseCommand( const std::string&s, GlobalFactory* gFact, std::ostream& errorstream = std::cerr );
  };
};
#endif
