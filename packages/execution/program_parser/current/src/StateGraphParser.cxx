/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  StateGraphParser.cxx

                        StateGraphParser.cxx -  description
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
#include "execution/StateGraphParser.hpp"
#include "execution/CommonParser.hpp"
#include "execution/ConditionParser.hpp"
#include "execution/CommandParser.hpp"
#include "execution/ValueChangeParser.hpp"
#include "execution/ProgramGraphParser.hpp"
#include "execution/PeerParser.hpp"
#include "execution/ArgumentsParser.hpp"
#include "execution/StateMachineBuilder.hpp"
#include "execution/DataSourceFactory.hpp"
#include "execution/TaskContext.hpp"
#include "execution/StateMachineTask.hpp"
#include "execution/mystd.hpp"

#include "execution/CommandComposite.hpp"
#include "corelib/ConditionTrue.hpp"
#include "corelib/ConditionInvert.hpp"
#include "execution/StateDescription.hpp"
#include "execution/ParsedStateMachine.hpp"

#include <iostream>
#include <functional>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/call_traits.hpp>
#include <iostream>
#include <memory>

namespace ORO_Execution
{
    using namespace boost;
    using namespace detail;
    using boost::bind;
    using namespace ORO_CoreLib;
    using namespace std;

    namespace {
        enum GraphSyntaxErrors
        {
            state_expected,
            handle_expected,
            transition_expected,
        };

        assertion<GraphSyntaxErrors> expect_state(state_expected);
        assertion<GraphSyntaxErrors> expect_handle(handle_expected);
        assertion<GraphSyntaxErrors> expect_transition(transition_expected);
        assertion<std::string> expect_end("Ending '}' expected ( or could not find out what this line means ).");
        assertion<std::string> expect_end_of_state("Exptected ending '}' at end of state ( or could not find out what this line means ).");
        assertion<std::string> expect_if("Wrongly formatted \"if ... then select\" clause.");
        assertion<std::string> expect_select("'select' statement required after emty transition program.");
        assertion<std::string> expect_comma("Expected a comma separator.");
        assertion<std::string> expect_ident("Expected a valid identifier.");
        assertion<std::string> expect_open("Open brace expected.");
        assertion<std::string> expect_eof("Invalid input in file.");
        assertion<std::string> expect_eol("Newline expected at end of statement.");
        assertion<std::string> expect_semicolon("Semi colon expected after statement.");
        assertion<std::string> expect_open_parenth( "Open parenthesis expected." );
        assertion<std::string> expect_close_parenth( "Open parenthesis expected." );
        assertion<std::string> expect_eventselect("'select' statement required after event or transition program.");
        assertion<std::string> expect_eventargs("Could not parse arguments after event.");
    }


