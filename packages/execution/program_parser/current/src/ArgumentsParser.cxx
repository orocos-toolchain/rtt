/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  ArgumentsParser.cxx 

                        ArgumentsParser.cxx -  description
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
#pragma implementation
#include "execution/ArgumentsParser.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "execution/ExpressionParser.hpp"
#include "execution/TaskContext.hpp"


namespace ORO_Execution
{
  using boost::bind;

    namespace {
        assertion<std::string> expect_open("Open brace expected.");
        assertion<std::string> expect_close("Closing brace expected ( or could not find out what this line means ).");
        assertion<std::string> expect_arg("No argument given after comma.");
    }

  ArgumentsParser::ArgumentsParser(
    ExpressionParser& p,  TaskContext* peer, const std::string& o,
    const std::string& m )
    : mparsed( false ), expressionparser( p ),
      mobject( o ), mmethod( m ), _peer(peer)
  {
    BOOST_SPIRIT_DEBUG_RULE( argument );
    BOOST_SPIRIT_DEBUG_RULE( arguments );

    // a series of arguments; a command-separated list of arguments
    // between parentheses..
    arguments = (
      "("
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
    margs.back()->ref();
    expressionparser.dropResult();
  }

  void ArgumentsParser::seenarguments()
  {
    mparsed = true;
  };

  ArgumentsParser::~ArgumentsParser()
  {
    // we release our reference to the parsed DataSources..
    std::for_each( margs.begin(), margs.end(),
                   std::mem_fun( &DataSourceBase::deref ) );
    margs.clear();
  };
}
