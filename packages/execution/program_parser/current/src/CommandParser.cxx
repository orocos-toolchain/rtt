/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  CommandParser.cxx 

                        CommandParser.cxx -  description
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
#include "execution/CommandParser.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <functional>
#include <algorithm>

#include "corelib/CommandNOP.hpp"
#include "corelib/ConditionTrue.hpp"
#include "execution/GlobalCommandFactory.hpp"
#include "execution/CommandFactoryInterface.hpp"

#include "execution/GlobalFactory.hpp"
#include "execution/AsynchCommandDecorator.hpp"

#include "execution/ParseContext.hpp"
#include "execution/ArgumentsParser.hpp"

namespace ORO_Execution
{
  using boost::bind;
  using ORO_CoreLib::CommandNOP;
  using ORO_CoreLib::ConditionTrue;

  CommandParser::CommandParser( ParseContext& c )
    : masync( true ), retcommand( 0 ),
      implicittermcondition( 0 ), context( c ),
      argsparser( 0 ), expressionparser( c )
  {
    BOOST_SPIRIT_DEBUG_RULE( objectmethod );
    BOOST_SPIRIT_DEBUG_RULE( callcommand );
    BOOST_SPIRIT_DEBUG_RULE( nopcommand );
    BOOST_SPIRIT_DEBUG_RULE( command );
    BOOST_SPIRIT_DEBUG_RULE( arguments );

    command = callcommand | nopcommand;

    nopcommand = str_p( "nothing" )[
      bind( &CommandParser::seennopcommand, this ) ];

    // a function call: "x.y( arg1, arg2, ..., argN )"
    // a command can optionally be declared synchronous by
    // putting the word "sync" in front of it..
    callcommand =
         !str_p( "sync" ) [
           bind( &CommandParser::seensync, this ) ]
      >> objectmethod [
           bind( &CommandParser::seenstartofcall, this ) ]
      >> arguments[
           bind( &CommandParser::seencallcommand, this ) ];

    // the "x.y" part of a function call..
    objectmethod =
      lexeme_d[
        commonparser.lexeme_identifier[
          bind( &CommandParser::seenobjectname, this, _1, _2 ) ]
        >> ch_p( '.' )
        >> commonparser.lexeme_identifier[
          bind( &CommandParser::seenmethodname, this, _1, _2 ) ]
        ];
  }

  void CommandParser::seennopcommand()
  {
    retcommand = new CommandNOP;
    implicittermcondition = new ConditionTrue;
  }

  void CommandParser::seenstartofcall()
  {
    const GlobalCommandFactory& gcf =
      context.scriptableextension->commandFactory();
    const CommandFactoryInterface* cfi = gcf.getObjectFactory( mcurobject );
    if ( ! cfi )
      throw parse_exception( "Object \"" + mcurobject +
                             "\" not registered." );
    if ( ! cfi->hasCommand( mcurmethod ) )
      throw parse_exception( "No match for call to \"" + mcurmethod +
                             "\" on object \"" + mcurobject + "\"." );
    argsparser = new ArgumentsParser( expressionparser, context,
                                      mcurobject, mcurmethod );
    arguments = argsparser->parser();
  }

  void CommandParser::seencallcommand()
  {
    // not really necessary, since mcurobject and mcurmethod should
    // still be valid, but anyway :)
    mcurobject = argsparser->objectname();
    mcurmethod = argsparser->methodname();

    const GlobalCommandFactory& gbf =
      context.scriptableextension->commandFactory();
    const CommandFactoryInterface* cfi = gbf.getObjectFactory( mcurobject );
    // cfi should exist, because otherwise we would have noticed in
    // seenstartofcall()...
    assert( cfi );
    assert( cfi->hasCommand( mcurmethod ) );
    ComCon comcon;
    try
    {
      comcon = cfi->create( mcurmethod, argsparser->result() );
    }
    catch( const wrong_number_of_args_exception& e )
    {
      throw parse_exception(
        "Wrong number of arguments in call to \"" + mcurobject + "." +
        mcurmethod + "\":\n Got " + boost::lexical_cast<std::string>( e.received ) + " argument(s), but " +
        boost::lexical_cast<std::string>( e.wanted ) + " needed." );
    }
    catch( const wrong_types_of_args_exception& e )
    {
      throw parse_exception(
        "Wrong type of arg provided for argument " +
        boost::lexical_cast<std::string>( e.whicharg ) + " in call to \"" +
        mcurobject + "." + mcurmethod + "\"." );
    }
    catch( ... )
    {
      assert( false );
    };
    CommandInterface* com = comcon.first;
    ConditionInterface* implcond = comcon.second;

    mcurobject.clear();
    mcurmethod.clear();
    delete argsparser;
    argsparser = 0;

    // argumentsparser.result() should have been properly filled up,
    // and mcurobject.mcurmethod should exist, we checked that in
    // seenstartofcall() already, so com should really be valid..
    if ( ! com )
      throw parse_exception( "Something weird went wrong in calling "
                             "method \"" + mcurmethod +
                             "\" on object \"" + mcurobject + "\"." );

    if ( masync )
      com = new AsynchCommandDecorator( com );

    retcommand = com;
    implicittermcondition = implcond;
  }

  CommandParser::~CommandParser()
  {
    // if argsparser is non-zero, then something went wrong ( someone
    // threw an exception, and we didn't reach seencallcommand() ), so
    // we need to delete the argsparser..
    delete argsparser;
    delete retcommand;
    delete implicittermcondition;
  }
}
