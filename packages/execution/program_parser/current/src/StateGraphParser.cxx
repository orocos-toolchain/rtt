#include "execution/StateGraphParser.hpp"

#include "execution/Processor.hpp"
#include "corelib/CommandNOP.hpp"
#include "execution/StateGraph.hpp"
#include "corelib/ConditionTrue.hpp"
#include "execution/DataSourceCondition.hpp"
#include "execution/ParsedValue.hpp"
#include "execution/EventHandle.hpp"
#include "corelib/StandardEventListener.hpp"
#include "execution/StateDescription.hpp"

#include <iostream>
#include <boost/bind.hpp>

namespace ORO_Execution
{
    using namespace boost;
    using boost::bind;
    using namespace ORO_CoreLib;
    using namespace ORO_Execution::detail;
    
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
    

    StateGraphParser::StateGraphParser( iter_t& positer,
                                        Processor* proc,
                                        const GlobalFactory* e )
        : context( proc, e ), mpositer( positer ),
          conditionparser( context ),
          commandparser( context ),
          valuechangeparser( context )
  {
    BOOST_SPIRIT_DEBUG_RULE( newline );
    BOOST_SPIRIT_DEBUG_RULE( line );
    BOOST_SPIRIT_DEBUG_RULE( content );
    BOOST_SPIRIT_DEBUG_RULE( state );
    BOOST_SPIRIT_DEBUG_RULE( production );
    BOOST_SPIRIT_DEBUG_RULE( vardec );
    BOOST_SPIRIT_DEBUG_RULE( entry  );
    BOOST_SPIRIT_DEBUG_RULE( handle );
    BOOST_SPIRIT_DEBUG_RULE( exit  );
    BOOST_SPIRIT_DEBUG_RULE( eecommand );
    BOOST_SPIRIT_DEBUG_RULE( docommand );
    BOOST_SPIRIT_DEBUG_RULE( selectcommand );
    BOOST_SPIRIT_DEBUG_RULE( brancher );
    BOOST_SPIRIT_DEBUG_RULE( selector );
    BOOST_SPIRIT_DEBUG_RULE( connectevent );
    BOOST_SPIRIT_DEBUG_RULE( disconnectevent );
    BOOST_SPIRIT_DEBUG_RULE( eventbinding );
    BOOST_SPIRIT_DEBUG_RULE( varline );
    BOOST_SPIRIT_DEBUG_RULE( eeline );
    BOOST_SPIRIT_DEBUG_RULE( handleline );
    BOOST_SPIRIT_DEBUG_RULE( transline );
    BOOST_SPIRIT_DEBUG_RULE( statevars );
    newline = ch_p( '\n' );

    // Zero or more declarations and Zero or more states 
    production = ( *varline >> *state )[bind( &StateGraphParser::finished, this)];

    varline = !vardec >> newline;

    vardec = ( str_p("Event Handle")
               >> expect_ident(commonparser.identifier[ bind( &StateGraphParser::handledecl, this, _1, _2) ]))
               | ( str_p("Initial State")
                   >> expect_ident(commonparser.identifier[ bind( &StateGraphParser::initstate, this, _1, _2) ])
                   );
    
    // a function is very similar to a program, but it also has a name
    state = (
       *newline
       >> str_p( "state" ) 
       >> expect_ident(commonparser.identifier[ bind( &StateGraphParser::statedef, this, _1, _2 ) ])
       >> !newline
       >> expect_open(ch_p( '{' ))
       >> content
       >> expect_end(ch_p( '}' ))
       >> newline ) [ bind( &StateGraphParser::seenstateend, this ) ];

    // the content of a program can be any number of lines
    // a line is not strictly defined in the sense of text-line,
    // a line can contain newlines.
    content = *line;

    // a line can be empty or contain a statement. Empty is
    // necessary, because comment's are skipped, but newline's
    // aren't.  So a line like "/* very interesting comment
    // */\n" will reach us as simply "\n"..
    line = !( statevars | entry | handle | transitions | exit ) >> newline;

    statevars = (
        valuechangeparser.constantDefinitionParser()
      | valuechangeparser.variableDefinitionParser()
      | valuechangeparser.aliasDefinitionParser()
      | valuechangeparser.variableAssignmentParser()
        )[ bind( &StateGraphParser::seenvaluechange, this ) ];


    entry = str_p( "entry" )[ bind( &StateGraphParser::inentry, this)]
        >> expect_open(str_p("{"))>> *eeline >> expect_end(str_p("}"));

    exit = str_p( "exit" )[ bind( &StateGraphParser::inexit, this)]
        >> expect_open(str_p("{")) >> *eeline >> expect_end(str_p("}"));

    eeline = !( statevars | eecommand[bind( &StateGraphParser::seenstatement, this)]) >> newline;
    
    handle = str_p( "handle" )[ bind( &StateGraphParser::inhandle, this)]
        >> expect_open(str_p("{"))>> *handleline >> expect_end(str_p("}"));

    handleline = !( statevars | docommand[bind( &StateGraphParser::seenstatement, this)]) >> newline;

    transitions = str_p( "transitions" )
        >> expect_open(str_p("{"))>> *transline >> expect_end(str_p("}"));

    transline = !selectcommand[bind( &StateGraphParser::seenstatement, this)] >> newline;

    // In Entry/Exit : do something and setup the events :
    eecommand = (disconnectevent | connectevent | docommand );
        

    // You are able to do something everywhere except in transistions :
    docommand = str_p("do") >> commandparser.parser()[bind( &StateGraphParser::seencommand, this)];

    // You are only allowed to select a new state in transitions :
    selectcommand = (brancher | selector);

    brancher = str_p( "if") >> conditionparser.parser()[ bind( &StateGraphParser::seencondition, this)]
                            >> expect_if(str_p( "then" ))>> !newline >> expect_if(selector);
    
    selector = str_p( "select" ) >> commonparser.identifier[ bind( &StateGraphParser::selecting, this, _1, _2) ];
    
    connectevent = str_p( "connect" )
        >> expect_ident(commonparser.identifier[ bind( &StateGraphParser::selecthandler, this, _1, _2) ])
        >> eventbinding[ bind( &StateGraphParser::seenconnect, this)];

    disconnectevent = str_p( "disconnect" ) >>
        expect_ident(commonparser.identifier[ bind( &StateGraphParser::disconnecthandler, this, _1, _2) ]);

    eventbinding = expect_open(str_p("("))
        // We use the valueparser to get the const_string actually.
        >> context.valueparser.parser()[ bind( &StateGraphParser::eventselected, this )]
        >> expect_comma(str_p(","))
        >> commandparser.parser()[ bind( &StateGraphParser::seensink, this)]
        >> expect_end( str_p(")") );

  }

