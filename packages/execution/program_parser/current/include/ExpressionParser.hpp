#ifndef EXPRESSIONPARSER_HPP
#define EXPRESSIONPARSER_HPP

#include "parser-common.hpp"
#include "DataSource.hpp"

#include <stack>

namespace ORO_Execution
{
  /**
   * This parser parses a call of the form
   * "a.b( arg1, arg2, ..., argN )".
   */
  class DataCallParser
  {
    DataSourceBase::shared_ptr ret;
    std::string mobject;
    std::string mmethod;

    rule_t datacall, arguments;

    void seenobjectname( iter_t begin, iter_t end )
      {
        std::string name( begin, end );
        mobject = name;
      };
    void seenmethodname( iter_t begin, iter_t end )
      {
        std::string name( begin, end );
        mmethod = name;
      };
    void seendataname();
    void seendatacall();
    CommonParser commonparser;
    ExpressionParser& expressionparser;
    ParseContext& context;
    std::stack<ArgumentsParser*> argparsers;
  public:
    DataCallParser( ExpressionParser& p, ParseContext& pc );
    ~DataCallParser();

    rule_t& parser()
      {
        return datacall;
      };

    DataSourceBase* getParseResult()
      {
        return ret.get();
      };
  };

  /**
   * How we parse:  this parser works like a stack-based RPN
   * calculator.  An atomic expression pushes one DataSource up the
   * stack, a binary expression pops two DataSources, and pushes a new
   * one, a unary pops one, and pushes one etc.  This allows for the
   * reentrancy we need..
   */
  class ExpressionParser
  {
    rule_t expression, unarynotexp, unaryminusexp, unaryplusexp, multexp,
      divexp, modexp, plusexp, minusexp, smallereqexp, smallerexp,
      greatereqexp, greaterexp, equalexp, notequalexp, orexp, andexp,
      ifthenelseexp, frameexp, vectorexp, rotexp, groupexp, atomicexpression,
      constructorexp, framector, vectorctor, rotationctor, time_expression,
      time_spec;

    /**
     * The parse stack..  see the comment for this class ( scroll up
     * ;) ) for info on the general idea.
     * We keep a reference to the DataSources in here, while they're
     * in here..
     */
    std::stack<DataSourceBase*> parsestack;

    // the name that was parsed as the object to use a certain
    // data of..
    std::string mobjectname;

    // the name that was parsed as the name of the data to use
    // from the object with name mobjectname.
    std::string mpropname;

    // the amount of nanoseconds specified for a ConditionDuration
    int nsecs;

    void seen_unary( const std::string& op );
    void seen_binary( const std::string& op );
    void seen_ternary( const std::string& op );
    void seenvalue();
    void seendatacall();
    void seentimespec( int n );
    void seentimeunit( iter_t begin, iter_t end );

    ParseContext& context;
    DataCallParser datacallparser;
  public:
    ExpressionParser( ParseContext& pc );
    ~ExpressionParser();

    rule_t& parser();

    DataSourceBase* getResult();
    // after an expression is parsed, the resultant DataSourceBase will
    // still be on top of the stack, and it should be removed before
    // going back down the parse stack.  This is what this function
    // does..
    void dropResult();
  };
}

#endif
