#ifndef VALUECHANGEPARSER_HPP
#define VALUECHANGEPARSER_HPP

#include "parser-common.hpp"

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
    // the name of the value of which we're currently parsing the
    // definition or assignment..
    std::string valuename;

    // A TypeInfo of the type that was specified.  We use it to get
    // hold of a ParsedConstantValue or a ParsedVariableValue or ...
    TypeInfo* type;

    void seenconstantdefinition();
    void seenaliasdefinition();
    void seenvariabledefinition();
    void seenvariableassignment();
    void storedefinitionname( iter_t begin, iter_t end );
    void storename( iter_t begin, iter_t end );
    void seentype( iter_t begin, iter_t end );

    rule_t constantdefinition, aliasdefinition, variabledefinition,
      variableassignment;

    ParseContext& context;
    ExpressionParser expressionparser;
    CommonParser commonparser;
  public:
    ValueChangeParser( ParseContext& pc );

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
     * Reset should be called every time after this class parsed
     * something.  You should first check assignCommand, perhaps use
     * it, and then call reset()..
     */
    void reset();
  };
}

#endif
