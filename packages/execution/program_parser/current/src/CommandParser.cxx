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
#include "corelib/ConditionOnce.hpp"
#include "execution/GlobalCommandFactory.hpp"
#include "execution/CommandFactoryInterface.hpp"

#include "execution/AsynchCommandDecorator.hpp"

#include "execution/TaskContext.hpp"
#include "execution/ArgumentsParser.hpp"
#include "execution/ConditionComposite.hpp"
#include "execution/TryCommand.hpp"
#include "execution/CommandDispatch.hpp"

namespace ORO_Execution
{
  using boost::bind;
  using ORO_CoreLib::CommandNOP;
  using ORO_CoreLib::ConditionTrue;

    namespace {
        assertion<std::string> expect_methodname("Expected a method call on object.");
        assertion<std::string> expect_args( "Expected method call arguments between ()." );
    }
  CommandParser::CommandParser( TaskContext* c )
    : masync( true ),  tcom(0), retcommand( 0 ),
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
         !str_p( "sync" ) [
           bind( &CommandParser::seensync, this ) ]
      >> objectmethod [
           bind( &CommandParser::seenstartofcall, this ) ]
      >> expect_args( arguments[
           bind( &CommandParser::seencallcommand, this ) ]);

    // the "x.y" part of a function call..
    // when 'x.' is ommitted, 'this.' is assumed.
    objectmethod = peerparser.parser()
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

    const GlobalCommandFactory& gcf =
      peer->commandFactory;
    const GlobalMethodFactory& gmf =
      peer->methodFactory;
    const CommandFactoryInterface* cfi = gcf.getObjectFactory( mcurobject );
    const MethodFactoryInterface*  mfi = gmf.getObjectFactory( mcurobject );
    if ( ! cfi && ! mfi )
      throw parse_exception_no_such_component( peer->getName()+"."+mcurobject, mcurmethod );

    // One of both must have the method
    if ( !( ( cfi && cfi->hasCommand(mcurmethod)) || ( mfi && mfi->hasMember(mcurmethod)) ) )
        throw parse_exception_no_such_method_on_component( mcurobject, mcurmethod );
    argsparser = new ArgumentsParser( expressionparser, peer,
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
      assert( peer );
    // not really necessary, since mcurobject and mcurmethod should
    // still be valid, but anyway :)
    mcurobject = argsparser->objectname();
    mcurmethod = argsparser->methodname();

    const GlobalCommandFactory& gcf =
      peer->commandFactory;
    const GlobalMethodFactory& gmf =
      peer->methodFactory;
    const CommandFactoryInterface* cfi = gcf.getObjectFactory( mcurobject );
    const MethodFactoryInterface*  mfi = gmf.getObjectFactory( mcurobject );

    // cfi should exist, because otherwise we would have noticed in
    // seenstartofcall()...
    assert( cfi || mfi );

    ComCon comcon;
    bool ismethod = false;
    if ( cfi && cfi->hasCommand( mcurmethod ) )
        try
            {
                comcon = cfi->create( mcurmethod, argsparser->result(), masync );
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
                ismethod = true;
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
    if ( ! implcond )
        implcond = new ConditionTrue;

    // dispatch a TryCommand to other processor, overthere, the result is ignored,
    // it is interpreted here, with the implcond. Other condition branches
    // must be guarded likewise with wrapCondition().
    // we compare processors, as dispatching is not done if the processor is shared.
    // Also, methods are not dispatched.
    if ( peer->getProcessor() != context->getProcessor() && !ismethod ) {
        tcom = new TryCommand( com );
        com = new CommandDispatch( peer->getProcessor(), tcom, tcom->result().get() );
         // compose impl term cond with accept filter and do not invert the result :
        implcond = new ConditionBinaryComposite< std::logical_and<bool> >( new TryCommandResult( tcom->executed(), false ), implcond);
    }

    retcommand = com;
    implicittermcondition = implcond;
  }

    ConditionInterface* CommandParser::wrapCondition( ConditionInterface* c )
    {
        if ( peer != context )
            return new ConditionBinaryComposite< std::logical_and<bool> >( new TryCommandResult( tcom->executed(), false ), c);
        else 
            return c;
    }

    ConditionInterface* CommandParser::dispatchCondition()
    {
        if ( tcom == 0 )
            return 0;
        if ( dispatchCond == 0 )
            dispatchCond = new TryCommandResult( tcom->executed(), false );
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
        tcom = 0;
        retcommand = 0;
        implicittermcondition = 0;
        masync = true;
        mcurobject.clear();
        mcurmethod.clear();
      }

}
