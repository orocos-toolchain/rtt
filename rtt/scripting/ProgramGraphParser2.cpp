/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  ProgramGraphParser.cpp

                        ProgramGraphParser.cpp -  description
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
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#include "parser-debug.hpp"
#include "parse_exception.hpp"
#include "ProgramGraphParser.hpp"

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#ifdef WIN32
    #ifdef NDEBUG
        #pragma optimize( "", off)
    #endif
#endif

namespace RTT
{
  using namespace boost;
  using namespace detail;

    namespace {
        boost::spirit::classic::assertion<std::string> expect_ifblock("Expected a statement (or { block } ).");
        boost::spirit::classic::assertion<std::string> expect_then("Wrongly formatted \"if ... then\" clause.");
        boost::spirit::classic::assertion<std::string> expect_elseblock("Expected a statement (or {block} ) after else.");
        boost::spirit::classic::assertion<std::string> expect_ident("Expected a valid identifier.");
    }
    // Work around GCC 4.1 bug: not too much templates in one.cpp file.
  void ProgramGraphParser::setup2()
  {
    // a function statement : "call functionname"
    funcstatement = (
      lexeme_d[keyword_p( "call" )]
      >> expect_ident( commonparser.identifier[boost::bind( &ProgramGraphParser::seenfuncidentifier, this, _1, _2) ] )
      >> !arguments[ boost::bind( &ProgramGraphParser::seencallfuncargs, this )]
      )[ boost::bind( &ProgramGraphParser::seencallfuncstatement, this ) ];

    // a return statement : "return"
    returnstatement =
        (keyword_p( "return" )[boost::bind(&ProgramGraphParser::noskip_eol, this )]
        >> (  eps_p(commonparser.notassertingeos) | expressionparser.parser()[boost::bind( &ProgramGraphParser::seenreturnvalue, this ) ] )[boost::bind(&ProgramGraphParser::skip_eol, this )])[ boost::bind( &ProgramGraphParser::seenreturnstatement, this ) ];

    // break from a while or for loop,...
    breakstatement =
        keyword_p( "break" )[ boost::bind (&ProgramGraphParser::seenbreakstatement, this) ];

    catchpart = (keyword_p("catch") [boost::bind(&ProgramGraphParser::startcatchpart, this)]
                 >> expect_ifblock( ifblock ) )[boost::bind(&ProgramGraphParser::seencatchpart, this)];

    forstatement = ( keyword_p("for") >> openbrace
                     >> !(valuechangeparser.parser()[boost::bind(&ProgramGraphParser::seenforinit, this)]
                          |
                          expressionparser.parser()[boost::bind(&ProgramGraphParser::seenforinit_expr, this)])>> semicolon
                     >> condition >> semicolon >> !keyword_p("set")
                     >> ( (expressionparser.parser()[boost::bind(&ProgramGraphParser::seenforincr, this)] >> closebrace ) | closebrace[boost::bind(&ProgramGraphParser::seenemptyforincr, this)])
                     ) [boost::bind(&ProgramGraphParser::seenforstatement, this)]
                                  >> expect_ifblock( ifblock[ boost::bind(&ProgramGraphParser::endforstatement, this) ]);

    ifstatement = (keyword_p("if")
                   >> condition
                   >> expect_then( keyword_p("then")[boost::bind(&ProgramGraphParser::seenifstatement, this)] )
                   >> expect_ifblock( ifblock[ boost::bind(&ProgramGraphParser::endifblock, this) ] )
                   >> !( keyword_p("else") >> expect_elseblock(ifblock) )
                   )[ boost::bind(&ProgramGraphParser::endifstatement, this) ];

    // ifblock is used for a group of statements or one statement (see also whilestatement)
    // The semicolon after a single statement is optional, but do not emit an error if there is a semicolon before the else clause.
    ifblock = ( ch_p('{') >> *line >> closecurly ) | ( statement >> !commonparser.notassertingeos );

    whilestatement =
        (keyword_p("while")
         >> condition )
        [boost::bind(&ProgramGraphParser::seenwhilestatement, this)]
         >> expect_ifblock( ifblock[ boost::bind(&ProgramGraphParser::endwhilestatement, this) ] );

    continuepart = keyword_p("continue")[ boost::bind( &ProgramGraphParser::seencontinue, this)];

  }
}

