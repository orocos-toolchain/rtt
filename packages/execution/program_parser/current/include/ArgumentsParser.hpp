#ifndef ARGUMENTSPARSER_HPP
#define ARGUMENTSPARSER_HPP

#include "parser-common.hpp"

#include <corelib/PropertyBag.hpp>
#include "ParseContext.hpp"

namespace ORO_Execution
{
  using ORO_CoreLib::PropertyBagOwner;
  using ORO_CoreLib::PropertyBag;

  /**
   * This is a parser that you construct to parse a set of arguments.
   * It is meant to only be constructed after the objectname and
   * methodname have been parsed.  You should then assign its parser
   * to a rule ( it is one of boost.spirit coolest features that it
   * allows to do this during parsing ), and get the results back
   * after the rule is done..
   * You are supposed to construct a new ArgumentsParser for every
   * method call you see.
   */
  class ArgumentsParser
  {
    // the arguments we have already parsed.  We take a reference to
    // them when they reach us, and release it in the destructor
    std::vector<DataSourceBase*> margs;
    bool mparsed;

    rule_t arguments, argument;

    ParseContext& context;
    ExpressionParser& expressionparser;

    void seen_arg();
    void seenarguments();

    std::string mobject;
    std::string mmethod;
  public:
    /**
     * Construct a new ArgumentsParser.  The object and method
     * parameters are for use in its error messages, and will
     * otherwise not be used..
     */
    ArgumentsParser( ExpressionParser& p, ParseContext& c,
                     const std::string& object, const std::string& method );
    ~ArgumentsParser();

    rule_t& parser()
      {
        return arguments;
      };

    /**
     * Get the parsed DataSource's..
     * ArgumentsParser holds a reference to the returned DataSource's,
     * but releases it in its destructor..  You should take a
     * reference yourself if you're going to be using them after
     * that..
     */
    std::vector<DataSourceBase*> result()
      {
        return margs;
      };
    /**
     * Some syntaxes allow an argument list to be left out, and then
     * this variable is useful to find out whether an argument list
     * was actually parsed or not..
     */
    bool parsed() const
      {
        return mparsed;
      };

    std::string objectname()
      {
        return mobject;
      };
    std::string methodname()
      {
        return mmethod;
      };
  };
}

#endif
