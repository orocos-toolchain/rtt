/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ValueChangeParser.hpp

                        ValueChangeParser.hpp -  description
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

#ifndef VALUECHANGEPARSER_HPP
#define VALUECHANGEPARSER_HPP

#include "parser-types.hpp"

#include "CommonParser.hpp"
#include "ExpressionParser.hpp"

namespace ORO_Execution
{
  /**
   * This class is responsible for parsing constant definitions,
   * variable definitions, variable change instructions, and alias
   * definitions..  It stores these in the ValueParser in the
   * ParseContext, and parses values using ExpressionParser..
   */
  class ValueChangeParser
  {
    // the AssignVariableCommand we've built..
    CommandInterface* assigncommand;

    // the last defined value...
    ParsedValueBase* lastdefinedvalue;

    // the last parsed variable or constant or alias or param
    // definition name
    std::string lastparseddefname;

    // the name of the value of which we're currently parsing the
    // definition or assignment..
    std::string valuename;

    // A TypeInfo of the type that was specified.  We use it to get
    // hold of a ParsedConstantValue or a ParsedVariableValue or ...
    TypeInfo* type;

    void seenconstantdefinition();
    void seenaliasdefinition();
    void seenvariabledefinition();
    void seenparamdefinition();
    void seenvariableassignment();
    void storedefinitionname( iter_t begin, iter_t end );
    void storename( iter_t begin, iter_t end );
    void seentype( iter_t begin, iter_t end );
    void seenindexassignment();

    rule_t constantdefinition, aliasdefinition, variabledefinition,
      variableassignment, paramdefinition;

    TaskContext* context;
    ExpressionParser expressionparser;
    CommonParser commonparser;

    DataSourceBase::shared_ptr index_ds;
  public:
    ValueChangeParser( TaskContext* pc );

    /**
     * This CommandInterface holds the command assigning a value to
     * a variable that should be included in the program.  After a
     * constant definition, variable definition or variable
     * assignment is parsed, you should check it, and include it in
     * your program if it is non-zero.  After you called this
     * function, call reset() !!!
     */
    CommandInterface* assignCommand()
      {
        return assigncommand;
      };

    ParsedValueBase* lastDefinedValue()
      {
        return lastdefinedvalue;
      };

    std::string lastParsedDefinitionName()
      {
        return lastparseddefname;
      };

    /**
     * the parser that parses definitions of constants.  Do not
     * forget to check @ref assignCommand after a constant
     * definition is parsed..
     */
    rule_t& constantDefinitionParser();

    /**
     * the parser that parses variable definitions, don't forget to
     * check @ref assignCommand after a variable definition is
     * parsed..
     */
    rule_t& variableDefinitionParser();

    /**
     * the parser that parses variable assignments, don't forget to
     * check @ref assignCommand after a variable assignment is
     * parsed..
     */
    rule_t& variableAssignmentParser();

    /**
     * The parser that parses alias definitions.  This does not work
     * via an assignment, and it is not necessary to check
     * assignCommand() after this..
     */
    rule_t& aliasDefinitionParser();

    /**
     * The parser that parses state context parameter definitions.
     * These do not get initialised where they are defined, so it is
     * not necessary to check assignCommand() after this...
     */
    rule_t& paramDefinitionParser();

    /**
     * Reset should be called every time after this class parsed
     * something.  You should first check assignCommand, perhaps use
     * it, and then call reset()..
     */
    void reset();
  };
}

#endif