    void StateGraphParser::syntaxerr()
    {
        throw parse_exception("Syntax error at line " + mpositer.get_position().line);
    }

    void StateGraphParser::initstate( iter_t s, iter_t f)
    {
        minit = std::string(s,f);
    }

    void StateGraphParser::statedef( iter_t s, iter_t f)
    {
        // This is a bit clumsy, but ok, because we indicate
        // that the initial state must be set before the first
        // state definition.
        
        if ( mstates.size() > 1 && minit == std::string("") )
            throw parse_exception("Initial State not set.");

        std::string def(s, f);
        if ( mstates.count( def ) != 0 )
            if ( (mstates[def])->isDefined() )
                throw parse_exception("state " + def + " redefined.");
            else
                {
                    mstate = mstates[def];
                }
        else
            mstate = mstates[def] = state_graph->newState(); // create an empty state
        
        // start defining this state
        state_graph->startState( mstate );
    }
    
    void StateGraphParser::seenstateend()
    {
        assert ( mstate );
        state_graph->endState();     // inform graph this one is done
        context.valueparser.clear(); // cleanup left over variables
        mstate = 0;
    }
 
    void StateGraphParser::inentry()
    {
        state_graph->selectEntryNode();
    }
    void StateGraphParser::inexit()
    {
        state_graph->selectExitNode();
    }
    void StateGraphParser::inhandle()
    {
        state_graph->selectHandleNode();
    }
    
    void StateGraphParser::seenstatement()
    {
        state_graph->proceedToNext();
    }

    void StateGraphParser::seencondition()
    {
        mcondition = conditionparser.getParseResult();
        assert( mcondition );
        conditionparser.reset();
    }

    void StateGraphParser::selecting( iter_t s, iter_t f)
    {
        std::string state_id(s,f);
        StateInterface* next_state;
        if ( mstates.count( state_id ) != 0 )
            {
                next_state = mstates[ state_id ];
            }
        else
            next_state = mstates[state_id] = state_graph->newState(); // create an empty state
        
        if (mcondition == 0)
            mcondition == new ConditionTrue;

        state_graph->transitionSet( mstate, next_state, mcondition );
        mcondition = 0;
    }

    void StateGraphParser::handledecl( iter_t s, iter_t f)
    {
        std::string h_name(s, f);
        if ( mhandles.count( h_name ) != 0 )
            throw parse_exception("Event Handle " + h_name + " redefined.");
            
        mhandles[ h_name ] = new detail::EventHandle;
    }

