/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  ArgumentsParser.cxx

                        ArgumentsParser.cxx -  description
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

#include "parser-debug.hpp"
#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "ArgumentsParser.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "ExpressionParser.hpp"
#include "../TaskContext.hpp"


namespace RTT
{
  using namespace detail;
  using boost::bind;

    namespace {
        boost::spirit::classic::assertion<std::string> expect_open("Open brace expected.");
        boost::spirit::classic::assertion<std::string> expect_close("Closing brace expected ( or could not find out what this line means ).");
        boost::spirit::classic::assertion<std::string> expect_arg("No argument given after comma.");
    }

  ArgumentsParser::ArgumentsParser(
    ExpressionParser& p,  TaskContext* peer, Service::shared_ptr tobject, const std::string& o,
    const std::string& m )
    : mparsed( false ), expressionparser( p ),
      mobject( o ), mmethod( m ), _peer(peer), mtobject(tobject)
  {
    BOOST_SPIRIT_DEBUG_RULE( argument );
    BOOST_SPIRIT_DEBUG_RULE( arguments );

    // a series of arguments; a command-separated list of arguments
    // between parentheses..
    arguments = (
      ch_p('(')
      >> !( argument >> *( ch_p( ',' ) >> expect_arg( argument ) ) )
      >> expect_close(ch_p(')')) )[
        bind( &ArgumentsParser::seenarguments, this ) ];

    // a single argument is just a normal expression..
    argument =
      expressionparser.parser() [
        bind( &ArgumentsParser::seen_arg, this ) ];
  }

  void ArgumentsParser::seen_arg()
  {
    margs.push_back( expressionparser.getResult() );
    expressionparser.dropResult();
  }

  void ArgumentsParser::seenarguments()
  {
    mparsed = true;
  }

  ArgumentsParser::~ArgumentsParser()
  {
    margs.clear();
  }
}
