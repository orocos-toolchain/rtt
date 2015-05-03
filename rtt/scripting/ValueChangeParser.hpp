/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  ValueChangeParser.hpp

                        ValueChangeParser.hpp -  description
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

#ifndef VALUECHANGEPARSER_HPP
#define VALUECHANGEPARSER_HPP

#include "parser-types.hpp"

#include "../Attribute.hpp"
#include "CommonParser.hpp"
#include "ExpressionParser.hpp"
#include "PeerParser.hpp"
#include "PropertyParser.hpp"
#include "../types/Types.hpp"
#include "../Service.hpp"

namespace RTT { namespace scripting
{
  /**
   * This class is responsible for parsing constant definitions,
   * variable definitions, variable change instructions, and alias
   * definitions..  It stores these in the ValueParser in the
   * ParseContext, and parses values using ExpressionParser..
   * @todo The ValueChangeParser.cxx implementation needs refactoring.
   */
  class ValueChangeParser
  {
      // all the AssignVariableCommand we've built..
      // This list is cleared in cleanup().
      std::vector<base::ActionInterface*> assigncommands;
      std::vector<scripting::ConditionInterface*> conditions;

      // the defined values...
      // This list is cleared in cleanup().
      std::vector<base::AttributeBase*> definedvalues;

      // the parsed variable or constant or alias or param
      // definition name.
      // This list is cleared in cleanup().
      std::vector<std::string> definednames;

      // A list of all the names stored in \a context.
      // this list is used to remove them when reset() is
      // called.
      std::vector<std::string> alldefinednames;

    // the name of the value of which we're currently parsing the
    // definition or assignment..
    std::string valuename;

    // A types::TypeInfo of the type that was specified.  We use it to get
    // hold of a Constant or a TaskVariable or ...
    types::TypeInfo* type;

    void seenconstantdefinition();
    void seenaliasdefinition();
    void seenvariabledefinition();
    void seenbaredefinition();
    void seenparamdefinition();
    void storedefinitionname( iter_t begin, iter_t end );
    void seentype( iter_t begin, iter_t end );
    void seensizehint();
    void seenproperty();

    rule_t constantdefinition, aliasdefinition, variabledefinition,
        paramdefinition, baredefinition,
        vardecl, constdecl, baredecl,
        valuechange_parsers;

      TaskContext* context;
      Service::shared_ptr mstore;
      ExpressionParser expressionparser;
      CommonParser& commonparser;

      int sizehint;
      boost::shared_ptr<types::TypeInfoRepository> typerepos;

      /**
       * Delete temporary variables before throwing an exception.
       */
      void cleanup();
  public:
      /**
       * Create a ValueChangeParser which operates and stores values in a task.
       * Use definedvalues() to get the values added to \a tc, use store()
       * to store the added values in another task context as well.
       * After reset(), \a tc will be cleared of all the stored values.
       * \a tc is thus used as a temporary storage container.
       * If you want the new added values in a different storage container, use
       * the \a storage argument. Defaults to tc->provides() if set to null.
       */
      ValueChangeParser( TaskContext* tc, CommonParser& cp,
                         Service::shared_ptr storage,
                         ExecutionEngine* caller);

      /**
       * Clear assignCommands(), definedValues() and
       * definedNames(). Does not delete any variables or commands.
       */
      void clear();

      /**
       * Store allDefinedNames() in a service.
       * This allows you to retrieve all parsed variable
       * declarations.
       */
      void store( Service::shared_ptr other );

      /**
       * Loads all defined names from a service.
       * Just like store(), but works the other way around and
       * allows you to pre-defined some variables. This function
       * removes all these variables from 'source'
       */
      void load( Service::shared_ptr source );

    /**
     * This base::ActionInterface holds the command assigning a value to
     * a variable that should be included in the program.  After a
     * constant definition, variable definition or variable
     * assignment is parsed, you should check it, and include it in
     * your program if it is non-zero.
     */
    base::ActionInterface* assignCommand()
      {
          if ( assigncommands.empty() )
              return 0;
          return assigncommands.back();
      }

    std::vector<base::ActionInterface*> assignCommands()
      {
          return assigncommands;
      }

    std::vector<scripting::ConditionInterface*> assignConditions()
      {
          return conditions;
      }

    base::AttributeBase* lastDefinedValue()
      {
          if ( definedvalues.empty() )
              return 0;
          return definedvalues.back();
      }

    std::vector<base::AttributeBase*> definedValues()
      {
          return definedvalues;
      }

    std::string lastDefinedName()
      {
          if ( definednames.empty() )
              return "";
          return definednames.back();
      }

    std::vector<std::string> definedNames()
      {
          return definednames;
      }

    std::vector<std::string> allDefinedNames()
      {
          return alldefinednames;
      }

    /**
     * Returns the full parser, as it is used most.
     * The individual sub-parsers are below.
     * @return
     */
    rule_t& parser();

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
     * The parser that parses a bare variable definition.
     * These do not get initialised where they are defined, so it is
     * not necessary to check assignCommand() after this...
     */
    rule_t& bareDefinitionParser();

    /**
     * Completely clear all data and erase all parsed
     * definitions from the taskcontext given in the constructor.
     */
    void reset();
  };
}}

#endif
