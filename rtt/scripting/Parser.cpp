/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  Parser.cxx

                        Parser.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#include "rtt/scripting/parser-debug.hpp"
#include "rtt/scripting/parser-types.hpp"
#include "rtt/scripting/parse_exception.hpp"
#include "rtt/scripting/Parser.hpp"
#include "rtt/scripting/ProgramGraphParser.hpp"
#include "rtt/scripting/StateGraphParser.hpp"
#include "rtt/scripting/ConditionParser.hpp"
#include "rtt/scripting/ExpressionParser.hpp"
#include "rtt/scripting/ValueChangeParser.hpp"
#include "rtt/scripting/ProgramService.hpp"
#include "rtt/scripting/StateMachineService.hpp"
#include "rtt/internal/DataSourceCommand.hpp"
#include "rtt/scripting/ConditionInterface.hpp"
#include "rtt/scripting/CommandComposite.hpp"
#include "rtt/internal/GlobalEngine.hpp"
#include "rtt/scripting/ScriptParser.hpp"

#include <iostream>
#include <fstream>

using namespace boost;

namespace RTT
{
  using namespace detail;

  Parser::Parser(ExecutionEngine* caller) : mcaller(caller) {

      if (mcaller == 0) {
          log(Debug) << "WARNING: Parser does not know which TaskContext is executing (calling) the parsed code. Using Global Engine. Please specify the caller explicitly in order to avoid any asynchronous operation problems." <<endlog();
          mcaller = GlobalEngine::Instance();
      }
  }

  void Parser::runScript(std::string const& code, TaskContext* mowner, ScriptingService* service, std::string const& filename ) {
      our_buffer_t script(code + "\n"); // work around mandatory trailing newline/eos for statements.
      our_pos_iter_t parsebegin( script.begin(), script.end(), filename );
      our_pos_iter_t parseend( script.end(), script.end(), filename );

      // will store all in mowner, functions are loaded in service, caller is the one calling us.
      ScriptParser gram( parsebegin, mowner, mcaller );

      try {
          gram.parse( parsebegin, parseend );
      }
      catch( const parse_exception& exc )
      {
        throw file_parse_exception(
          exc.copy(), parsebegin.get_position().file,
          parsebegin.get_position().line, parsebegin.get_position().column );
      }
  }

  Parser::ParsedFunctions Parser::parseFunction( const std::string& text, TaskContext* c, const std::string& filename)
  {
    our_buffer_t function(text);
    our_pos_iter_t parsebegin( function.begin(), function.end(), filename );
    our_pos_iter_t parseend( function.end(), function.end(), filename );
    // The internal parser.
    CommonParser cp;
    ProgramGraphParser gram( parsebegin, c, c->engine(), cp );
    ParsedFunctions ret = gram.parseFunction( parsebegin, parseend );
    return ret;
  }

  Parser::ParsedPrograms Parser::parseProgram( const std::string& text, TaskContext* c, const std::string& filename)
  {
    our_buffer_t program(text);
    our_pos_iter_t parsebegin( program.begin(), program.end(), filename );
    our_pos_iter_t parseend( program.end(),program.end(),filename );

    // The internal parser.
    CommonParser cp;
    ProgramGraphParser gram( parsebegin, c, c->engine(), cp );
    ParsedPrograms ret = gram.parse( parsebegin, parseend );

    return ret;
  }

  Parser::ParsedStateMachines Parser::parseStateMachine( const std::string& text, TaskContext* c, const std::string& filename)
  {
      // This code is copied from parseProgram()

    our_buffer_t program(text);
    our_pos_iter_t parsebegin( program.begin(), program.end(), filename );
    our_pos_iter_t parseend( program.end(),program.end(),filename );

    // The internal parser.
    CommonParser cp;
    StateGraphParser gram( parsebegin, c, c->engine(), &cp );
    Parser::ParsedStateMachines ret;
    try {
      ret = gram.parse( parsebegin, parseend );
    }
    catch( const parse_exception& exc )
    {
      throw file_parse_exception(
        exc.copy(), parsebegin.get_position().file,
        parsebegin.get_position().line, parsebegin.get_position().column );
    }
    return ret;
  }

