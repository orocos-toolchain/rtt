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

#include "execution/parser-debug.hpp"
#include "execution/parser-types.hpp"
#include "execution/parse_exception.hpp"
#include "execution/Parser.hpp"
#include "execution/ProgramGraphParser.hpp"
#include "execution/StateGraphParser.hpp"
#include "execution/ConditionParser.hpp"
#include "execution/ExpressionParser.hpp"
#include "execution/CommandParser.hpp"
#include "corelib/ConditionInterface.hpp"

#include <iostream>
#include <fstream>

using namespace boost;

namespace ORO_Execution
{

  std::vector<FunctionGraph*> Parser::parseFunction( const std::string& file, TaskContext* c)
  {
      std::ifstream inputfile(file.c_str());
      return this->parseFunction( inputfile, c, file );
  }

  std::vector<FunctionGraph*> Parser::parseFunction( std::istream& s, TaskContext* c, const std::string& filename)
  {
    our_buffer_t function;

    s.unsetf( std::ios_base::skipws );

    std::istream_iterator<char> streambegin( s );
    std::istream_iterator<char> streamend;
    std::copy( streambegin, streamend, std::back_inserter( function ) );

    our_pos_iter_t parsebegin( function.begin(), function.end(), filename );
    our_pos_iter_t parseend; // not used.

    // The internal parser.
    ProgramGraphParser gram( parsebegin, c );
    std::vector<FunctionGraph*> ret = gram.parseFunction( parsebegin, parseend );
    return ret;
  }

  std::vector<ProgramGraph*> Parser::parseProgram( const std::string& file, TaskContext* c)
  {
      std::ifstream inputfile(file.c_str());
      return this->parseProgram( inputfile, c, file );
  }

  std::vector<ProgramGraph*> Parser::parseProgram( std::istream& s, TaskContext* c, const std::string& filename)
  {
    our_buffer_t program;

    s.unsetf( std::ios_base::skipws );

    std::istream_iterator<char> streambegin( s );
    std::istream_iterator<char> streamend;
    std::copy( streambegin, streamend, std::back_inserter( program ) );

    our_pos_iter_t parsebegin( program.begin(), program.end(), filename );
    our_pos_iter_t parseend; // not used.

    // The internal parser.
    ProgramGraphParser gram( parsebegin, c );
    std::vector<ProgramGraph*> ret = gram.parse( parsebegin, parseend );

    return ret;
  }

  std::vector<ParsedStateMachine*> Parser::parseStateMachine( const std::string& file, TaskContext* c)
  {
      std::ifstream inputfile(file.c_str());
      return this->parseStateMachine( inputfile, c, file );
  }

  std::vector<ParsedStateMachine*> Parser::parseStateMachine( std::istream& s, TaskContext* c, const std::string& filename)
  {
      // This code is copied from parseProgram()

    our_buffer_t program;

    s.unsetf( std::ios_base::skipws );

    std::istream_iterator<char> streambegin( s );
    std::istream_iterator<char> streamend;
    std::copy( streambegin, streamend, std::back_inserter( program ) );

    our_pos_iter_t parsebegin( program.begin(), program.end(), filename );
    our_pos_iter_t parseend;

    // The internal parser.
    StateGraphParser gram( parsebegin, c );
    std::vector<ParsedStateMachine*> ret;
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

  ConditionInterface* Parser::parseCondition( std::string& s,
                                              TaskContext* tc )
  {
    our_pos_iter_t parsebegin( s.begin(), s.end(), "teststring" );
    our_pos_iter_t parseend;

    ConditionParser parser( tc );
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
    return ret;
  }

  DataSourceBase::shared_ptr Parser::parseExpression( const std::string& _s,
                                           TaskContext* tc )
  {
    std::string s( _s );

    our_pos_iter_t parsebegin( s.begin(), s.end(), "teststring" );
    our_pos_iter_t parseend;

    ExpressionParser parser( tc );
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
    throw parse_exception_syntactic_error("No expression found");
  }

  std::pair<CommandInterface*, ConditionInterface*>
  Parser::parseCommand( const std::string& _s,
                        TaskContext* tc )
  {
    // we need a writable version of the string..
    std::string s( _s );
      // This code is copied from parseCondition

    our_pos_iter_t parsebegin( s.begin(), s.end(), "input" );
    our_pos_iter_t parseend;

    CommandParser parser( tc );
    try
    {
      boost::spirit::parse_info<iter_t> ret = parse( parsebegin, parseend, parser.parser(), SKIP_PARSER );
      if ( ! ret.hit )
        throw parse_exception_syntactic_error( "No command found" );
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
