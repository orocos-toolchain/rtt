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

#include "execution/parser-debug.hpp"
#include "execution/parse_exception.hpp"

#pragma implementation
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

    namespace {
        assertion<std::string> expect_methodname("Expected a method call on object.");
        assertion<std::string> expect_args( "Expected method call arguments between ()." );
    }
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

    command = nopcommand | callcommand;

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
      >> expect_args( arguments[
           bind( &CommandParser::seencallcommand, this ) ]);

    // the "x.y" part of a function call..
    objectmethod =
      lexeme_d[
        commonparser.lexeme_identifier[
          bind( &CommandParser::seenobjectname, this, _1, _2 ) ]
        >> ch_p( '.' )
        >> expect_methodname( commonparser.lexeme_identifier[
           bind( &CommandParser::seenmethodname, this, _1, _2 ) ])
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
      context.globalfactory->commandFactory();
    const GlobalMethodFactory& gmf =
      context.globalfactory->methodFactory();
    const CommandFactoryInterface* cfi = gcf.getObjectFactory( mcurobject );
    const MethodFactoryInterface*  mfi = gmf.getObjectFactory( mcurobject );
    if ( ! cfi && ! mfi )
      throw parse_exception_no_such_component( mcurobject );

    // One of both must have the method
    if ( !( ( cfi && cfi->hasCommand(mcurmethod)) || ( mfi && mfi->hasMember(mcurmethod)) ) )
        throw parse_exception_no_such_method_on_component( mcurobject, mcurmethod );
    argsparser = new ArgumentsParser( expressionparser, context,
                                      mcurobject, mcurmethod );
    arguments = argsparser->parser();
  }
    namespace {
        struct CommandDataSource :
            public CommandInterface
        {
            DataSourceBase::shared_ptr _dsb;
            CommandDataSource( DataSourceBase* dsb )
                : _dsb(dsb) {}
            bool execute() {
                _dsb->evaluate();
                return true;
            }
            void reset() {
                _dsb->reset();
            }
            CommandInterface* clone() const {
                return new CommandDataSource( _dsb.get() );
            }

            CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new CommandDataSource( _dsb->copy( alreadyCloned ) );
            }
            
        };
        struct CommandDataSourceBool :
            public CommandInterface
        {
            DataSource<bool>::shared_ptr _dsb;
            CommandDataSourceBool( DataSource<bool>* dsb )
                : _dsb(dsb) {}
            bool execute() {
                return _dsb->get();
            }
            void reset() {
                _dsb->reset();
            }
            CommandInterface* clone() const {
                return new CommandDataSourceBool( _dsb.get() );
            }

            CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new CommandDataSourceBool( _dsb->copy( alreadyCloned ) );
            }
            
        };
    }

  void CommandParser::seencallcommand()
  {
    // not really necessary, since mcurobject and mcurmethod should
    // still be valid, but anyway :)
    mcurobject = argsparser->objectname();
    mcurmethod = argsparser->methodname();

    const GlobalCommandFactory& gcf =
      context.globalfactory->commandFactory();
    const GlobalMethodFactory& gmf =
      context.globalfactory->methodFactory();
    const CommandFactoryInterface* cfi = gcf.getObjectFactory( mcurobject );
    const MethodFactoryInterface*  mfi = gmf.getObjectFactory( mcurobject );

    // cfi should exist, because otherwise we would have noticed in
    // seenstartofcall()...
    assert( cfi || mfi );

    ComCon comcon;
    if ( cfi && cfi->hasCommand( mcurmethod ) )
        try
            {
                comcon = cfi->create( mcurmethod, argsparser->result() );
            }
        catch( const wrong_number_of_args_exception& e )
            {
                throw parse_exception_wrong_number_of_arguments
                    (mcurobject, mcurmethod, e.wanted, e.received );
            }
        catch( const wrong_types_of_args_exception& e )
            {
                throw parse_exception_wrong_type_of_argument
                    ( mcurobject, mcurmethod, e.whicharg );
            }
        catch( ... )
            {
                assert( false );
            }
    else if ( mfi && mfi->hasMember( mcurmethod ) )
        try
            {
                // if the method returns a boolean, construct it as a command
                // which accepts/rejects the result.
                DataSourceBase* dsb =  mfi->create( mcurmethod, argsparser->result() );
                DataSource<bool>* dsb_res =  dynamic_cast< DataSource<bool>* >( dsb );
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
                    ( mcurobject, mcurmethod, e.whicharg );
            }
        catch( ... )
            {
                assert( false );
            }
    else
        assert(false);

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
      throw parse_exception_semantic_error(
        "Something weird went wrong in calling method \"" + mcurmethod +
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
