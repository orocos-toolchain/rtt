/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  CommandParser.hpp 

                        CommandParser.hpp -  description
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
 
#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

#include "parser-common.hpp"
#include <corelib/PropertyBag.hpp>
#include "ExpressionParser.hpp"

namespace ORO_Execution
{
  using ORO_CoreLib::PropertyBagOwner;

  /**
   * This class parses commands.  Actually, it only parses call
   * commands, and the keyword 'nothing'.  Value Set Commands are
   * parsed by the ValueChangeParser.  It also takes care of checking
   * whether a command was specified as synchronous ( by putting the
   * keyword sync before it ), and otherwise wraps the generated
   * command in an AsynchCommandDecorator.
   */
  class CommandParser
  {
    // the object that the user wants to call a method on, in the
    // current call command..
    std::string mcurobject;

    // the method the user wants to call in the current call command..
    std::string mcurmethod;

    // whether or not the current command was specified as
    // asynchronous..
    bool masync;

    CommandInterface* retcommand;
    ConditionInterface* implicittermcondition;

    void seenobjectname( iter_t begin, iter_t end )
      {
        std::string objname( begin, end );
        mcurobject = objname;
      };

    void seenmethodname( iter_t begin, iter_t end )
      {
        std::string methodname( begin, end );
        mcurmethod = methodname;
      };

    void seenstartofcall();
    void seennopcommand();
    void seensync()
      {
        masync = false;
      }

    void seencallcommand();

    rule_t objectmethod, command, callcommand, nopcommand, arguments;

    ParseContext& context;
    CommonParser commonparser;
    ArgumentsParser* argsparser;
    ExpressionParser expressionparser;

  public:
    CommandParser( ParseContext& context );
    ~CommandParser();

    rule_t& parser()
      {
        return command;
      };

    /**
     * Get the parsed command.  Call the reset method if you use the
     * generated command, otherwise it will be deleted in the
     * CommandParser's destructor..
     */
    CommandInterface* getCommand()
      {
        return retcommand;
      }

    /**
     * The implicit termination condition associated with the command,
     * if any.  Ownership semantics are the same as for the
     * getCommand() function.
     */
    ConditionInterface* getImplTermCondition()
      {
        return implicittermcondition;
      }

    /**
     * resets the CommandParser, use this after you've succesfully
     * used the created command and implicit termination condition.
     */
    void reset()
      {
        retcommand = 0;
        implicittermcondition = 0;
        masync = true;
      }
  };
}

#endif
