/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:25 CET 2004  ExpressionParser.hpp

                        ExpressionParser.hpp -  description
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

#ifndef EXPRESSIONPARSER_HPP
#define EXPRESSIONPARSER_HPP

#include "parser-types.hpp"
#include "CommonParser.hpp"
#include "PeerParser.hpp"
#include "ValueParser.hpp"
#include "SendHandleAlias.hpp"
#include "../internal/DataSource.hpp"
#include "../types/Operators.hpp"
#include "../Time.hpp"

#include <stack>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT { namespace scripting
{
  /**
   * This parser parses a call of the form
   * "a.b( arg1, arg2, ..., argN )".
   *
   * @todo check why lexeme_d[] is used in implementation,
   * thus why datacalls are parsed on the character level
   * instead of on the phrase level. (probably for the dots ?)
   */
  class DataCallParser
  {
    base::DataSourceBase::shared_ptr ret;
    boost::shared_ptr<SendHandleAlias> mhandle;
    ConditionInterface* mcmdcnd;
    std::string mobject;
    std::string mmethod;
    ExecutionEngine* mcaller;
    enum CallType { DEFAULT_CALLTYPE, CALLTYPE_CALL, CALLTYPE_SEND, CALLTYPE_CMD } mcalltype;

    rule_t datacall, arguments, peerpath, object, method;

    void seenmethodname( iter_t begin, iter_t end );
    void seenobjectname( iter_t begin, iter_t end );
    void seendataname();
    void seendatacall();

    CommonParser& commonparser;
    ExpressionParser& expressionparser;
    PeerParser peerparser;
    std::stack<ArgumentsParser*> argparsers;
  public:
    DataCallParser( ExpressionParser& p, CommonParser& cp, TaskContext* pc, ExecutionEngine* caller );
    ~DataCallParser();

    rule_t& parser()
      {
        return datacall;
      }

    base::DataSourceBase* getParseResult()
      {
        return ret.get();
      }
    ConditionInterface* getParseCmdResult()
      {
        return mcmdcnd;
      }
    boost::shared_ptr<SendHandleAlias> getParseHandle()
      {
        return mhandle;
      }
  };

  /**
   * Parses type constructor syntax
   */
  class ConstructorParser
  {
    base::DataSourceBase::shared_ptr ret;
    rule_t type_name, arguments;

    CommonParser& commonparser;
    ExpressionParser& expressionparser;
    std::stack<ArgumentsParser*> argparsers;
  public:
    ConstructorParser( ExpressionParser& p, CommonParser& cp);
    ~ConstructorParser();

    void seen_type_name( iter_t begin, iter_t end );
    void seen_constructor( void );

    rule_t& parser()
      {
        return type_name;
      }

    base::DataSourceBase* getParseResult()
      {
        return ret.get();
      }
  };

  /**
   * How we parse:  this parser works like a stack-based RPN
   * calculator.  An atomic expression pushes one internal::DataSource up the
   * stack, a binary expression pops two DataSources, and pushes a new
   * one, a unary pops one, and pushes one etc.  This allows for the
   * reentrancy we need..
   */
  class ExpressionParser
  {
    rule_t expression, unarynotexp, unaryminusexp, unaryplusexp, div_or_mul,
      modexp, plus_or_min, smallereqexp, smallerexp,
      greatereqexp, greaterexp, equalexp, notequalexp, orexp, andexp,
      ifthenelseexp, dotexp, groupexp, atomicexpression,
      time_expression, time_spec, indexexp, comma, close_brace,
      value_expression, call_expression, assignexp, constructor_expression;

    /**
     * The parse stack..  see the comment for this class ( scroll up
     * ;) ) for info on the general idea.
     * We keep a reference to the DataSources in here, while they're
     * in here..
     */
    std::stack<base::DataSourceBase::shared_ptr> parsestack;
    /**
     * Contains the last SendHandle encountered, Will also be dropped
     * by dropResult().
     */
    boost::shared_ptr<SendHandleAlias> mhandle;

    ConditionInterface* mcmdcnd;

    // the name that was parsed as the object to use a certain
    // data of..
    std::string mobjectname;

    // the name that was parsed as the name of the data to use
    // from the object with name mobjectname.
    std::string mpropname;

    // time specification
    nsecs tsecs;

    void seen_unary( const std::string& op );
    void seen_binary( const std::string& op );
    void seen_index();
    void seen_assign();
    void seen_dotmember( iter_t begin, iter_t end );
    void seenvalue();
    void seendatacall();
    void seenconstructor();
    void seentimespec( int n );
    void seentimeunit( iter_t begin, iter_t end );
      void inverttime();
      void seentimeexpr();

      DataCallParser datacallparser;
      ConstructorParser constrparser;
      /**
       * The governing common parser.
       */
      CommonParser& commonparser;
      ValueParser valueparser;
      bool _invert_time;
      types::OperatorRepository::shared_ptr opreg;

      TaskContext* context;
  public:
      ExpressionParser( TaskContext* pc, ExecutionEngine* caller, CommonParser& common_parser );
    ~ExpressionParser();

    rule_t& parser();

    base::DataSourceBase::shared_ptr getResult();
    ConditionInterface* getCmdResult();

    /**
     * In case the parsed result returns a SendHandle,
     * ask the parser to also create a handle for it.
     * @return null pointer if no handle could be made.
     */
    boost::shared_ptr<base::AttributeBase> getHandle();

    // after an expression is parsed, the resultant base::DataSourceBase will
    // still be on top of the stack, and it should be removed before
    // going back down the parse stack.  This is what this function
    // does..
    void dropResult();

      bool hasResult() { return !parsestack.empty(); }
  };
}}

#endif