    StateGraphParser::StateGraphParser( iter_t& positer,
                                        TaskContext* tc )
        : context( tc ),
          curcontext( 0 ),
          mpositer( positer ),
          ln_offset(0),
          curtemplate(),
          curinstantiatedcontext(),
          curcontextbuilder( 0 ),
          curinitialstateflag( false ),
          curfinalstateflag( false ),
          curstate( 0 ),
          curnonprecstate( 0 ),
          progParser( 0 ),
          elsestate(0),
          curcondition( 0 ),
          isroot(false),
          selectln(0),
          evname(""),
          conditionparser( new ConditionParser( context ) ),
          commonparser( new CommonParser ),
          valuechangeparser( new ValueChangeParser(context) ),
          expressionparser( new ExpressionParser(context) ),
          argsparser(0),
          peerparser( new PeerParser(context, true) ) // full-path peer parser for events.
    {
        BOOST_SPIRIT_DEBUG_RULE( production );
        BOOST_SPIRIT_DEBUG_RULE( rootcontextinstantiation );
        BOOST_SPIRIT_DEBUG_RULE( statecontext );
        BOOST_SPIRIT_DEBUG_RULE( contextinstantiation );
        BOOST_SPIRIT_DEBUG_RULE( statecontextcontent );
        BOOST_SPIRIT_DEBUG_RULE( varline );
        BOOST_SPIRIT_DEBUG_RULE( state );
        BOOST_SPIRIT_DEBUG_RULE( vardec );
        BOOST_SPIRIT_DEBUG_RULE( subMachinedecl );
        BOOST_SPIRIT_DEBUG_RULE( statecontent );
        BOOST_SPIRIT_DEBUG_RULE( statecontentline );
        BOOST_SPIRIT_DEBUG_RULE( entry );
        BOOST_SPIRIT_DEBUG_RULE( preconditions );
        BOOST_SPIRIT_DEBUG_RULE( precondition );
        BOOST_SPIRIT_DEBUG_RULE( handle );
        BOOST_SPIRIT_DEBUG_RULE( transitions );
        BOOST_SPIRIT_DEBUG_RULE( transition );
        BOOST_SPIRIT_DEBUG_RULE( exit );
        BOOST_SPIRIT_DEBUG_RULE( transline );
        BOOST_SPIRIT_DEBUG_RULE( selectcommand );
        BOOST_SPIRIT_DEBUG_RULE( brancher );
        BOOST_SPIRIT_DEBUG_RULE( selector );
        BOOST_SPIRIT_DEBUG_RULE( contextinstarguments );
        BOOST_SPIRIT_DEBUG_RULE( contextinstargument );
        BOOST_SPIRIT_DEBUG_RULE( contextmemvar );
        BOOST_SPIRIT_DEBUG_RULE( contextvariable );
        BOOST_SPIRIT_DEBUG_RULE( contextparam );
        BOOST_SPIRIT_DEBUG_RULE( contextconstant );
        BOOST_SPIRIT_DEBUG_RULE( contextalias );
        BOOST_SPIRIT_DEBUG_RULE( subMachinevarchange );

        production = *( (statecontext[ bind( &StateGraphParser::seenstatecontextend, this ) ]
                         >> *( rootcontextinstantiation ))[bind( &StateGraphParser::saveText, this, _1, _2)])
                         >> expect_eof(end_p);

        rootcontextinstantiation =
            str_p("RootMachine")[bind (&StateGraphParser::startrootcontextinstantiation, this) ]
            >> contextinstantiation[ bind( &StateGraphParser::seenrootcontextinstantiation, this ) ];

        statecontext =
            str_p("StateMachine") //[bind( &StateGraphParser::storeOffset, this)]
            >> expect_ident( commonparser->identifier[ bind( &StateGraphParser::seenstatecontextname, this, _1, _2 )] )
            >> expect_open( ch_p( '{' ) )
            >> statecontextcontent
            >> expect_end( ch_p( '}' ) );

        // Zero or more declarations and Zero or more states
        statecontextcontent = *( varline | state );

        varline = vardec[lambda::var(eol_skip_functor::skipeol) = false] >> commonparser->eos[lambda::var(eol_skip_functor::skipeol) = true];

        vardec = subMachinedecl | contextmemvar | contextparam;

        contextmemvar = ( contextconstant | contextvariable | contextalias )[bind( &StateGraphParser::seencontextvariable, this )];
        contextconstant = valuechangeparser->constantDefinitionParser();
        contextvariable = valuechangeparser->variableDefinitionParser();
        contextalias = valuechangeparser->aliasDefinitionParser();

        contextparam = valuechangeparser->paramDefinitionParser()[bind( &StateGraphParser::seencontextparam, this )];

        subMachinedecl = str_p("SubMachine")
                         >> contextinstantiation[bind( &StateGraphParser::seensubMachineinstantiation, this )];

        contextinstantiation =
            expect_ident( commonparser->identifier[ bind( &StateGraphParser::seencontexttypename, this, _1, _2 )] )
            >> expect_ident( commonparser->identifier[ bind( &StateGraphParser::seeninstcontextname, this, _1, _2 )] )
            >> ( ! ( ch_p( '(' )
                     >> !contextinstarguments
                     >> expect_close_parenth( ch_p( ')' ) ) ) )[ bind( &StateGraphParser::seencontextinstantiation, this )];

        contextinstarguments =
            contextinstargument >> *( ',' >> contextinstargument );

        contextinstargument =
            commonparser->identifier[ bind( &StateGraphParser::seencontextinstargumentname, this, _1, _2 )]
            >> '='
            >> expressionparser->parser()[ bind( &StateGraphParser::seencontextinstargumentvalue, this )];

        state =
          !( str_p( "initial" )[bind( &StateGraphParser::seeninitialstate,this )]
             | str_p( "final" )[bind( &StateGraphParser::seenfinalstate,this )] )
          >> str_p( "state" )
          >> expect_ident(commonparser->identifier[ bind( &StateGraphParser::statedef, this, _1, _2 ) ])
          >> expect_open(ch_p( '{' ))
          >> statecontent
          >> expect_end_of_state(ch_p( '}' ))[ bind( &StateGraphParser::seenstateend, this ) ];

        // the content of a program can be any number of lines
        // a line is not strictly defined in the sense of text-line.
        statecontent = *statecontentline;

        // a can contain a statement.
        statecontentline = entry | preconditions | run | handle | transitions | exit;

        precondition = str_p( "precondition")
            >> conditionparser->parser()[ bind( &StateGraphParser::seenprecondition, this)] ;

        preconditions = (str_p( "preconditions" )[ bind( &StateGraphParser::inpreconditions, this )]
                        >> expect_open( ch_p( '{' ))
                        >> *transline
                        >> expect_end( ch_p( '}' ) )[
                            bind( &StateGraphParser::seenpreconditions, this )]) | precondition;

        entry = str_p( "entry" )[ bind( &StateGraphParser::inprogram, this, "entry" )]
                >> expect_open(ch_p('{'))>> programBody >> expect_end(ch_p('}'))[
                    bind( &StateGraphParser::seenentry, this )];

        run = str_p( "run" )[ bind( &StateGraphParser::inprogram, this, "run" )]
                 >> expect_open(ch_p('{'))>> programBody >> expect_end(ch_p('}'))[
                     bind( &StateGraphParser::seenrun, this )];

        exit = str_p( "exit" )[ bind( &StateGraphParser::inprogram, this, "exit" )]
               >> expect_open(ch_p('{')) >> programBody >> expect_end(ch_p('}'))[
                   bind( &StateGraphParser::seenexit, this )];

        handle = str_p( "handle" )[ bind( &StateGraphParser::inprogram, this, "handle" )]
                 >> expect_open(ch_p('{'))>> programBody >> expect_end(ch_p('}'))[
                     bind( &StateGraphParser::seenhandle, this )];

        // event based transition
        transition = str_p("transition")
            >> !peerparser->parser() >> expect_ident(commonparser->identifier[ bind( &StateGraphParser::seeneventname, this,_1,_2)])
            >> expect_eventargs(argslist[ bind( &StateGraphParser::seeneventargs, this)])
            >> expect_eventselect(selectcommand[ bind( &StateGraphParser::seeneventtrans, this)]);

        // condition based transition.
        // the order of rule "transition" vs "transitions" is important
        transitions = ( str_p( "transitions" )
                        >> expect_open(ch_p('{'))>> *transline >> expect_end(ch_p('}')) ) | transition;

        transline = selectcommand;

        // You are only allowed to select a new state in transitions :
        selectcommand = (brancher | selector)[bind(&StateGraphParser::seenendcondition,this)];

        brancher = str_p( "if") >> conditionparser->parser()[ bind( &StateGraphParser::seencondition, this)]
                                >> expect_if(str_p( "then" ))
                                >> !transprog
                                >> !selector
                                >> !(str_p("else")[bind( &StateGraphParser::seenelse, this )] >> expect_select( (elseprog >> !selector) | selector));
        transprog =
            ch_p('{')[ bind( &StateGraphParser::inprogram, this, "transition" )]
                >> programBody
                >> expect_end(ch_p('}'))[bind( &StateGraphParser::seentransprog, this )];

        elseprog = 
            ch_p('{')[ bind( &StateGraphParser::inprogram, this, "elsetransition" )]
                >> programBody
                >> expect_end(ch_p('}'))[bind( &StateGraphParser::seenelseprog, this )];


        selector = str_p( "select" ) >> ( commonparser->identifier[ bind( &StateGraphParser::seenselect, this, _1, _2) ]
                                          >> *("or" >> commonparser->identifier[ bind( &StateGraphParser::seenselect, this, _1, _2) ])
                                          )[lambda::var(eol_skip_functor::skipeol) = false]
                                     >> commonparser->eos[lambda::var(eol_skip_functor::skipeol) = true]; // if eos fails skipeol stays false !, see cleanup() !

    }

