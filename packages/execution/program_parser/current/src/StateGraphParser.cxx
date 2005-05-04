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
#include "execution/StateMachineBuilder.hpp"
#include "execution/DataSourceFactory.hpp"
#include "execution/TaskContext.hpp"
#include "execution/mystd.hpp"

#include "execution/Processor.hpp"
#include "execution/CommandComposite.hpp"
#include "corelib/ConditionTrue.hpp"
#include "execution/StateDescription.hpp"
#include "execution/ProgramGraph.hpp"
#include "execution/ParsedStateMachine.hpp"

#include <iostream>
#include <functional>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/call_traits.hpp>
#include <iostream>

namespace ORO_Execution
{
    using namespace boost;
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
        assertion<std::string> expect_end("Ending brace expected ( or could not find out what this line means ).");
        assertion<std::string> expect_if("Wrongly formatted \"if ... then select\" clause.");
        assertion<std::string> expect_comma("Expected a comma separator.");
        assertion<std::string> expect_ident("Expected a valid identifier.");
        assertion<std::string> expect_open("Open brace expected.");
        assertion<std::string> expect_eof("Invalid input in file.");
        assertion<std::string> expect_semicolon("Semi colon expected after statement.");
        assertion<std::string> expect_open_parenth( "Open parenthesis expected." );
        assertion<std::string> expect_close_parenth( "Open parenthesis expected." );
    }


    StateGraphParser::StateGraphParser( iter_t& positer,
                                        TaskContext* tc )
        : context( tc ),
          curcontext( 0 ),
          mpositer( positer ),
          ln_offset(0),
          curtemplatecontext( 0 ),
          curinstantiatedcontext( 0 ),
          curcontextbuilder( 0 ),
          curinitialstateflag( false ),
          curfinalstateflag( false ),
          curstate( 0 ),
          curnonprecstate( 0 ),
          progParser( 0 ),
          curcondition( 0 ),
          conditionparser( new ConditionParser( context ) ),
          commonparser( new CommonParser ),
          valuechangeparser( new ValueChangeParser(context) ),
          expressionparser( new ExpressionParser(context) )
    {
        BOOST_SPIRIT_DEBUG_RULE( production );
        BOOST_SPIRIT_DEBUG_RULE( newline );
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
        BOOST_SPIRIT_DEBUG_RULE( handle );
        BOOST_SPIRIT_DEBUG_RULE( transitions );
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

        newline = ch_p( '\n' );

        production = *( newline | statecontext[bind( &StateGraphParser::saveText, this, _1, _2)][ bind( &StateGraphParser::seenstatecontextend, this ) ] ) >> *( newline | rootcontextinstantiation ) >> expect_eof(end_p);

        rootcontextinstantiation =
            str_p("RootMachine")
            >> contextinstantiation[ bind( &StateGraphParser::seenrootcontextinstantiation, this ) ];

        statecontext =
            str_p("StateMachine")[bind( &StateGraphParser::storeOffset, this)]
            >> expect_ident( commonparser->identifier[ bind( &StateGraphParser::seenstatecontextname, this, _1, _2 )] )
            >> !newline
            >> expect_open( ch_p( '{' ) )
            >> statecontextcontent
            >> expect_end( ch_p( '}' ) )
            >> newline;

        // Zero or more declarations and Zero or more states
        statecontextcontent = ( *varline >> *( state | newline ) );

        varline = !vardec >> newline;

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
            >> ( ! ( str_p( "(" )
                     >> !contextinstarguments
                     >> expect_close_parenth( str_p( ")" ) ) ) )[ bind( &StateGraphParser::seencontextinstantiation, this )];

        contextinstarguments =
            contextinstargument >> *( "," >> contextinstargument );

        contextinstargument =
            commonparser->identifier[ bind( &StateGraphParser::seencontextinstargumentname, this, _1, _2 )]
            >> "="
            >> expressionparser->parser()[ bind( &StateGraphParser::seencontextinstargumentvalue, this )];

        state =
          !( str_p( "initial" )[bind( &StateGraphParser::seeninitialstate,this )]
             | str_p( "final" )[bind( &StateGraphParser::seenfinalstate,this )] )
          >> str_p( "state" )
          >> expect_ident(commonparser->identifier[ bind( &StateGraphParser::statedef, this, _1, _2 ) ])
          >> !newline
          >> expect_open(ch_p( '{' ))
          >> statecontent
          >> expect_end(ch_p( '}' ))
          >> newline[ bind( &StateGraphParser::seenstateend, this ) ];

        // the content of a program can be any number of lines
        // a line is not strictly defined in the sense of text-line,
        // a line can contain newlines.
        statecontent = *statecontentline;

        // a line can be empty or contain a statement. Empty is
        // necessary, because comment's are skipped, but newline's
        // aren't.  So a line like "/* very interesting comment
        // */\n" will reach us as simply "\n"..
        statecontentline = !( entry | preconditions | handle | transitions | exit ) >> newline;

        preconditions = str_p( "preconditions" )[ bind( &StateGraphParser::inpreconditions, this )]
                        >> expect_open( str_p( "{" ))
                        >> *transline
                        >> expect_end( str_p( "}" ) )[
                            bind( &StateGraphParser::seenpreconditions, this )];

        entry = str_p( "entry" )[ bind( &StateGraphParser::inprogram, this, "entry" )]
                >> expect_open(str_p("{"))>> programBody >> expect_end(str_p("}"))[
                    bind( &StateGraphParser::seenentry, this )];

        exit = str_p( "exit" )[ bind( &StateGraphParser::inprogram, this, "exit" )]
               >> expect_open(str_p("{")) >> programBody >> expect_end(str_p("}"))[
                   bind( &StateGraphParser::seenexit, this )];

        handle = str_p( "handle" )[ bind( &StateGraphParser::inprogram, this, "handle" )]
                 >> expect_open(str_p("{"))>> programBody >> expect_end(str_p("}"))[
                     bind( &StateGraphParser::seenhandle, this )];

        transitions = str_p( "transitions" )
                      >> expect_open(str_p("{"))>> *transline >> expect_end(str_p("}"));

        transline = !selectcommand >> newline;

        // You are only allowed to select a new state in transitions :
        selectcommand = brancher | selector;

        brancher = str_p( "if") >> conditionparser->parser()[ bind( &StateGraphParser::seencondition, this)]
                                >> expect_if(str_p( "then" ))>> !newline >> expect_if(selector);

        selector = str_p( "select" ) >> commonparser->identifier[ bind( &StateGraphParser::seenselect, this, _1, _2) ];

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
        if ( curtemplatecontext->getState( def ) != 0 )
        {
            assert( dynamic_cast<StateDescription*>( curtemplatecontext->getState( def ) ) );
            StateDescription* existingstate = static_cast<StateDescription*>( curtemplatecontext->getState( def ) );
            if ( existingstate->isDefined() )
                throw parse_exception_semantic_error("state " + def + " redefined.");
            else
                curstate = existingstate;
            curstate->setEntryPoint( mpositer.get_position().line - ln_offset );
        }
        else
        {
            curstate = new StateDescription(def, mpositer.get_position().line - ln_offset ); // create an empty state
            curtemplatecontext->addState( def, curstate );
        }

    }

    void StateGraphParser::seenstateend()
    {
        if ( curinitialstateflag )
        {
            if ( curtemplatecontext->getInitialState() )
                throw parse_exception_semantic_error( "Attempt to define more than one initial state." );
            else curtemplatecontext->setInitialState( curstate );
        }
        if ( curfinalstateflag )
        {
            if ( curtemplatecontext->getFinalState() )
                throw parse_exception_semantic_error( "Attempt to define more than one final state." );
            else curtemplatecontext->setFinalState( curstate );
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
        progParser->initBodyParser( name, curcontext, ln_offset );
        
        programBody = progParser->bodyParser();
    }

    FunctionGraph* StateGraphParser::finishProgram()
    {
        return progParser->bodyParserResult();
    }

    void StateGraphParser::seenentry()
    {
        curstate->setEntryProgram( finishProgram() );
    }

    void StateGraphParser::seenexit()
    {
        curstate->setExitProgram( finishProgram() );
    }

    void StateGraphParser::seenhandle()
    {
        curstate->setHandleProgram( finishProgram() );
    }

    void StateGraphParser::seencondition()
    {
        assert( !curcondition );
        curcondition = conditionparser->getParseResult();
        assert( curcondition );
        conditionparser->reset();
    }

    void StateGraphParser::seenselect( iter_t s, iter_t f)
    {
        std::string state_id(s,f);
        StateDescription* next_state;
        if ( curtemplatecontext->getState( state_id ) != 0 )
        {
            next_state = curtemplatecontext->getState( state_id );
        }
        else
        {
            next_state = new StateDescription(state_id, 1); // create an empty state
            curtemplatecontext->addState( state_id, next_state );
        }
        assert( next_state );

        if (curcondition == 0)
            curcondition = new ConditionTrue;

        // this transition has a lower priority than the previous one
        curtemplatecontext->transitionSet( curstate, next_state, curcondition, rank--, mpositer.get_position().line - ln_offset );
        curcondition = 0;
    }

    void StateGraphParser::seenstatecontextend()
    {
        assert( curtemplatecontext );
        assert( ! curstate );

        // Check if the Initial and Final States are ok.
        if ( curtemplatecontext->getInitialState() == 0 )
            throw parse_exception_semantic_error("No initial state defined.");
        if ( curtemplatecontext->getFinalState() == 0 )
            throw parse_exception_semantic_error("No final state defined.");

        if ( curtemplatecontext->getStateList().empty() )
            throw parse_exception_semantic_error("No states defined in this state context !");

        // Check if all States are defined.
        const std::map<std::string, StateDescription*>& states = curtemplatecontext->getStates();
        for( std::map<std::string, StateDescription*>::const_iterator it = states.begin(); it != states.end(); ++it)
        {
            assert( dynamic_cast<StateDescription*>( it->second ) );
            StateDescription* sd = static_cast<StateDescription*>( it->second );
            if ( !sd->isDefined() )
                throw parse_exception_semantic_error("State " + it->first + " not defined, but referenced to.");
        }

        // prepend the commands for initialising the subMachine
        // variables..
        assert( curtemplatecontext->getInitCommand() == 0);
        if ( varinitcommands.size() > 1 )
            {
                CommandComposite* comcom = new CommandComposite;
                for ( std::vector<CommandInterface*>::iterator i = varinitcommands.begin();
                      i != varinitcommands.end(); ++i )
                    comcom->add( *i );
                curtemplatecontext->setInitCommand( comcom );
            }
        else if (varinitcommands.size() == 1 )
            curtemplatecontext->setInitCommand( *varinitcommands.begin() );

        varinitcommands.clear();

        // remove temporary subMachine peers from current task.
        for( StateMachine::ChildList::const_iterator it= curtemplatecontext->getChildren().begin();
             it != curtemplatecontext->getChildren().end(); ++it ) {
            ParsedStateMachine* psc = dynamic_cast<ParsedStateMachine*>( *it );
            if (psc)
                context->removePeer( psc->getTaskContext()->getName() );
        }

        // finally : 
        curtemplatecontext->finish();

        delete progParser;
        progParser = 0;

        // reset stack to task.
        valuechangeparser->setStack(context);
//         commandparser->setStack(context);
        expressionparser->setStack(context);
        conditionparser->setStack(context);

        StateMachineBuilder* scb = new StateMachineBuilder( curtemplatecontext );
        contextbuilders[curcontextname] = scb;

        curcontextname.clear();
        curtemplatecontext = 0;
        curcontextname.clear();
        curcontext = 0;
    }

    std::vector<ParsedStateMachine*> StateGraphParser::parse( iter_t& begin, iter_t end )
    {
        skip_parser_t skip_parser = SKIP_PARSER;
        iter_pol_t iter_policy( skip_parser );
        scanner_pol_t policies( iter_policy );
        scanner_t scanner( begin, end, policies );

        // reset the condition-transition priority.
        rank = 0;

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
            std::vector<ParsedStateMachine*> ret = mystd::values( rootcontexts );
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
    }

    void StateGraphParser::clear() {
        delete curcondition;
        curcondition = 0;
        // we own curstate, but not through this pointer...
        curstate = 0;
        delete curnonprecstate;
        curnonprecstate = 0;
        // we own curcontextbuilder, but not through this pointer...
        curcontextbuilder = 0;
        delete curinstantiatedcontext;
        curinstantiatedcontext = 0;
        // If non null, there was a parse-error, undo all :
        if ( curtemplatecontext )
        {
          // remove all 'this' data factories
          curtemplatecontext->getTaskContext()->dataFactory.unregisterObject( "this" );
          curtemplatecontext->getTaskContext()->methodFactory.unregisterObject( "this" );

          // remove temporary subMachine peers from current task.
          for( StateMachine::ChildList::const_iterator it= curtemplatecontext->getChildren().begin();
               it != curtemplatecontext->getChildren().end(); ++it ) {
              ParsedStateMachine* psc = dynamic_cast<ParsedStateMachine*>( *it );
              if (psc)
                  context->removePeer( psc->getTaskContext()->getName() );
          }

          // remove the type from __states
          context->getPeer("__states")->removePeer( curtemplatecontext->getTaskContext()->getName() ) ;

          // will also delete all children : 
          delete curtemplatecontext;
          curtemplatecontext = 0;
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
            std::vector<std::string> names = __s->getPeerList();
            context->removePeer("__states");
            for ( std::vector<std::string>::iterator it= names.begin(); it!= names.end(); ++it) {
                if ( rootcontexts.count(*it) != 0 ) {
                    delete rootcontexts[*it];
                    rootcontexts.erase(*it);
                }
                __s->removePeer( *it );
            }
            delete __s;
        }

    }

    void StateGraphParser::seenstatecontextname( iter_t begin, iter_t end ) {
        assert( curcontextname.empty() );
        // the 'type' of the SC :
        curcontextname = std::string ( begin, end );

        // store the SC in the TaskContext current.__states
        // __states is a storage for template SC's
        TaskContext* __s = context->getPeer("__states");
        if ( __s == 0 ) {
            // install the __states if not yet present.
            __s = new TaskContext("__states", context->getProcessor() );
            context->addPeer( __s );
        }

        // check if the type exists already :
        if ( __s->hasPeer( curcontextname ) )
            throw parse_exception_semantic_error("State Context " + curcontextname + " redefined.");

        curtemplatecontext = new ParsedStateMachine();
        // Connect the new SC to the relevant contexts.
        // 'sc' acts as a stack for storing variables.
        curcontext = new TaskContext(curcontextname, context->getProcessor() );
        __s->addPeer( curcontext );   // store in __states.
        curtemplatecontext->setTaskContext( curcontext ); // store.
        
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
        curtemplatecontext->setName( curcontextname, false );
    }

    void StateGraphParser::storeOffset() {
        ln_offset = mpositer.get_position().line - 1;
    }

    void StateGraphParser::saveText( iter_t begin, iter_t end ) {
        assert ( curtemplatecontext != 0 );
        curtemplatecontext->setText( std::string( begin, end) );
    }

    void StateGraphParser::inpreconditions() {
        // we postpone the current state
        assert( curnonprecstate == 0 );
        curnonprecstate = curstate->postponeState();
    }

    void StateGraphParser::seenpreconditions() {
        curtemplatecontext->transitionSet( curstate, curnonprecstate, new ConditionTrue, rank--, mpositer.get_position().line - ln_offset );
        curstate->setDefined( true );
        curstate = curnonprecstate;
        curnonprecstate = 0;
    }

    void StateGraphParser::seenrootcontextinstantiation() {
        if( rootcontexts.find( curinstcontextname ) != rootcontexts.end() )
            throw parse_exception_semantic_error( "Root context \"" + curinstcontextname + "\" already defined." );
        rootcontexts[curinstcontextname] = curinstantiatedcontext;
        // recursively set the name of this SC and all subs :
        curinstantiatedcontext->setName( curinstcontextname, true );

        // add it to the "states" (all instantiated) of the current context :
        TaskContext* __s = context->getPeer("states");
        if ( __s == 0 ) {
            // install the __states if not yet present.
            __s = new TaskContext("states", context->getProcessor() );
            context->addPeer( __s );
            __s->addPeer(context, "task");
        }

        // check if the type exists already :
        if ( __s->hasPeer( curinstcontextname ) )
            throw parse_exception_semantic_error("Task '"+context->getName()+"' has already a State Context '" + curinstcontextname + "' .");
        __s->connectPeers( curinstantiatedcontext->getTaskContext() );

        curinstantiatedcontext = 0;
        curinstcontextname.clear();
    }

    void StateGraphParser::seensubMachineinstantiation() {
        if( curtemplatecontext->getSubMachine( curinstcontextname ) != 0 )
            throw parse_exception_semantic_error( "SubMachine \"" + curinstcontextname + "\" already defined." );

        // Since we parse in the task context, we must _temporarily_
        // make each subMachine a peer of the task so that we can access
        // its methods.
        if ( !context->addPeer( curinstantiatedcontext->getTaskContext() ) )
            throw parse_exception_semantic_error(
                "Name clash: name of instantiated context \"" + curinstcontextname +
                "\"  already used as peer name in task '"+context->getName()+"'." );
            
        curtemplatecontext->addSubMachine( curinstcontextname, curinstantiatedcontext );
        // we add this statecontext to the list of variables, so that the
        // user can refer to it by its name...
        detail::ParsedAlias<std::string>* pv = new detail::ParsedAlias<std::string>( curinstantiatedcontext->getNameDS() );
        context->attributeRepository.setValue( curinstcontextname, pv );

        curinstantiatedcontext = 0;
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
        ParsedStateMachine* nsc = curcontextbuilder->build();

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
        CommandInterface* assigncommand = valuechangeparser->assignCommand();
        // if an assignment was given, add it.
        if ( assigncommand )
            varinitcommands.push_back( assigncommand );
        valuechangeparser->reset();
    }

  void StateGraphParser::seencontextparam() {
    curtemplatecontext->addParameter( valuechangeparser->lastParsedDefinitionName(), valuechangeparser->lastDefinedValue() );
    valuechangeparser->reset();
  }

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

}