    void StateGraphParser::selecthandler( iter_t s, iter_t f)
    {
        std::string h_name(s, f);
        if ( mhandles.count( h_name ) == 0 )
            throw parse_exception("Event Handle " + h_name + " not declared.");
            
        mhand = mhandles[ h_name ];
    }

    void StateGraphParser::seenconnect()
    {
        assert( mhand );
        mhand->init( mevent, meventsink );
        state_graph->setCommand( mhand->createConnect() );
        state_graph->connectToNext( state_graph->currentNode(),  new ConditionTrue );
        mhand = 0;
    }

    void StateGraphParser::disconnecthandler( iter_t s, iter_t f)
    {
        std::string h_name(s, f);
        if ( mhandles.count( h_name ) == 0 )
            throw parse_exception("Event Handle " + h_name + " not declared.");
            
        state_graph->setCommand( mhandles[ h_name ]->createDisconnect() );
        state_graph->connectToNext( state_graph->currentNode(),  new ConditionTrue );
    }
    
    void StateGraphParser::eventselected()
    {
        const ParsedAliasValue<std::string>* res = dynamic_cast<const ParsedAliasValue<std::string>* >( context.valueparser.lastParsed()) ;

        if ( !res )
            throw parse_exception("Please specify a string containing the Event's name. e.g. \"eventname\".");
            
        std::string ev_name( res->toDataSource()->get() );
        if ( !EventInterface::nameserver.isNameRegistered(ev_name) )
            throw parse_exception("Event " + ev_name + " not known.");
        
        mevent = EventInterface::nameserver.getObject( ev_name );
    }

    void StateGraphParser::finished()
    {
        // Check if Initial State is ok.
        if ( mstates.count( minit ) == 0 || !mstates[minit]->isDefined() )
            throw parse_exception("Initial State " + minit + " not defined.");
        
        state_graph->initState( mstates[minit] );

        // Check if all States are defined.
        for( statemap::iterator it= mstates.begin(); it != mstates.end(); ++it)
            if ( !it->second->isDefined() )
                throw parse_exception("State " + it->first + " not defined, but referenced to.");
                
    }

    void StateGraphParser::seensink()
    {
        CommandInterface *cresult = commandparser.getCommand();
        meventsink = listener( &CommandInterface::execute, cresult );
        delete commandparser.getImplTermCondition(); // we do not use this here
        commandparser.reset();
    }

    void StateGraphParser::seencommand()
    {
        CommandInterface *cresult = commandparser.getCommand();
        delete commandparser.getImplTermCondition(); // we do not use this here
        commandparser.reset();
        state_graph->setCommand( cresult );
        state_graph->connectToNext( state_graph->currentNode(),  new ConditionTrue );
    }

    void StateGraphParser::seenvaluechange()
    {
        // some value changes generate a command, we need to add it to
        // the program.
        CommandInterface* ac = valuechangeparser.assignCommand();
        // and not forget to reset()..
        valuechangeparser.reset();
        if ( ac )
            {
                state_graph->setCommand( ac );
                // Since a valuechange does not add edges, we use this variant
                // to create one.
                state_graph->proceedToNext( new ConditionTrue );
            }
    }

  StateGraph* StateGraphParser::parse( iter_t& begin, iter_t end )
  {
    skip_parser_t skip_parser = SKIP_PARSER;
    iter_pol_t iter_policy( skip_parser );
    scanner_pol_t policies( iter_policy );
    scanner_t scanner( begin, end, policies );

    state_graph = new StateGraph();
    context.valueparser.clear();

    try {
      if ( ! production.parse( scanner ) )
      {
        std::cerr << "Syntax error at line "
                  << mpositer.get_position().line
                  << std::endl;
        delete state_graph;
        state_graph = 0;
        return 0;
      }
      return state_graph;
    }
    catch( const parser_error<std::string, iter_t>& e )
        {
            std::cerr << "Parse error at line "
                      << mpositer.get_position().line
                      << ": " << e.descriptor << std::endl;
            delete state_graph;
            state_graph = 0;
            return 0;
        }
    catch( const parser_error<GraphSyntaxErrors, iter_t>& e )
        {
            std::cerr << "Parse error at line "
                      << mpositer.get_position().line
                      << ": " << "expected one of : entry, handle, exit, transitions" << std::endl;
            delete state_graph;
            state_graph = 0;
            return 0;
        }
    catch( const parse_exception& e )
    {
      std::cerr << "Parse error at line "
                << mpositer.get_position().line
                << ": " << e.what() << std::endl;
      delete state_graph;
      state_graph = 0;
      return 0;
    }
  }
}