    void StateGraphParser::seeninitialstate()
    {
        curinitialstateflag = true;
    }

    void StateGraphParser::seenfinalstate()
    {
        curfinalstateflag = true;
    }

    void StateGraphParser::statedef( iter_t s, iter_t f)
    {
        assert( !curstate );

        std::string def(s, f);
        if ( curtemplate->getState( def ) != 0 )
        {
            assert( dynamic_cast<StateDescription*>( curtemplate->getState( def ) ) );
            StateDescription* existingstate = static_cast<StateDescription*>( curtemplate->getState( def ) );
            if ( existingstate->isDefined() )
                throw parse_exception_semantic_error("state " + def + " redefined.");
            else
                curstate = existingstate;
            curstate->setEntryPoint( mpositer.get_position().line - ln_offset );
        }
        else
        {
            curstate = new StateDescription(def, curtemplate->getTaskContext()->engine()->programs(), mpositer.get_position().line - ln_offset ); // create an empty state
            curtemplate->addState( curstate );
        }

    }

    void StateGraphParser::seenstateend()
    {
        if ( curinitialstateflag )
        {
            if ( curtemplate->getInitialState() )
                throw parse_exception_semantic_error( "Attempt to define more than one initial state." );
            else curtemplate->setInitialState( curstate );
        }
        if ( curfinalstateflag )
        {
            if ( curtemplate->getFinalState() )
                throw parse_exception_semantic_error( "Attempt to define more than one final state." );
            else curtemplate->setFinalState( curstate );
        }

        assert( curstate );
        curstate->setDefined( true );
        curstate = 0;
        curinitialstateflag = false;
        curfinalstateflag = false;

    }