  ConditionInterface* Parser::parseCondition( const std::string& s,
                                              TaskContext* tc )
  {
    our_buffer_t scopy(s);
    our_pos_iter_t parsebegin( scopy.begin(), scopy.end(), "teststring" );
    our_pos_iter_t parseend( scopy.end(), scopy.end(), "teststring" );

        CommonParser cp;
    ConditionParser parser( tc, mcaller ? mcaller : tc->engine(), cp );
    bool skipref=true;
    try
    {
      parse( parsebegin, parseend, parser.parser(), SKIP_PARSER );
    }
    catch( const parse_exception& )
    {
      throw;
    }
    catch( const parser_error<std::string, iter_t>& e )
        {
            throw parse_exception_syntactic_error( e.descriptor );
        }
    ConditionInterface* ret = parser.getParseResult();
    parser.reset();
    if ( ret == 0 )
        throw parse_exception_parser_fail("Parser did not find a condition in text.");
    return ret;
  }

  DataSourceBase::shared_ptr Parser::parseExpression( const std::string& _s,
                                           TaskContext* tc )
  {
    std::string s( _s );

    our_pos_iter_t parsebegin( s.begin(), s.end(), "teststring" );
    our_pos_iter_t parseend( s.end(), s.end(), "teststring" );

    CommonParser cp;
    ExpressionParser parser( tc, mcaller, cp );
    bool skipref=true;
    try
    {
        parse( parsebegin, parseend, parser.parser(), SKIP_PARSER );
    }
    catch( const parse_exception& )
    {
        throw;
    }
    catch( const parser_error<std::string, iter_t>& e )
        {
            throw parse_exception_syntactic_error( e.descriptor );
        }
    if ( parser.hasResult() ) {
        DataSourceBase::shared_ptr ret = parser.getResult();
        parser.dropResult();
        return ret;
    }
    throw parse_exception_parser_fail("Parser did not find a valid expression in text.");
  }

  DataSourceBase::shared_ptr Parser::parseValueChange( const std::string& _s,
                                                       TaskContext* tc )
  {
      return parseExpression( _s, tc );
  }

  DataSourceBase::shared_ptr Parser::parseValueStatement( const std::string& _s,
                                                       TaskContext* tc )
  {
    std::string s( _s );

    our_pos_iter_t parsebegin( s.begin(), s.end(), "teststring" );
    our_pos_iter_t parseend( s.end(), s.end(), "teststring" );

    CommonParser cp;
    ValueChangeParser parser( tc, cp, tc->provides(), mcaller ? mcaller : tc->engine() );
    bool skipref=true;
    try
    {
        parse( parsebegin, parseend, parser.parser(), SKIP_PARSER );
    }
    catch( const parse_exception& )
    {
        throw;
    }
    catch( const parser_error<std::string, iter_t>& e )
        {
            // this only happens if input is really wrong.
            throw parse_exception_syntactic_error( e.descriptor );
        }

    ActionInterface* ac = 0;
    std::vector<ActionInterface*> acv = parser.assignCommands();
    // and not forget to reset()..
    if ( acv.empty() && parser.lastDefinedValue() ) {
        return parser.lastDefinedValue()->getDataSource();
    }
    if ( acv.size() == 1 ) {
        if (acv.front() ) { // front will be null if its an alias.
            ac = acv.front();
            ac->readArguments();
            ac->execute();
            delete ac;
        }
        return parser.lastDefinedValue()->getDataSource();
    }
    else if (acv.size() > 1) {
        ac = new CommandComposite(acv);
    }

    if ( ac ) {
        DataSourceBase::shared_ptr ret = new DataSourceCommand( ac );
        //parser.reset(); don't do this, we want to keep it.
        return ret;
    }
    return DataSourceBase::shared_ptr();
  }
}
