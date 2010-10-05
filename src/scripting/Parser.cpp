/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  Parser.cxx

                        Parser.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

#include "parser-debug.hpp"
#include "parser-types.hpp"
#include "parse_exception.hpp"
#include "Parser.hpp"
#include "ProgramGraphParser.hpp"
#include "StateGraphParser.hpp"
#include "ConditionParser.hpp"
#include "ExpressionParser.hpp"
#include "ValueChangeParser.hpp"
#include "CommandParser.hpp"
#include "ProgramTask.hpp"
#include "StateMachineTask.hpp"
#include "DataSourceCommand.hpp"
#include "ConditionInterface.hpp"

#include <iostream>
#include <fstream>

using namespace boost;

#ifdef WIN32
    #ifdef NDEBUG
        #pragma optimize( "", off)
    #endif
#endif

namespace RTT
{
  using namespace detail;


  Parser::ParsedFunctions Parser::parseFunction( const std::string& text, TaskContext* c, const std::string& filename)
  {
    our_buffer_t function(text);
    our_pos_iter_t parsebegin( function.begin(), function.end(), filename );
    our_pos_iter_t parseend( function.end(), function.end(), filename );
    // The internal parser.
    CommonParser cp;
    ProgramGraphParser gram( parsebegin, c, cp );
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
    ProgramGraphParser gram( parsebegin, c, cp );
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
    StateGraphParser gram( parsebegin, c );
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
    ConditionParser parser( tc, cp );
    bool skipref=true;
    try
    {
      parse( parsebegin, parseend, parser.parser(), SKIP_PARSER );
    }
    catch( const parse_exception& e )
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
        throw parse_exception_parser_fail();
    return ret;
  }

  DataSourceBase::shared_ptr Parser::parseExpression( const std::string& _s,
                                           TaskContext* tc )
  {
    std::string s( _s );

    our_pos_iter_t parsebegin( s.begin(), s.end(), "teststring" );
    our_pos_iter_t parseend( s.end(), s.end(), "teststring" );

    CommonParser cp;
    ExpressionParser parser( tc, cp );
    bool skipref=true;
    try
    {
        parse( parsebegin, parseend, parser.parser(), SKIP_PARSER );
    }
    catch( const parse_exception& e )
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
    throw parse_exception_parser_fail();
  }

  DataSourceBase::shared_ptr Parser::parseValueChange( const std::string& _s,
                                                       TaskContext* tc )
  {
    std::string s( _s );

    our_pos_iter_t parsebegin( s.begin(), s.end(), "teststring" );
    our_pos_iter_t parseend( s.end(), s.end(), "teststring" );

    CommonParser cp;
    ValueChangeParser parser( tc, cp );
    bool skipref=true;
    try
    {
        parse( parsebegin, parseend, parser.variableChangeParser(), SKIP_PARSER );
    }
    catch( const parse_exception& e )
    {
        throw;
    }
    catch( const parser_error<std::string, iter_t>& e )
        {
            // this only happens if input is really wrong.
            throw parse_exception_syntactic_error( e.descriptor );
        }
    if ( parser.assignCommand() ) {
        DataSourceBase::shared_ptr ret = new DataSourceCommand( parser.assignCommand()->clone() );
        parser.reset();
        return ret;
    }
    throw parse_exception_parser_fail();
  }

  std::pair<CommandInterface*, ConditionInterface*>
  Parser::parseCommand( const std::string& _s,
                        TaskContext* tc, bool dodispatch )
  {
    // we need a writable version of the string..
    std::string s( _s );
      // This code is copied from parseCondition

    our_pos_iter_t parsebegin( s.begin(), s.end(), "input" );
    our_pos_iter_t parseend( s.end(), s.end(), "input" );

    CommonParser cp;
    CommandParser parser( tc, cp, !dodispatch );
    bool skipref=true;
    try
    {
      boost_spirit::parse_info<iter_t> ret = parse( parsebegin, parseend, parser.parser(), SKIP_PARSER );
      if ( ! ret.hit )
        throw parse_exception_parser_fail();
    }
    catch( const parse_exception& e )
    {
      // hm, no reason to catch here really
      throw;
    } catch( const parser_error<std::string, iter_t>& e )
        {
            throw parse_exception_syntactic_error( e.descriptor );
        }
    CommandInterface* ret = parser.getCommand();
    ConditionInterface* cond_ret = parser.getImplTermCondition();
    parser.reset();
    return std::make_pair( ret, cond_ret );
  }
}

#ifdef WIN32
    #ifdef NDEBUG
        #pragma optimize( "", on)
    #endif
#endif