    void StateGraphParser::inprogram(const std::string& name)
    {
        // setup the progParser to parse the program body,
        // dynamically assign this parser to body.
        assert( progParser != 0 );
        // program name, stack, line offset.
        //cerr << "SGP : Stack is " << curcontext->getName() <<endl;
        progParser->initBodyParser( name, curcontext, ln_offset );
        
        programBody = progParser->bodyParser();
    }

    ProgramInterfacePtr StateGraphParser::finishProgram()
    {
        return progParser->bodyParserResult();
    }

    void StateGraphParser::seenentry()
    {
        if ( curstate->getEntryProgram() )
            throw parse_exception_semantic_error( "Attempt to define entry twice in state "+ curstate->getName() );
        curstate->setEntryProgram( finishProgram() );
    }

    void StateGraphParser::seenexit()
    {
        if ( curstate->getExitProgram() )
            throw parse_exception_semantic_error( "Attempt to define exit twice in state "+ curstate->getName() );
        curstate->setExitProgram( finishProgram() );
    }

    void StateGraphParser::seenhandle()
    {
        if ( curstate->getHandleProgram() )
            throw parse_exception_semantic_error( "Attempt to define handle twice in state "+ curstate->getName() );
        curstate->setHandleProgram( finishProgram() );
    }

    void StateGraphParser::seenrun()
    {
        if ( curstate->getRunProgram() )
            throw parse_exception_semantic_error( "Attempt to define run twice in state "+ curstate->getName() );
        curstate->setRunProgram( finishProgram() );
    }

    void StateGraphParser::seentransprog()
    {
        transProgram = finishProgram();
        transProgram->setProgramProcessor(curtemplate->getTaskContext()->engine()->programs());
    }

    void StateGraphParser::seenelseprog()
    {
        // reuse transProgram to store else progr. See seenselect().
        transProgram = finishProgram();
        transProgram->setProgramProcessor(curtemplate->getTaskContext()->engine()->programs());
    }

    void StateGraphParser::seenelse()
    {
        assert( curcondition);
        curcondition = new ConditionInvert( curcondition );
    }

    void StateGraphParser::seencondition()
    {
        assert( !curcondition );
        curcondition = conditionparser->getParseResult();
        assert( curcondition );
        conditionparser->reset();
        selectln = mpositer.get_position().line - ln_offset;
    }

    void StateGraphParser::seeneventname(iter_t s, iter_t f)
    {
        evname = string(s,f);

        // seenselect() will use evname to see if event is causing transition
        assert(evname.length());
        peer    = peerparser->peer();
        peerparser->reset();
        argsparser =
            new ArgumentsParser( *expressionparser, peer,
                                 evname, "handle" );
        argslist = argsparser->parser();
    }

    void StateGraphParser::seeneventargs()
    {
        evargs = argsparser->result();
        delete argsparser;
        argsparser = 0;
    }

    void StateGraphParser::seenselect( iter_t s, iter_t f)
    {
        std::string state_id(s,f);
        StateInterface* next_state;
        if ( curtemplate->getState( state_id ) != 0 )
        {
            next_state = curtemplate->getState( state_id );
        }
        else
        {
            next_state = new StateDescription(state_id,curtemplate->getTaskContext()->engine()->programs(), 1); // create an empty state
            curtemplate->addState( next_state );
        }
        assert( next_state );

        if (curcondition == 0)
            curcondition = new ConditionTrue;

        if (evname.empty()) {
            // this transition has a lower priority than the previous one
            if ( selectln == 0)
                selectln = mpositer.get_position().line - ln_offset;
//             if ( elsestate != 0)
//                 curtemplate->transitionSet( curstate, next_state, curcondition->clone(), transProgram, elsestate, elseProgram, rank--, selectln );
//             else
            curtemplate->transitionSet( curstate, next_state, curcondition->clone(), transProgram, rank--, selectln );
        } else {
            bool res;
//             if ( elsestate != 0)
//                 res = curtemplate->createEventTransition( &(peer->eventService), evname, evargs, curstate, next_state, curcondition->clone(), transProgram, elsestate, elseProgram );
//             else
            res = curtemplate->createEventTransition( peer->events(), evname, evargs, curstate, next_state, curcondition->clone(), transProgram );
            elsestate = 0;
            elseProgram.reset();
            if (res == false)
                throw parse_exception_fatal_semantic_error("In state "+curstate->getName()+": Event "+evname+" not found in Task "+peer->getName() );
        }
    }

