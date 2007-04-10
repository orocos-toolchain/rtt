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

#include "parser-debug.hpp"
#include "parse_exception.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "CommandParser.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <functional>
#include <algorithm>

#include "CommandNOP.hpp"
#include "CommandDataSource.hpp"
#include "ConditionTrue.hpp"
#include "ConditionOnce.hpp"

#include "AsynchCommandDecorator.hpp"

#include "TaskContext.hpp"
#include "ArgumentsParser.hpp"
#include "ConditionComposite.hpp"
#include "CommandDispatch.hpp"
#include "DispatchAction.hpp"

namespace RTT
{
    using boost::bind;

    using namespace detail;

    namespace {
        assertion<std::string> expect_methodname("Expected a method call on object.");
        assertion<std::string> expect_args( "Expected method call arguments between ()." );
    }
  CommandParser::CommandParser( TaskContext* c, bool as_action)
      : mas_action( as_action ),
        dcom(0), retcommand( 0 ),
        implicittermcondition( 0 ), peer(0), context( c ),
        argsparser( 0 ), expressionparser( c ), peerparser( c )
  {
    BOOST_SPIRIT_DEBUG_RULE( objectmethod );
    BOOST_SPIRIT_DEBUG_RULE( callcommand );
    BOOST_SPIRIT_DEBUG_RULE( nopcommand );
    BOOST_SPIRIT_DEBUG_RULE( command );
    BOOST_SPIRIT_DEBUG_RULE( arguments );

    command = nopcommand | callcommand;

    nopcommand = str_p( "nothing" )[
      bind( &CommandParser::seennopcommand, this ) ];

    // a function call: "x.y( arg1, arg2, ..., argN )"
    // a command can optionally be declared synchronous by
    // putting the word "sync" in front of it..
    callcommand =
        objectmethod [
           bind( &CommandParser::seenstartofcall, this ) ]
      >> expect_args( arguments[
           bind( &CommandParser::seencallcommand, this ) ]);

    // the "x.y" part of a function call..
    // when 'x.' is ommitted, 'this.' is assumed.
    objectmethod = !peerparser.parser()
        >> lexeme_d[ expect_methodname( commonparser.lexeme_identifier[
           bind( &CommandParser::seenmethodname, this, _1, _2 ) ])
        ];
  }

  void CommandParser::seennopcommand()
  {
    retcommand = new CommandNOP;
    implicittermcondition = new ConditionOnce( false );
  }

  void CommandParser::seenstartofcall()
  {
      mcurobject = peerparser.object();
      peer = peerparser.peer();
      OperationInterface* obj = peerparser.taskObject();
      peerparser.reset();

      if ( obj == 0 )
          throw parse_exception_no_such_component( peer->getName(), mcurobject+"."+mcurmethod );
      else {
          if ( obj->methods()->hasMember(mcurmethod) == false && obj->commands()->hasMember(mcurmethod) == false && obj->events()->hasMember(mcurmethod) == false)
              throw parse_exception_no_such_method_on_component( "CommandParser::"+mcurobject, mcurmethod );
      }

      // ok object and method are found.
      argsparser = new ArgumentsParser( expressionparser, peer, obj,
                                        mcurobject, mcurmethod );
      arguments = argsparser->parser();
  }

  void CommandParser::seencallcommand()
  {
      assert( peer );
    // not really necessary, since mcurobject and mcurmethod should
    // still be valid, but anyway :)
    mcurobject = argsparser->objectname();
    mcurmethod = argsparser->methodname();

    OperationInterface* obj = argsparser->object();
    CommandRepository::Factory* cfi = obj->commands();
    MethodRepository::Factory* mfi = obj->methods();
    EventService::Factory* efi = obj->events();

    assert(mfi);
    assert(cfi);
    assert(efi);

    typedef std::pair<CommandInterface*,ConditionInterface*> ComCon;
    ComCon comcon;

    if ( cfi->hasMember( mcurmethod ) )
        try
            {
                dcom = cfi->produce( mcurmethod, argsparser->result() );
                // check if wrapping is required:
                if ( mas_action ) {
                    // wrap it.
                    comcon.first = new DispatchAction(dcom);
                } else {
                    // plain dispatch
                    comcon.first = dcom;
                }
                comcon.second = dcom->createCondition();
            }
        catch( const wrong_number_of_args_exception& e )
            {
                throw parse_exception_wrong_number_of_arguments
                    (mcurobject, mcurmethod, e.wanted, e.received );
            }
        catch( const wrong_types_of_args_exception& e )
            {
                throw parse_exception_wrong_type_of_argument
                    ( mcurobject, mcurmethod, e.whicharg, e.expected_, e.received_ );
            }
        catch( ... )
            {
                assert( false );
            }
    else if ( mfi->hasMember( mcurmethod ) )
        try
            {
                // if the method returns a boolean, construct it as a command
                // which accepts/rejects the result.
                DataSourceBase* dsb =  mfi->produce( mcurmethod, argsparser->result() );
                DataSource<bool>* dsb_res = DataSource<bool>::narrow( dsb );
                if ( dsb_res == 0 )
                    comcon.first =  new CommandDataSource( dsb );
                else
                    comcon.first =  new CommandDataSourceBool( dsb_res );
                comcon.second = new ConditionTrue();
            }
        catch( const wrong_number_of_args_exception& e )
            {
                throw parse_exception_wrong_number_of_arguments
                    (mcurobject, mcurmethod, e.wanted, e.received );
            }
        catch( const wrong_types_of_args_exception& e )
            {
                throw parse_exception_wrong_type_of_argument
                    ( mcurobject, mcurmethod, e.whicharg, e.expected_, e.received_ );
            }
        catch( ... )
            {
                assert( false );
            }
    else if ( efi->hasMember( mcurmethod ) )
        try
            {
                // if the method returns a boolean, construct it as a command
                // which accepts/rejects the result.
                comcon.first  = efi->produce( mcurmethod, argsparser->result() );
                comcon.second = new ConditionTrue();
            }
        catch( const wrong_number_of_args_exception& e )
            {
                throw parse_exception_wrong_number_of_arguments
                    (mcurobject, mcurmethod, e.wanted, e.received );
            }
        catch( const wrong_types_of_args_exception& e )
            {
                throw parse_exception_wrong_type_of_argument
                    ( mcurobject, mcurmethod, e.whicharg, e.expected_, e.received_ );
            }
        catch( ... )
            {
                assert( false );
            }
    else {
        assert( false );
    }

    delete argsparser;
    argsparser = 0;

    // argumentsparser.result() should have been properly filled up,
    // and mcurobject.mcurmethod should exist, we checked that in
    // seenstartofcall() already, so com should really be valid..
    if ( ! comcon.first )
      throw parse_exception_fatal_semantic_error(
        "Something weird went wrong in calling method \"" + mcurmethod +
        "\" on object \"" + mcurobject + "\"." );
    if ( ! comcon.second )
        comcon.second = new ConditionTrue;

    retcommand = comcon.first;
    implicittermcondition = comcon.second;

    mcurobject.clear();
    mcurmethod.clear();
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

    void CommandParser::reset()
      {
        peer = 0;
        dcom = 0;
        retcommand = 0;
        implicittermcondition = 0;
        mcurobject.clear();
        mcurmethod.clear();
      }

}
