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

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "execution/CommandParser.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <functional>
#include <algorithm>

#include "corelib/CommandNOP.hpp"
#include "corelib/CommandDataSource.hpp"
#include "corelib/ConditionTrue.hpp"
#include "corelib/ConditionOnce.hpp"
#include "execution/GlobalCommandFactory.hpp"
#include "execution/CommandFactoryInterface.hpp"

#include "execution/AsynchCommandDecorator.hpp"

#include "execution/TaskContext.hpp"
#include "execution/ArgumentsParser.hpp"
#include "execution/ConditionComposite.hpp"
#include "execution/CommandDispatch.hpp"

namespace ORO_Execution
{
    using boost::bind;
    using ORO_CoreLib::CommandNOP;
    using ORO_CoreLib::ConditionTrue;
    using namespace detail;

    namespace {
        assertion<std::string> expect_methodname("Expected a method call on object.");
        assertion<std::string> expect_args( "Expected method call arguments between ()." );
    }
  CommandParser::CommandParser( TaskContext* c, bool dispatch )
      : mdispatch( dispatch ),
        dcom(0), retcommand( 0 ),
        implicittermcondition( 0 ), dispatchCond(0), peer(0), context( c ),
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

    TaskContext* CommandParser::setStack( TaskContext* tc )
    {
        //std::cerr<< "Commands: stack: "<< tc->getName()<<std::endl;
        return expressionparser.setStack( tc );
    }

    TaskContext* CommandParser::setContext( TaskContext* tc )
    {
        //std::cerr<< "Commands: context: "<< tc->getName()<<std::endl;
        context = tc;
        peerparser.setContext(tc);
        peerparser.reset();
        return expressionparser.setContext( tc );
    }

  void CommandParser::seennopcommand()
  {
    retcommand = new CommandNOP;
    implicittermcondition = new ConditionOnce( false );
  }

  void CommandParser::seenstartofcall()
  {
      mcurobject =  peerparser.object();
      peer = peerparser.peer();
      peerparser.reset();

    const GlobalCommandFactory* gcf =
      peer->commands();
    const GlobalMethodFactory* gmf =
      peer->methods();
    const GlobalDataSourceFactory* gdf =
      peer->datasources();
    const CommandFactoryInterface* cfi = gcf->getObjectFactory( mcurobject );
    const MethodFactoryInterface*  mfi = gmf->getObjectFactory( mcurobject );
    const DataSourceFactoryInterface*  dfi = gdf->getObjectFactory( mcurobject );

    // In case the object is not found :
    if ( ! cfi && ! mfi && ! dfi) {
        if ( mcurobject == "this" )
            mcurobject = mcurmethod;
        else
            mcurobject = mcurobject + "." + mcurmethod;
        throw parse_exception_no_such_component( peer->getName(), mcurobject );
    }

    // In case the method/command is not found :
    if ( !( ( cfi && cfi->hasCommand(mcurmethod)) || ( mfi && mfi->hasMember(mcurmethod)) || ( dfi && dfi->hasMember(mcurmethod)) ) )
        throw parse_exception_no_such_method_on_component( mcurobject, mcurmethod );

    // we found it !
    argsparser = new ArgumentsParser( expressionparser, peer,
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

    const GlobalCommandFactory* gcf =
      peer->commands();
    const GlobalMethodFactory* gmf =
      peer->methods();
    const GlobalDataSourceFactory* gdf =
      peer->datasources();
    const CommandFactoryInterface* cfi = gcf->getObjectFactory( mcurobject );
    const MethodFactoryInterface*  mfi = gmf->getObjectFactory( mcurobject );
    const DataSourceFactoryInterface*  dfi = gdf->getObjectFactory( mcurobject );

    // cfi should exist, because otherwise we would have noticed in
    // seenstartofcall()...
    assert( cfi || mfi || dfi);

    ComCon comcon;
    if ( cfi && cfi->hasCommand( mcurmethod ) )
        try
            {
                // check if dispatching is required:
                if ( peer->engine()->commands() != context->engine()->commands() || mdispatch ) {
                    // different, dispatch:
                    comcon = cfi->create( mcurmethod, argsparser->result(), false );
                    dcom = dynamic_cast<DispatchInterface*>( comcon.first );
                    assert( dcom );
                } else {
                    // within same execution engine, no dispatch:
                    comcon = cfi->create( mcurmethod, argsparser->result(), true );
                }
                
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
    else if ( mfi && mfi->hasMember( mcurmethod ) )
        try
            {
                // if the method returns a boolean, construct it as a command
                // which accepts/rejects the result.
                DataSourceBase* dsb =  mfi->create( mcurmethod, argsparser->result() );
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
    else if ( dfi && dfi->hasMember( mcurmethod ) )
        try
            {
                // if the method returns a boolean, construct it as a command
                // which accepts/rejects the result.
                DataSourceBase* dsb =  dfi->create( mcurmethod, argsparser->result() );
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
    else
        assert(false);

    mcurobject.clear();
    mcurmethod.clear();
    delete argsparser;
    argsparser = 0;

    // argumentsparser.result() should have been properly filled up,
    // and mcurobject.mcurmethod should exist, we checked that in
    // seenstartofcall() already, so com should really be valid..
    if ( ! comcon.first )
      throw parse_exception_semantic_error(
        "Something weird went wrong in calling method \"" + mcurmethod +
        "\" on object \"" + mcurobject + "\"." );
    if ( ! comcon.second )
        comcon.second = new ConditionTrue;

    retcommand = comcon.first;
    implicittermcondition = comcon.second;
  }

    ConditionInterface* CommandParser::wrapCondition( ConditionInterface* c )
    {
        if ( peer != context )
            return new ConditionBinaryCompositeAND( dcom->createValidCondition(), c);
        else 
            return c;
    }

    ConditionInterface* CommandParser::dispatchCondition()
    {
        if ( dcom == 0 )
            return 0;
        if ( dispatchCond == 0 )
            dispatchCond = dcom->createValidCondition();
        return dispatchCond;
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
        delete dispatchCond;
        dispatchCond = 0;
        peer = 0;
        dcom = 0;
        retcommand = 0;
        implicittermcondition = 0;
        mcurobject.clear();
        mcurmethod.clear();
      }

}
