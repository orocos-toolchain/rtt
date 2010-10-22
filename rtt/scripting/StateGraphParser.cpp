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

#include "parser-debug.hpp"
#include "parse_exception.hpp"
#include "StateGraphParser.hpp"
#include "CommonParser.hpp"
#include "ConditionParser.hpp"
#include "ConditionCompare.hpp"
#include "ConditionComposite.hpp"
#include "ConditionCache.hpp"
#include "ConditionBoolDataSource.hpp"
#include "ValueChangeParser.hpp"
#include "ProgramGraphParser.hpp"
#include "PeerParser.hpp"
#include "ArgumentsParser.hpp"
#include "StateMachineBuilder.hpp"
#include "../TaskContext.hpp"
#include "StateMachineService.hpp"

#include "CommandComposite.hpp"
#include "../internal/Exceptions.hpp"
#include "../base/AttributeBase.hpp"
#include "ConditionTrue.hpp"
#include "ConditionInvert.hpp"
#include "StateDescription.hpp"
#include "ParsedStateMachine.hpp"

#include <iostream>
#include <functional>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/call_traits.hpp>
#include <iostream>
#include <memory>
#include "../internal/mystd.hpp"

namespace RTT
{
    using namespace boost;
    using namespace detail;
    using boost::bind;

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
        assertion<std::string> expect_select_ident("'select' requires a valid state name.");
        assertion<std::string> expect_comma("Expected a comma separator.");
        assertion<std::string> expect_ident("Expected a valid identifier.");
        assertion<std::string> expect_event_or_if("Expected an event name or an if clause in transition statement.");
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
                                        TaskContext* tc, ExecutionEngine* tcaller, CommonParser* cp )
        : context( tc ),
          caller( tcaller ),
          mpositer( positer ),
          ln_offset(0),
          curtemplate(),
          curinstantiatedmachine(),
          curmachinebuilder( 0 ),
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
          commonparser( cp ),
          conditionparser( new ConditionParser( context, caller, *commonparser ) ),
          valuechangeparser( new ValueChangeParser(context, *commonparser, context->provides(), caller) ),
          expressionparser( new ExpressionParser(context, caller, *commonparser) ),
          argsparser(0),
          peerparser( new PeerParser(context, *commonparser, true) ) // full-path peer parser for events.
    {
        BOOST_SPIRIT_DEBUG_RULE( production );
        BOOST_SPIRIT_DEBUG_RULE( body );
        BOOST_SPIRIT_DEBUG_RULE( rootmachineinstantiation );
        BOOST_SPIRIT_DEBUG_RULE( statemachine );
        BOOST_SPIRIT_DEBUG_RULE( machineinstantiation );
        BOOST_SPIRIT_DEBUG_RULE( statemachinecontent );
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
        BOOST_SPIRIT_DEBUG_RULE( eventline );
        BOOST_SPIRIT_DEBUG_RULE( ifbranch );
        BOOST_SPIRIT_DEBUG_RULE( elsebranch );
        BOOST_SPIRIT_DEBUG_RULE( progselect );
        BOOST_SPIRIT_DEBUG_RULE( program );
        BOOST_SPIRIT_DEBUG_RULE( selector );
        BOOST_SPIRIT_DEBUG_RULE( machineinstarguments );
        BOOST_SPIRIT_DEBUG_RULE( machineinstargument );
        BOOST_SPIRIT_DEBUG_RULE( machinememvar );
        BOOST_SPIRIT_DEBUG_RULE( machinevariable );
        BOOST_SPIRIT_DEBUG_RULE( machineparam );
        BOOST_SPIRIT_DEBUG_RULE( machineconstant );
        BOOST_SPIRIT_DEBUG_RULE( machinealias );
        BOOST_SPIRIT_DEBUG_RULE( subMachinevarchange );

        production = *body[bind( &StateGraphParser::saveText, this, _1, _2)] >> expect_eof(end_p);

        body = statemachine[ bind( &StateGraphParser::seenstatemachineend, this ) ]
                         | rootmachineinstantiation;


        rootmachineinstantiation =
            str_p("RootMachine")[bind (&StateGraphParser::startrootmachineinstantiation, this) ]
            >> machineinstantiation[ bind( &StateGraphParser::seenrootmachineinstantiation, this ) ];

        statemachine =
            str_p("StateMachine") //[bind( &StateGraphParser::storeOffset, this)]
            >> expect_ident( commonparser->identifier[ bind( &StateGraphParser::seenstatemachinename, this, _1, _2 )] )
            >> expect_open( ch_p( '{' ) )
            >> statemachinecontent
            >> expect_end( ch_p( '}' ) );

        // Zero or more declarations and Zero or more states. Once a state is encountered, no more global transitions may be defined.
        statemachinecontent = *( varline | transitions | transition) >> *( varline | state);

        varline = vardec[lambda::var(commonparser->skipeol) = false] >> commonparser->eos[lambda::var(commonparser->skipeol) = true];

        vardec = subMachinedecl | machinememvar | machineparam;

        machinememvar = ( machineconstant | machinevariable | machinealias )[bind( &StateGraphParser::seenmachinevariable, this )];
        machineconstant = valuechangeparser->constantDefinitionParser();
        machinevariable = valuechangeparser->variableDefinitionParser();
        machinealias = valuechangeparser->aliasDefinitionParser();

        machineparam = valuechangeparser->paramDefinitionParser()[bind( &StateGraphParser::seenmachineparam, this )];

        subMachinedecl = str_p("SubMachine")
                         >> machineinstantiation[bind( &StateGraphParser::seensubMachineinstantiation, this )];

        machineinstantiation =
            expect_ident( commonparser->identifier[ bind( &StateGraphParser::seenmachinetypename, this, _1, _2 )] )
            >> expect_ident( commonparser->identifier[ bind( &StateGraphParser::seeninstmachinename, this, _1, _2 )] )
            >> ( ! ( ch_p( '(' )
                     >> !machineinstarguments
                     >> expect_close_parenth( ch_p( ')' ) ) ) )[ bind( &StateGraphParser::seenmachineinstantiation, this )];

        machineinstarguments =
            machineinstargument >> *( ',' >> machineinstargument );

        machineinstargument =
            commonparser->identifier[ bind( &StateGraphParser::seenmachineinstargumentname, this, _1, _2 )]
            >> '='
            >> expressionparser->parser()[ bind( &StateGraphParser::seenmachineinstargumentvalue, this )];

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

        // a state can contain various programs and variable definitions
        statecontentline =
            entry
            | preconditions
            | run
            | handle
            | transitions
            | transition
            | exit
            | (machinememvar[lambda::var(commonparser->skipeol) = false] >> commonparser->eos[lambda::var(commonparser->skipeol) = true]);

        precondition = str_p( "precondition")
            >> conditionparser->parser()[ bind( &StateGraphParser::seenprecondition, this)] ;

        preconditions = (str_p( "preconditions" )[ bind( &StateGraphParser::inpreconditions, this )]
                        >> expect_open( ch_p( '{' ))
                        >> *transline[bind(&StateGraphParser::seenendcondition,this)]
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

        // formal:
        // transition [event] [[ {program} ][ select s]] | [ if c then ][ {program} ][select s][ else [ {program} ][select s]]
        // parsed:
        // transition [ [if c then ][ {program} ][select s][ else [ {program} ][select s]]]
        //          | [ event [[ {program} ][ select s]] | [ if c then ][ {program} ][select s][ else [ {program} ][select s]] ]
        // rules:
        // transition = "transition" >> (transline | eventline)
        // transline  = progselect
        //            | (ifbranch >> !elsebranch)
        // eventline  = eventname >> transline
        // progselect = (selector | (program >> !selector))
        // ifbranch   = "if" >> c >> "then" >> progselect
        // elsebranch = "else" >> progselect
        // selector   = "select" >> ...
        // program    = "{" >> ...
        //

        // old transition statements
        // the order of rule "transition" vs "transitions" is important
        transitions = ( str_p( "transitions" )
                        >> expect_open(ch_p('{'))
                        >> *((transline|eventline)[bind(&StateGraphParser::seenendcondition,this)])
                        >> expect_end(ch_p('}')) );

        // new transition statements
        transition = str_p("transition") >> expect_event_or_if( transline | eventline )[bind(&StateGraphParser::seenendcondition,this)];
        transline  = progselect | (ifbranch >> !elsebranch);

        // @todo: capturing events are only on local ports ?!.
        eventline  =
            /*!peerparser->parser() >>*/ commonparser->identifier[ bind( &StateGraphParser::seeneventname, this,_1,_2)]
            >> expect_eventargs(argslist[ bind( &StateGraphParser::seeneventargs, this)])
            >> expect_eventselect(transline[ bind( &StateGraphParser::seeneventtrans, this)]);

        progselect = selector | (program >> (selector | eps_p[bind( &StateGraphParser::noselect, this )] ));
        // | eps_p[bind( &StateGraphParser::noselect, this )] ); // if eos fails skipeol stays false !, see clear() !

        ifbranch = str_p( "if") >> conditionparser->parser()[ bind( &StateGraphParser::seencondition, this)]
                                >> !str_p( "then" )
                                >> progselect;
        elsebranch = str_p("else")[bind( &StateGraphParser::seenelse, this )]
            >> progselect;

        program =
            ch_p('{')[ bind( &StateGraphParser::inprogram, this, "transition" )]
                >> programBody
                >> expect_end(ch_p('}'))[bind( &StateGraphParser::seentransprog, this )];

        selector =  str_p( "select" ) >> expect_select_ident(( commonparser->identifier[ bind( &StateGraphParser::seenselect, this, _1, _2) ]
                                           >> *("or" >> commonparser->identifier[ bind( &StateGraphParser::seenselect, this, _1, _2) ])
                                          )[lambda::var(commonparser->skipeol) = false]
                                                       >> commonparser->eos[lambda::var(commonparser->skipeol) = true]);

    }

    rule_t& StateGraphParser::parser() {
        return body;
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
                ORO_THROW(parse_exception_semantic_error("state " + def + " redefined.") );
            else
                curstate = existingstate;
            curstate->setEntryPoint( mpositer.get_position().line - ln_offset );
        }
        else
        {
            curstate = new StateDescription(def, mpositer.get_position().line - ln_offset ); // create an empty state
            curtemplate->addState( curstate );
        }

    }

    void StateGraphParser::seenstateend()
    {
        if ( curinitialstateflag )
        {
            if ( curtemplate->getInitialState() )
                ORO_THROW(parse_exception_semantic_error( "Attempt to define more than one initial state." ));
            else curtemplate->setInitialState( curstate );
        }
        if ( curfinalstateflag )
        {
            if ( curtemplate->getFinalState() )
                ORO_THROW(parse_exception_semantic_error( "Attempt to define more than one final state." ));
            else curtemplate->setFinalState( curstate );
        }

        assert( curstate );
        curstate->setDefined( true );
        curstate = 0;
        curinitialstateflag = false;
        curfinalstateflag = false;
        // clear all port-triggered transitions for this state.
        cur_port_events.clear();
    }

    void StateGraphParser::inprogram(const std::string& name)
    {
        // setup the progParser to parse the program body,
        // dynamically assign this parser to body.
        assert( progParser != 0 );
        // program name, stack, line offset.
        //cerr << "SGP : Stack is " << curobject->getName() <<endl;
        progParser->initBodyParser( name, curobject, ln_offset );

        programBody = progParser->bodyParser();
    }

    ProgramInterfacePtr StateGraphParser::finishProgram()
    {
        return progParser->bodyParserResult();
    }

    void StateGraphParser::seenentry()
    {
        if ( curstate->getEntryProgram() )
            ORO_THROW( parse_exception_semantic_error( "Attempt to define entry twice in state "+ curstate->getName() ));
        curstate->setEntryProgram( finishProgram() );
    }

    void StateGraphParser::seenexit()
    {
        if ( curstate->getExitProgram() )
            ORO_THROW( parse_exception_semantic_error( "Attempt to define exit twice in state "+ curstate->getName() ));
        curstate->setExitProgram( finishProgram() );
    }

    void StateGraphParser::seenhandle()
    {
        if ( curstate->getHandleProgram() )
            ORO_THROW( parse_exception_semantic_error( "Attempt to define handle twice in state "+ curstate->getName() ));
        curstate->setHandleProgram( finishProgram() );
    }

    void StateGraphParser::seenrun()
    {
        if ( curstate->getRunProgram() )
            ORO_THROW( parse_exception_semantic_error( "Attempt to define run twice in state "+ curstate->getName() ));
        curstate->setRunProgram( finishProgram() );
    }

    void StateGraphParser::seentransprog()
    {
        transProgram = finishProgram();
    }

    void StateGraphParser::seenelseprog()
    {
        // reuse transProgram to store else progr. See seenselect().
        //transProgram = finishProgram();
        //transProgram->setProgramProcessor(curtemplate->getService()->engine()->programs());
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
        peer    = peerparser->taskObject();
        peerparser->reset();

        // check if it's an operation:
        if (peer->hasOperation(evname) ) {
            argsparser =
                new ArgumentsParser( *expressionparser, context, peer->provides(),
                                     evname, "callback" );
        } else {
            // check if it's a port.
            if ( peer->hasService(evname) == false || peer->provides(evname)->hasOperation("read") == false) {
                if (curstate)
                    ORO_THROW( parse_exception_fatal_semantic_error("In state "+curstate->getName()+": InputPort or Operation "+evname+" not found in Task "+peer->getName() ));
                else
                    ORO_THROW( parse_exception_fatal_semantic_error("In statemachine: InputPort or Operation "+evname+" not found in Task "+peer->getName() ));
            }
            argsparser =
                new ArgumentsParser( *expressionparser, context, peer->provides(evname),
                                     evname, "read" );
        }

        argslist = argsparser->parser();
    }

    void StateGraphParser::seeneventargs()
    {
        evargs = argsparser->result();
        delete argsparser;
        argsparser = 0;
    }

    void StateGraphParser::noselect()
    {
        // if omitted, implicitly re-enter current state.
        if (curstate)
            doselect( curstate->getName() );
        else
            doselect(""); // global events/transitions
    }

    void StateGraphParser::seenselect( iter_t s, iter_t f)
    {
        std::string state_id(s,f);
        doselect(state_id);
    }

    void StateGraphParser::doselect( const std::string& state_id )
    {
        StateInterface* next_state = 0;
        if ( !state_id.empty() ) {
            if ( curtemplate->getState( state_id ) != 0 )
            {
                next_state = curtemplate->getState( state_id );
            }
            else
            {
                next_state = new StateDescription(state_id, 1); // create an empty state
                curtemplate->addState( next_state );
            }
            assert( next_state );
        }

        // this transition has a lower priority than the previous one
        if ( selectln == 0)
            selectln = mpositer.get_position().line - ln_offset;

        if (evname.empty()) {
            if (curcondition == 0)
                curcondition = new ConditionTrue;
        } else if ( peer->provides(evname) && peer->provides(evname)->hasOperation("read") ) { // is a port
            try {
                assert(peer->provides(evname)); // checked in seeneventname()
                ConditionInterface* evcondition = 0;
                if ( global_port_events.count(evname) ){
                    // clone the cached condition in order to avoid a second read on the port.
                    evcondition = new ConditionBoolDataSource( global_port_events[evname]->getResult().get() );
                } else
                if ( cur_port_events.count(evname) ){
                    // clone the cached condition in order to avoid a second read on the port.
                    evcondition = new ConditionBoolDataSource( cur_port_events[evname]->getResult().get() );
                } else {
                    // combine the implicit 'read(arg) == NewData' with the guard, if any.
                    DataSourceBase::shared_ptr read_dsb = peer->provides(evname)->produce("read", evargs, context->engine() );
                    DataSource<FlowStatus>* read_ds = dynamic_cast<DataSource<FlowStatus>*>(read_dsb.get());
                    assert(read_ds);
                    evcondition = new ConditionCompare<FlowStatus,std::equal_to<FlowStatus> >( new ConstantDataSource<FlowStatus>(NewData), read_ds );
                    if (curstate) {
                        cur_port_events[evname] = new ConditionCache( evcondition ); // caches result until reset().
                        evcondition = cur_port_events[evname]->clone();
                    } else {
                        //global event:
                        global_port_events[evname] = new ConditionCache( evcondition ); // caches result until reset().
                        evcondition = global_port_events[evname]->clone();
                    }
                }
                if (curcondition == 0) {
                    curcondition = evcondition;
                } else {
                    curcondition = new ConditionBinaryCompositeAND( evcondition, curcondition );
                }
            }
            catch( const wrong_number_of_args_exception& e )
                {
                    throw parse_exception_wrong_number_of_arguments
                        ( peer->getName(), evname + ".read", e.wanted, e.received );
                }
            catch( const wrong_types_of_args_exception& e )
                {
                    throw parse_exception_wrong_type_of_argument
                        ( peer->getName(), evname + ".read", e.whicharg, e.expected_, e.received_ );
                }
            elsestate = 0;
            elseProgram.reset();
        } else { // is an operation
            assert( peer->provides()->hasMember(evname) );
            bool res;
            if (curcondition == 0)
                curcondition = new ConditionTrue;

            //             if ( elsestate != 0)
            //                 res = curtemplate->createEventTransition( &(peer->eventService), evname, evargs, curstate, next_state, curcondition->clone()
            //             else
            //cerr << "Registering "<<evname<<" handler for SM."<<endl;
            try {
                res = curtemplate->createEventTransition( peer->provides(), evname, evargs, curstate, next_state, curcondition->clone(), transProgram );
                if (!res)
                    throw parse_exception_fatal_semantic_error("StateMachine could not install a Signal Handler for Operation "+evname);
            }
            catch( const wrong_number_of_args_exception& e )
                {
                    throw parse_exception_wrong_number_of_arguments
                        ( peer->getName(), evname, e.wanted, e.received );
                }
            catch( const wrong_types_of_args_exception& e )
                {
                    throw parse_exception_wrong_type_of_argument
                        ( peer->getName(), evname, e.whicharg, e.expected_, e.received_ );
                }
            catch( ... )
                {
                    assert( false );
                }

            assert( res ); // checked in seeneventname()
            elsestate = 0;
            elseProgram.reset();
            return; // we installed the Signal Handler !
        }
        // finally, install the handler:
        curtemplate->transitionSet( curstate, next_state, curcondition->clone(), transProgram, rank--, selectln );
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


    void StateGraphParser::seenstatemachineend()
    {
        assert( curtemplate );
        assert( ! curstate );

        // reclaim the defined variables:


        // Check if the Initial and Final States are ok.
        if ( curtemplate->getInitialState() == 0 )
            ORO_THROW( parse_exception_semantic_error("No initial state defined."));
        if ( curtemplate->getFinalState() == 0 )
            ORO_THROW( parse_exception_semantic_error("No final state defined."));

        if ( curtemplate->getStateList().empty() )
            ORO_THROW( parse_exception_semantic_error("No states defined in this state machine !"));

        // Check if all States are defined.
        vector<string> states = curtemplate->getStateList();
        for( vector<string>::const_iterator it = states.begin(); it != states.end(); ++it)
        {
            assert( dynamic_cast<StateDescription*>( curtemplate->getState( *it ) ) );
            StateDescription* sd = static_cast<StateDescription*>( curtemplate->getState( *it ) );
            if ( !sd->isDefined() )
                ORO_THROW( parse_exception_semantic_error("State " + *it + " not defined, but referenced to."));
        }

        // retrieve _all_ defined variables and parameters, store them and cleanup the
        // valuechangeparser.
        valuechangeparser->store( curtemplate->getService() );
        valuechangeparser->reset();

        // prepend the commands for initialising the subMachine
        // variables..
        assert( curtemplate->getInitCommand() == 0);
        if ( varinitcommands.size() > 1 )
            {
                CommandComposite* comcom = new CommandComposite;
                for ( std::vector<ActionInterface*>::iterator i = varinitcommands.begin();
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
            if (psc) {
                // remove from parent
                context->provides()->removeService( psc->getService()->getName() );
            }

        }

        // finally :
        curtemplate->finish();

        delete progParser;
        progParser = 0;

        StateMachineBuilder* scb = new StateMachineBuilder( curtemplate );
        machinebuilders[curmachinename] = scb;

        // save curmachinename for saveText.
        curobject.reset();
        curtemplate.reset();
    }

    std::vector<ParsedStateMachinePtr> StateGraphParser::parse( iter_t& begin, iter_t end )
    {
        //skip_parser_t skip_parser = SKIP_PARSER;
        //iter_pol_t iter_policy( skip_parser );
		//#define SKIP_PARSER
        iter_pol_t iter_policy( ( comment_p( "#" ) | comment_p( "//" ) | comment_p( "/*", "*/" ) | (space_p - eol_p) | commonparser->skipper  ) );
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
            std::vector<ParsedStateMachinePtr> ret = values( rootmachines );
            rootmachines.clear();
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
        catch( const parser_error<GraphSyntaxErrors, iter_t>& )
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
        delete valuechangeparser;
        delete expressionparser;
        delete conditionparser;
        delete peerparser;
    }

    void StateGraphParser::clear() {

        // remove tmp vars from TaskContext
        valuechangeparser->reset();

        // in case of corrupt file, skipeol could have remained on false,
        // so make sure it is set correctly again
        commonparser->skipeol = true;
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
        // we own curmachinebuilder, but not through this pointer...
        curmachinebuilder = 0;
        curinstantiatedmachine.reset();
        // If non null, there was a parse-error, undo all :
        if ( curtemplate )
        {

          // remove temporary subMachine peers from current task.
          for( StateMachine::ChildList::const_iterator it= curtemplate->getChildren().begin();
               it != curtemplate->getChildren().end(); ++it ) {
              ParsedStateMachine* psc = dynamic_cast<ParsedStateMachine*>( it->get() );
              if (psc && psc->getService() ) {
                  context->provides()->removeService( psc->getService()->getName() );
              }
          }
          // remove all 'this' data factories
          curtemplate->getService()->clear();

          // will also delete all children :
          curtemplate.reset();
        }
        // should be empty in most cases :
        for ( std::vector<ActionInterface*>::iterator i = varinitcommands.begin();
              i != varinitcommands.end(); ++ i )
            delete *i;
        varinitcommands.clear();
        for ( std::vector<ActionInterface*>::iterator i = paraminitcommands.begin();
              i != paraminitcommands.end(); ++ i )
            delete *i;
        paraminitcommands.clear();
        for ( machinebuilders_t::iterator i = machinebuilders.begin();
              i != machinebuilders.end(); ++i )
          delete i->second;
        machinebuilders.clear();

    }

    void StateGraphParser::seenstatemachinename( iter_t begin, iter_t end ) {
        // the 'type' of the SC :
        curmachinename = std::string ( begin, end );

        // check if the type exists already :
        if ( machinebuilders.count( curmachinename ) != 0 )
            ORO_THROW( parse_exception_semantic_error("StateMachine type " + curmachinename + " redefined."));

        curtemplate.reset(new ParsedStateMachine());
        // Connect the new SC to the relevant machines.
        // 'sc' acts as a stack for storing variables.
        curobject.reset( new StateMachineService(curtemplate, context ) );
        curobject->setName( curmachinename );
        curtemplate->setService( curobject ); // store.

        // we pass the plain file positer such that parse errors are
        // refering to correct file line numbers.
        progParser = new ProgramGraphParser(mpositer, context, caller, *commonparser);

        // set the 'type' name :
        curtemplate->setName( curmachinename, false );
    }

    void StateGraphParser::storeOffset() {
        // stores the begining of a (possible) new statemachine definition.
        // start line number :
        ln_offset = mpositer.get_position().line - 1;
        // start of text :
        saveStartPos = mpositer;
    }

    void StateGraphParser::saveText( iter_t begin, iter_t end ) {
        assert( curmachinename.length() != 0 );
        //cerr << std::string(begin, end)<<endl;
        if ( machinebuilders.count( curmachinename ) == 0 )
            return; // yes this might be possible
        // due to the shared-text implementation, we can set the text afterwards.
        machinebuilders[curmachinename]->item()->setText( std::string( saveStartPos, end) );
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

    void StateGraphParser::startrootmachineinstantiation() {
        isroot = true;
    }

    void StateGraphParser::seenrootmachineinstantiation() {
        // first reset the flag.
        isroot = false;
        if( rootmachines.find( curinstmachinename ) != rootmachines.end() )
            ORO_THROW( parse_exception_semantic_error( "Root machine \"" + curinstmachinename + "\" already defined." ));
        rootmachines[curinstmachinename] = curinstantiatedmachine;

        // recursively set the name of this SC and all subs :
        // furthermore, it adds the TC of each child as peer TC to the parent.
        curinstantiatedmachine->setName( curinstmachinename, true );

        // check if the type exists already :
        if ( context->provides()->hasService( curinstmachinename ) )
            ORO_THROW( parse_exception_semantic_error("TaskContext '"+context->getName()+"' has already a Service named '" + curinstmachinename + "' ."));

        // Transfer ownership to the owning task.
        context->provides()->addService( curinstantiatedmachine->getService() );

        curinstantiatedmachine.reset();
        curinstmachinename.clear();
    }

    void StateGraphParser::seensubMachineinstantiation() {
        if ( find_if( curtemplate->getChildren().begin(),
                      curtemplate->getChildren().end(),
                      bind( equal_to<string>(), bind(&StateMachine::getName,_1), curinstmachinename )) != curtemplate->getChildren().end() )
            ORO_THROW( parse_exception_semantic_error( "SubMachine \"" + curinstmachinename + "\" already defined." ));

        // Since we parse in the task context, we must _temporarily_
        // make each subMachine a peer of the task so that we can access
        // its methods.

        // Warning: use context->unmountService() since curinstantiatedmachine must owns it.
        if ( !context->provides()->addService( curinstantiatedmachine->getService() ) )
            ORO_THROW( parse_exception_semantic_error(
                "Name clash: name of instantiated machine \"" + curinstmachinename +
                "\"  already used as object name in task '"+context->getName()+"'." ));

        // SM child relation
        curtemplate->addChild( curinstantiatedmachine );
        // sub-Service relation.
        curtemplate->getService()->addService( curinstantiatedmachine->getService() );

        curinstantiatedmachine->setName(curinstmachinename, false ); // not recursive !

        curinstantiatedmachine.reset();
        curinstmachinename.clear();
    }

    void StateGraphParser::seenmachinetypename( iter_t begin, iter_t end ) {
        assert( curmachinebuilder == 0 );
        std::string name( begin, end );
        machinebuilders_t::iterator i = machinebuilders.find( name );
        if ( i == machinebuilders.end() )
            ORO_THROW( parse_exception_semantic_error( "StateMachine \"" + name + "\" not defined." ));
        curmachinebuilder = i->second;
    }

    void StateGraphParser::seeninstmachinename( iter_t begin, iter_t end ) {
        assert( curmachinebuilder != 0 );
        assert( curinstmachinename.empty() );
        curinstmachinename = std::string( begin, end );
    }

    void StateGraphParser::seenmachineinstargumentname( iter_t begin, iter_t end ) {
        assert( curmachineinstargumentname.empty() );
        std::string name( begin, end );
        curmachineinstargumentname = name;
    }

    void StateGraphParser::seenmachineinstargumentvalue() {
        DataSourceBase::shared_ptr value = expressionparser->getResult();
        // let's not forget this...
        expressionparser->dropResult();
        if ( curinstmachineparams.find( curmachineinstargumentname ) != curinstmachineparams.end() )
            ORO_THROW( parse_exception_semantic_error(
                "In initialisation of StateMachine \"" + curinstmachinename +
                "\": Parameter \"" + curmachineinstargumentname +"\" initialised twice..." ));
        curinstmachineparams[curmachineinstargumentname] = value;
        curmachineinstargumentname.clear();
    }

    void StateGraphParser::seenmachineinstantiation()
    {
        // TODO : move this code to the ParsedStateMachine builder.

        // Create a full depth copy (including subMachines)
        // if RootMachine, make special copy which fixes attributes such
        // that on subsequent copy() they keep pointing to same var.
        // use shared_ptr to release on throw's below.
        ParsedStateMachinePtr nsc( curmachinebuilder->build( isroot ) );

        // we stored the attributes which are params of nsc
        // in the build operation :
        machineparams_t params = nsc->getParameters();

        // first run over the given parameters to see if they all exist in
        // the context we're instantiating...
        for ( machineparamvalues_t::iterator i = curinstmachineparams.begin(); i != curinstmachineparams.end(); ++i )
        {
            machineparams_t::iterator j = params.find( i->first );
            if ( j == params.end() )
                ORO_THROW( parse_exception_semantic_error( "No parameter \"" + i->first + "\" in this StateMachine." ) );
        }

        for ( machineparams_t::iterator i = params.begin(); i != params.end(); ++i )
        {
            machineparamvalues_t::iterator j = curinstmachineparams.find( i->first );
            if ( j == curinstmachineparams.end() )
                ORO_THROW( parse_exception_semantic_error(
                    "No value given for argument \"" + i->first + "\" in instantiation of this StateMachine." ));
#ifndef ORO_EMBEDDED
            try {
                paraminitcommands.push_back( i->second->getDataSource()->updateAction( j->second.get() ) );
            }
            catch( const bad_assignment& )
                {
                    throw parse_exception_semantic_error("Attempt to initialize parameter '"+i->first+"' with a value which is of a different type." );
                }
#else
            ActionInterface* ret =  i->second->getDataSource()->updateAction( j->second.get());
            if (ret)
                paraminitcommands.push_back( ret );
            else
                return;
#endif
        }

        curinstantiatedmachine = nsc;

        // prepend the commands for initialising the subMachine
        // parameters
        if ( paraminitcommands.size() > 0 )
            {
                CommandComposite* comcom = new CommandComposite;
                for ( std::vector<ActionInterface*>::iterator i = paraminitcommands.begin();
                      i != paraminitcommands.end(); ++i )
                    comcom->add( *i );
                // init the vars as last (if any), so that they can be inited by an expression containing the params :
                if ( curinstantiatedmachine->getInitCommand() )
                    comcom->add( curinstantiatedmachine->getInitCommand() );
                curinstantiatedmachine->setInitCommand( comcom );
            }
        paraminitcommands.clear();

        curmachinebuilder = 0;
        curinstmachineparams.clear();

        // set the TaskContext name to the instance name :
        curinstantiatedmachine->getService()->setName(curinstmachinename );
    }

    void StateGraphParser::seenmachinevariable() {
        std::vector<ActionInterface*> acv = valuechangeparser->assignCommands();
        for(std::vector<ActionInterface*>::iterator it = acv.begin(); it!=acv.end(); ++it)
            varinitcommands.push_back( *it );
        // this only clears the last parsed variables, not the 'store' (see reset())
        valuechangeparser->clear();
    }

    void StateGraphParser::seenmachineparam() {
        std::vector<std::string> pnames = valuechangeparser->definedNames();
        std::vector<AttributeBase*> tbases = valuechangeparser->definedValues();
        assert( pnames.size() == tbases.size() );
        for (unsigned int i = 0; i < pnames.size(); ++i)
            curtemplate->addParameter( pnames[i] , tbases[i] );
        // this only clears the last parsed variables, not the 'store'  (see reset())
        valuechangeparser->clear();
    }


}
