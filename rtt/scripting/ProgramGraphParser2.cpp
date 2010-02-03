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

namespace RTT
{
  using namespace boost;
  using namespace detail;

    namespace {
        assertion<std::string> expect_ifblock("Expected a statement (or { block } ).");
        assertion<std::string> expect_then("Wrongly formatted \"if ... then\" clause.");
        assertion<std::string> expect_elseblock("Expected a statement (or {block} ) after else.");
        assertion<std::string> expect_ident("Expected a valid identifier.");
    }
    // Work around GCC 4.1 bug: not too much templates in one.cpp file.
  void ProgramGraphParser::setup2()
  {
    // a function statement : "call functionname"
    funcstatement = (
      str_p( "call" )
      >> expect_ident( commonparser.identifier[bind( &ProgramGraphParser::seenfuncidentifier, this, _1, _2) ] )
      >> !arguments[ bind( &ProgramGraphParser::seencallfuncargs, this )]
      )[ bind( &ProgramGraphParser::seencallfuncstatement, this ) ];

    // a return statement : "return"
    returnstatement =
        str_p( "return" )[ bind( &ProgramGraphParser::seenreturnstatement, this ) ];

    // break from a while or for loop,...
    breakstatement =
        str_p( "break" )[ bind (&ProgramGraphParser::seenbreakstatement, this) ];

    catchpart = (str_p("catch") [bind(&ProgramGraphParser::startcatchpart, this)]
                 >> expect_ifblock( ifblock ) )[bind(&ProgramGraphParser::seencatchpart, this)];

    forstatement = ( str_p("for") >> openbrace
                     >> !valuechangeparser.parser()[bind(&ProgramGraphParser::seenforinit, this)] >> semicolon
                     >> condition >> semicolon
                     >> !valuechangeparser.parser()[bind(&ProgramGraphParser::seenforincr, this)] >> closebrace
                     ) [bind(&ProgramGraphParser::seenforstatement, this)]
                                  >> expect_ifblock( ifblock[ bind(&ProgramGraphParser::endforstatement, this) ]);

    ifstatement = (str_p("if")
                   >> condition
                   >> expect_then( str_p("then")[bind(&ProgramGraphParser::seenifstatement, this)] )
                   >> expect_ifblock( ifblock[ bind(&ProgramGraphParser::endifblock, this) ] )
                   >> !( str_p("else") >> expect_elseblock(ifblock) )
                   )[ bind(&ProgramGraphParser::endifstatement, this) ];

    // ifblock is used for a group of statements or one statement (see also whilestatement)
    ifblock = ( ch_p('{') >> *line >> closecurly ) | statement;

    whilestatement =
        (str_p("while")
         >> condition )
        [bind(&ProgramGraphParser::seenwhilestatement, this)]
         >> expect_ifblock( ifblock[ bind(&ProgramGraphParser::endwhilestatement, this) ] );

    continuepart = str_p("continue")[ bind( &ProgramGraphParser::seencontinue, this)];

  }
}