    void StateGraphParser::seenendcondition() {
        delete curcondition;
        curcondition = 0;
        selectln = 0;
        transProgram.reset();
    }

    void StateGraphParser::seeneventtrans() {
        // cleanup all event related state.
        evname.clear();
        evargs.clear();
    }

    void StateGraphParser::seenprecondition()
    {
        assert( !curcondition );
        curcondition = conditionparser->getParseResult();
        assert( curcondition );
        conditionparser->reset();
        selectln = mpositer.get_position().line - ln_offset;

        curtemplate->preconditionSet(curstate, curcondition, selectln );
        selectln = 0;
        curcondition = 0;
    }


    void StateGraphParser::seenstatecontextend()
    {
        assert( curtemplate );
        assert( ! curstate );

        // Check if the Initial and Final States are ok.
        if ( curtemplate->getInitialState() == 0 )
            throw parse_exception_semantic_error("No initial state defined.");
        if ( curtemplate->getFinalState() == 0 )
            throw parse_exception_semantic_error("No final state defined.");

        if ( curtemplate->getStateList().empty() )
            throw parse_exception_semantic_error("No states defined in this state context !");

        // Check if all States are defined.
        vector<string> states = curtemplate->getStateList();
        for( vector<string>::const_iterator it = states.begin(); it != states.end(); ++it)
        {
            assert( dynamic_cast<StateDescription*>( curtemplate->getState( *it ) ) );
            StateDescription* sd = static_cast<StateDescription*>( curtemplate->getState( *it ) );
            if ( !sd->isDefined() )
                throw parse_exception_semantic_error("State " + *it + " not defined, but referenced to.");
        }

        // prepend the commands for initialising the subMachine
        // variables..
        assert( curtemplate->getInitCommand() == 0);
        if ( varinitcommands.size() > 1 )
            {
                CommandComposite* comcom = new CommandComposite;
                for ( std::vector<CommandInterface*>::iterator i = varinitcommands.begin();
                      i != varinitcommands.end(); ++i )
                    comcom->add( *i );
                curtemplate->setInitCommand( comcom );
            }
        else if (varinitcommands.size() == 1 )
            curtemplate->setInitCommand( *varinitcommands.begin() );

        varinitcommands.clear();

        // remove temporary subMachine peers from current task.
        for( StateMachine::ChildList::const_iterator it= curtemplate->getChildren().begin();
             it != curtemplate->getChildren().end(); ++it ) {
            ParsedStateMachine* psc = dynamic_cast<ParsedStateMachine*>( it->get() );
            if (psc)
                context->removePeer( psc->getTaskContext()->getName() );
        }

        // finally : 
        curtemplate->finish();

        delete progParser;
        progParser = 0;

        // reset stack to task.
        valuechangeparser->setStack(context);
//         commandparser->setStack(context);
        expressionparser->setStack(context);
        conditionparser->setStack(context);

        StateMachineBuilder* scb = new StateMachineBuilder( curtemplate );
        contextbuilders[curcontextname] = scb;

        // save curcontextname for saveText.
        curcontext = 0;
        curtemplate.reset();
    }

    std::vector<ParsedStateMachinePtr> StateGraphParser::parse( iter_t& begin, iter_t end )
    {
        skip_parser_t skip_parser = SKIP_PARSER;
        iter_pol_t iter_policy( skip_parser );
        scanner_pol_t policies( iter_policy );
        scanner_t scanner( begin, end, policies );

        // reset the condition-transition priority.
        rank = 0;

        this->storeOffset();

        try {
            if ( ! production.parse( scanner ) )
            {
                // on error, we clear all remaining data, cause we can't
                // guarantee consistency...
                clear();
                throw file_parse_exception(
                    new parse_exception_syntactic_error( "Syntax error" ),
                    mpositer.get_position().file, mpositer.get_position().line,
                    mpositer.get_position().column );
            }
            std::vector<ParsedStateMachinePtr> ret = ORO_std::values( rootcontexts );
            rootcontexts.clear();
            return ret;
        }
        catch( const parser_error<std::string, iter_t>& e )
        {
            // on error, we clear all remaining data, cause we can't
            // guarantee consistency...
            clear();
            throw file_parse_exception(
                new parse_exception_syntactic_error( e.descriptor ),
                mpositer.get_position().file, mpositer.get_position().line,
                mpositer.get_position().column );
        }
        catch( const parser_error<GraphSyntaxErrors, iter_t>& e )
        {
            // on error, we clear all remaining data, cause we can't
            // guarantee consistency...
            clear();
            throw file_parse_exception(
                new parse_exception_syntactic_error( "Expected one of: entry, handle, exit, transitions" ),
                mpositer.get_position().file, mpositer.get_position().line,
                mpositer.get_position().column );
        }
        catch( const parse_exception& e )
        {
            // on error, we clear all remaining data, cause we can't
            // guarantee consistency...
            clear();
            throw file_parse_exception(
                e.copy(), mpositer.get_position().file,
                mpositer.get_position().line, mpositer.get_position().column );
        }
//         catch( ... ) {
//             assert( false );
//         }
    }

