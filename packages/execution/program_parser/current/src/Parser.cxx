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
#include "execution/CommandParser.hpp"
#include "corelib/ConditionInterface.hpp"

#include <iostream>

using namespace boost;

namespace ORO_Execution
{

  ProgramGraph* Parser::parseProgram( std::istream& s, Processor* proc,
                                 const GlobalFactory* ext )
  {
    our_buffer_t program;

    s.unsetf( std::ios_base::skipws );

    std::istream_iterator<char> streambegin( s );
    std::istream_iterator<char> streamend;
    std::copy( streambegin, streamend, std::back_inserter( program ) );

    our_pos_iter_t parsebegin( program.begin(), program.end(), "teststring" );
    our_pos_iter_t parseend;

    // The internal parser.
    ProgramGraphParser gram( parsebegin, proc, ext );
    ProgramGraph* ret = gram.parse( parsebegin, parseend );
    if ( ret )
      std::cerr << "Program Parsed Successfully !" << std::endl;
    return ret;
  };

  StateContext* Parser::parseStateContext( std::istream& s, Processor* proc,
                                           const GlobalFactory* ext )
  {
      // This code is copied from parseProgram()

    our_buffer_t program;

    s.unsetf( std::ios_base::skipws );

    std::istream_iterator<char> streambegin( s );
    std::istream_iterator<char> streamend;
    std::copy( streambegin, streamend, std::back_inserter( program ) );

    our_pos_iter_t parsebegin( program.begin(), program.end(), "teststring" );
    our_pos_iter_t parseend;

    // The internal parser.
    StateGraphParser gram( parsebegin, proc, ext );
    StateGraph* ret = gram.parse( parsebegin, parseend );
    if ( ret )
      std::cerr << "State Parsed Successfully !" << std::endl;
    return ret;
  };

  ConditionInterface* Parser::parseCondition( std::string& s,
                                              const GlobalFactory* e )
  {
    our_pos_iter_t parsebegin( s.begin(), s.end(), "teststring" );
    our_pos_iter_t parseend;

    // TODO: get a processor from somewhere, even though the condition
    // parser doesn't really need it..
    ParseContext pc( 0, e );
    ConditionParser parser( pc );
    try
    {
      parse( parsebegin, parseend, parser.parser(), SKIP_PARSER );
    }
    catch( const parse_exception& e )
    {
      std::cerr << "Parse error at line "
                << parsebegin.get_position().line
                << ": " << e.what() << std::endl;
      return 0;
    };
    ConditionInterface* ret = parser.getParseResult();
    parser.reset();
    return ret;
  };

  std::pair<CommandInterface*, ConditionInterface*>
  Parser::parseCommand( std::string& s,
                        const GlobalFactory* e )
  {
      // This code is copied from parseCondition

    our_pos_iter_t parsebegin( s.begin(), s.end(), "teststring" );
    our_pos_iter_t parseend;

    ParseContext pc( 0, e );
    CommandParser parser( pc );
    try
    {
      parse( parsebegin, parseend, parser.parser(), SKIP_PARSER );
    }
    catch( const parse_exception& e )
    {
      std::cerr << "Parse error at line "
                << parsebegin.get_position().line
                << ": " << e.what() << std::endl;
      return std::pair<CommandInterface*,ConditionInterface*>(0,0);
    };
    CommandInterface* ret = parser.getCommand();
    ConditionInterface* cond_ret = parser.getImplTermCondition();
    parser.reset();
    return std::make_pair( ret, cond_ret );
  };
}
