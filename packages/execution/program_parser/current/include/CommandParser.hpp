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
