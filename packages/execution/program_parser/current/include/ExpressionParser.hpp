/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  ExpressionParser.hpp 

                        ExpressionParser.hpp -  description
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
 
#ifndef EXPRESSIONPARSER_HPP
#define EXPRESSIONPARSER_HPP

#include "parser-types.hpp"
#include "CommonParser.hpp"
#include "PeerParser.hpp"
#include "ValueParser.hpp"
#include "DataSource.hpp"
#include "corelib/Time.hpp"

#include <stack>

#pragma interface

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

    void seenmethodname( iter_t begin, iter_t end )
      {
        std::string name( begin, end );
        mmethod = name;
      };

    void seendataname();
    void seendatacall();
    CommonParser commonparser;
    ExpressionParser& expressionparser;
    PeerParser peerparser;
    TaskContext* context;
    std::stack<ArgumentsParser*> argparsers;
  public:
    DataCallParser( ExpressionParser& p, TaskContext* pc );
    ~DataCallParser();

      /**
       * Change the context in which datacalls are 
       * looked up.
       * @return the previous TaskContext.
       */
      TaskContext* setContext( TaskContext* tc);

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
      ifthenelseexp, frameexp, wrenchexp, twistexp, vectorexp, double6Dexp, rotexp, groupexp, atomicexpression,
      constructorexp, framector, wrenchctor, twistctor, vectorctor, double6Dctor, double6Dctor6, rotationctor, time_expression,
      time_spec, indexexp, comma, open_brace, close_brace;

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
    ORO_CoreLib::nsecs nsecs;

    void seen_unary( const std::string& op );
    void seen_binary( const std::string& op );
    void seen_ternary( const std::string& op );
    void seen_sixary( const std::string& op );
    void seenvalue();
    void seendatacall();
    void seentimespec( int n );
    void seentimeunit( iter_t begin, iter_t end );
      void inverttime();

      TaskContext* context;
      DataCallParser datacallparser;
      ValueParser valueparser;
      bool _invert_time;
  public:
    ExpressionParser( TaskContext* pc );
    ~ExpressionParser();

      /**
       * Change the context in which values are 
       * looked up.
       * @return the previous TaskContext.
       */
      TaskContext* setStack( TaskContext* tc);
      TaskContext* setContext( TaskContext* tc);

    rule_t& parser();

    DataSourceBase* getResult();
    // after an expression is parsed, the resultant DataSourceBase will
    // still be on top of the stack, and it should be removed before
    // going back down the parse stack.  This is what this function
    // does..
    void dropResult();

      bool hasResult() { return !parsestack.empty(); }
  };
}

#endif