    StateGraphParser::~StateGraphParser() {
        clear();
        delete commonparser;
        delete valuechangeparser;
        delete expressionparser;
        delete conditionparser;
        delete peerparser;
    }

    void StateGraphParser::clear() {

        // in case of corrupt file, skipeol could have remained on false,
        // so make sure it is set correctly again (I hate this global variable approach, it should be a member of commonparser !)
        eol_skip_functor::skipeol = true;
        selectln = 0;
        transProgram.reset();
        elseProgram.reset();
        delete argsparser;
        argsparser = 0;
        delete curcondition;
        curcondition = 0;
        // we own curstate, but not through this pointer...
        curstate = 0;
        delete curnonprecstate;
        curnonprecstate = 0;
        // we own curcontextbuilder, but not through this pointer...
        curcontextbuilder = 0;
        curinstantiatedcontext.reset();
        // If non null, there was a parse-error, undo all :
        if ( curtemplate )
        {
          // remove all 'this' data factories
          curtemplate->getTaskContext()->dataFactory.unregisterObject( "this" );
          curtemplate->getTaskContext()->methodFactory.unregisterObject( "this" );

          // remove temporary subMachine peers from current task.
          for( StateMachine::ChildList::const_iterator it= curtemplate->getChildren().begin();
               it != curtemplate->getChildren().end(); ++it ) {
              ParsedStateMachine* psc = dynamic_cast<ParsedStateMachine*>( it->get() );
              if (psc)
                  context->removePeer( psc->getTaskContext()->getName() );
          }

          // remove the type from __states
          context->getPeer("__states")->removePeer( curtemplate->getTaskContext()->getName() ) ;

          // will also delete all children : 
          curtemplate.reset();
        }
        // should be empty in most cases :
        for ( std::vector<CommandInterface*>::iterator i = varinitcommands.begin();
              i != varinitcommands.end(); ++ i )
            delete *i;
        varinitcommands.clear();
        for ( std::vector<CommandInterface*>::iterator i = paraminitcommands.begin();
              i != paraminitcommands.end(); ++ i )
            delete *i;
        paraminitcommands.clear();
        for ( contextbuilders_t::iterator i = contextbuilders.begin();
              i != contextbuilders.end(); ++i )
          delete i->second;
        contextbuilders.clear();
        
        // We also remove all parsed SC's because there is for now no possiblity for
        // reuse, since only the TaskContext pointer is stored and not the ParsedSC's.
        TaskContext* __s = context->getPeer("__states");
        if ( __s ) {
            context->removePeer("__states");
            delete __s;
        }

    }

    void StateGraphParser::seenstatecontextname( iter_t begin, iter_t end ) {
        // the 'type' of the SC :
        curcontextname = std::string ( begin, end );

        // store the SC in the TaskContext current.__states
        // __states is a storage for template SC's
        TaskContext* __s = context->getPeer("__states");
        if ( __s == 0 ) {
            // install the __states if not yet present.
            __s = new TaskContext("__states", context->getExecutionEngine() );
            context->addPeer( __s );
        }

        // check if the type exists already :
        if ( __s->hasPeer( curcontextname ) )
            throw parse_exception_semantic_error("State Context " + curcontextname + " redefined.");

        curtemplate.reset(new ParsedStateMachine());
        // Connect the new SC to the relevant contexts.
        // 'sc' acts as a stack for storing variables.
        curcontext = new StateMachineTask(curtemplate, context->getExecutionEngine() );
        __s->addPeer( curcontext );   // store in __states.
        curtemplate->setTaskContext( curcontext ); // store.
        
        // add the 'task' peer :
        curcontext->addPeer( context, "task" );

        // we pass the plain file positer such that parse errors are
        // refering to correct file line numbers.
        progParser = new ProgramGraphParser(mpositer, context);

        // Only the stack is stored in curcontext
        valuechangeparser->setStack(curcontext);
//         commandparser->setStack(curcontext);
        expressionparser->setStack(curcontext);
        conditionparser->setStack(curcontext);

        // set the 'type' name :
        curtemplate->setName( curcontextname, false );
    }

    void StateGraphParser::storeOffset() {
        // stores the begining of a (possible) new statemachine definition.
        // start line number :
        ln_offset = mpositer.get_position().line - 1;
        // start of text :
        saveStartPos = mpositer;
    }

    void StateGraphParser::saveText( iter_t begin, iter_t end ) {
        assert( curcontextname.length() != 0 );
        //cerr << std::string(begin, end)<<endl;
        if ( contextbuilders.count( curcontextname ) == 0 )
            return; // yes this might be possible
        // due to the shared-text implementation, we can set the text afterwards.
        contextbuilders[curcontextname]->item()->setText( std::string( saveStartPos, end) );
        this->storeOffset();
    }

    void StateGraphParser::inpreconditions() {
        // we postpone the current state
        assert( curnonprecstate == 0 );
        curnonprecstate = curstate->postponeState();
        // add the postponed state in PSM :
        curtemplate->addState( curnonprecstate );
    }

    void StateGraphParser::seenpreconditions() {
        curtemplate->transitionSet( curstate, curnonprecstate, new ConditionTrue, rank--, mpositer.get_position().line - ln_offset );
        curstate->setDefined( true );
        curstate = curnonprecstate;
        curnonprecstate = 0;
    }

    void StateGraphParser::startrootcontextinstantiation() {
        isroot = true;
    }

    void StateGraphParser::seenrootcontextinstantiation() {
        // first reset the flag.
        isroot = false;
        if( rootcontexts.find( curinstcontextname ) != rootcontexts.end() )
            throw parse_exception_semantic_error( "Root context \"" + curinstcontextname + "\" already defined." );
        rootcontexts[curinstcontextname] = curinstantiatedcontext;

        // recursively set the name of this SC and all subs :
        // furthermore, it adds the TC of each child as peer TC to the parent.
        curinstantiatedcontext->setName( curinstcontextname, true );

        // add it to the "states" (all instantiated) of the current context :
        TaskContext* __s = context->getPeer("states");
        if ( __s == 0 ) {
            // install the __states if not yet present.
            __s = new TaskContext("states", context->getExecutionEngine() );
            context->addPeer( __s );
            __s->addPeer(context, "task");
        }

        // check if the type exists already :
        if ( __s->hasPeer( curinstcontextname ) )
            throw parse_exception_semantic_error("Task '"+context->getName()+"' has already a State Context '" + curinstcontextname + "' .");
        __s->connectPeers( curinstantiatedcontext->getTaskContext() );
        curinstantiatedcontext->getTaskContext()->addPeer(context, "task");

        curinstantiatedcontext.reset();
        curinstcontextname.clear();
    }

    void StateGraphParser::seensubMachineinstantiation() {
        if ( find_if( curtemplate->getChildren().begin(),
                      curtemplate->getChildren().end(),
                      bind( equal_to<string>(), bind(&StateMachine::getName,_1), curinstcontextname )) != curtemplate->getChildren().end() )
            throw parse_exception_semantic_error( "SubMachine \"" + curinstcontextname + "\" already defined." );

        // Since we parse in the task context, we must _temporarily_
        // make each subMachine a peer of the task so that we can access
        // its methods.

        if ( !context->addPeer( curinstantiatedcontext->getTaskContext() ) )
            throw parse_exception_semantic_error(
                "Name clash: name of instantiated context \"" + curinstcontextname +
                "\"  already used as peer name in task '"+context->getName()+"'." );
            

        curtemplate->addChild( curinstantiatedcontext );
        curtemplate->getTaskContext()->addPeer( curinstantiatedcontext->getTaskContext() );
        // we add this statecontext to the list of variables, so that the
        // user can refer to it by its name...
        //detail::ParsedAlias<std::string>* pv = new detail::ParsedAlias<std::string>( curinstantiatedcontext->getNameDS() );
        //context->attributeRepository.setValue( curinstcontextname, pv );

        curinstantiatedcontext->setName(curinstcontextname, false ); // not recursive !

        curinstantiatedcontext.reset();
        curinstcontextname.clear();
    }

    void StateGraphParser::seencontexttypename( iter_t begin, iter_t end ) {
        assert( curcontextbuilder == 0 );
        std::string name( begin, end );
        contextbuilders_t::iterator i = contextbuilders.find( name );
        if ( i == contextbuilders.end() )
            throw parse_exception_semantic_error( "StateMachine \"" + name + "\" not defined." );
        curcontextbuilder = i->second;
    }

    void StateGraphParser::seeninstcontextname( iter_t begin, iter_t end ) {
        assert( curcontextbuilder != 0 );
        assert( curinstcontextname.empty() );
        curinstcontextname = std::string( begin, end );
    }

    void StateGraphParser::seencontextinstargumentname( iter_t begin, iter_t end ) {
        assert( curcontextinstargumentname.empty() );
        std::string name( begin, end );
        curcontextinstargumentname = name;
    }

    void StateGraphParser::seencontextinstargumentvalue() {
        DataSourceBase::shared_ptr value = expressionparser->getResult();
        // let's not forget this...
        expressionparser->dropResult();
        if ( curinstcontextparams.find( curcontextinstargumentname ) != curinstcontextparams.end() )
            throw parse_exception_semantic_error(
                "In initialisation of StateMachine \"" + curinstcontextname +
                "\": Parameter \"" + curcontextinstargumentname +"\" initialised twice..." );
        curinstcontextparams[curcontextinstargumentname] = value;
        curcontextinstargumentname.clear();
    }

    void StateGraphParser::seencontextinstantiation()
    {
        // TODO : move this code to the ParsedStateMachine builder.

        // Create a full depth copy (including subMachines)
        // if RootMachine, make special copy which fixes attributes such
        // that on subsequent copy() they keep pointing to same var.
        // use shared_ptr to release on throw's below.
        ParsedStateMachinePtr nsc( curcontextbuilder->build( isroot ) );

        // we stored the attributes which are params of nsc 
        // in the build operation :
        contextparams_t params = nsc->getParameters();

        // first run over the given parameters to see if they all exist in
        // the context we're instantiating...
        for ( contextparamvalues_t::iterator i = curinstcontextparams.begin(); i != curinstcontextparams.end(); ++i )
        {
            contextparams_t::iterator j = params.find( i->first );
            if ( j == params.end() )
                throw parse_exception_semantic_error( "No parameter \"" + i->first + "\" in this state context." );
        }

        for ( contextparams_t::iterator i = params.begin(); i != params.end(); ++i )
        {
            contextparamvalues_t::iterator j = curinstcontextparams.find( i->first );
            if ( j == curinstcontextparams.end() )
                throw parse_exception_semantic_error(
                    "No value given for argument \"" + i->first + "\" in instantiation of this state context." );
            try {
                paraminitcommands.push_back( i->second->assignCommand( j->second.get(), true ) );
            }
            catch( const bad_assignment& e )
                {
                    throw parse_exception_semantic_error("Attempt to initialize parameter '"+i->first+"' with a value which is of a different type." );
                }

        }

        curinstantiatedcontext = nsc;

        // prepend the commands for initialising the subMachine
        // parameters
        if ( paraminitcommands.size() > 0 )
            {
                CommandComposite* comcom = new CommandComposite;
                for ( std::vector<CommandInterface*>::iterator i = paraminitcommands.begin();
                      i != paraminitcommands.end(); ++i )
                    comcom->add( *i );
                // init the vars as last (if any), so that they can be inited by an expression containing the params :
                if ( curinstantiatedcontext->getInitCommand() )
                    comcom->add( curinstantiatedcontext->getInitCommand() );
                curinstantiatedcontext->setInitCommand( comcom );
            }
        paraminitcommands.clear();

        curcontextbuilder = 0;
        curinstcontextparams.clear();

        // set the TaskContext name to the instance name :
        curinstantiatedcontext->getTaskContext()->setName(curinstcontextname );

    }

    void StateGraphParser::seencontextvariable() {
        std::vector<CommandInterface*> acv = valuechangeparser->assignCommands();
        for(std::vector<CommandInterface*>::iterator it = acv.begin(); it!=acv.end(); ++it)
            varinitcommands.push_back( *it );
        valuechangeparser->reset();
    }

  void StateGraphParser::seencontextparam() {
      std::vector<std::string> pnames = valuechangeparser->parsedDefinitionNames();
      std::vector<TaskAttributeBase*> tbases = valuechangeparser->definedValues();
      assert( pnames.size() == tbases.size() );
      for (unsigned int i = 0; i < pnames.size(); ++i)
          curtemplate->addParameter( pnames[i] , tbases[i] );
      valuechangeparser->reset();
  }

#if 0
  void StateGraphParser::seenscvcsubMachinename( iter_t begin, iter_t end )
  {
    // will fail if we have had our parse rule failing before (
    // happens when we encounter a normal set command... )
    //assert( curscvccontextname.empty() );
    curscvccontextname = std::string( begin, end );
  }

    void StateGraphParser::seenscvcparamname( iter_t begin, iter_t end )
  {
    assert( !curscvccontextname.empty() );
    assert( curscvcparamname.empty() );
    curscvcparamname = std::string( begin, end );
  }
#endif

}
