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

#define BOOST_SPIRIT_DEBUG 1

#include "execution/parser-debug.hpp"
#include "execution/parse_exception.hpp"
#include "execution/StateGraphParser.hpp"
#include "execution/StateContextBuilder.hpp"
#include "execution/DataSourceFactory.hpp"
#include "execution/GlobalFactory.hpp"
#include "execution/mystd.hpp"

#include "execution/Processor.hpp"
#include "corelib/CommandNOP.hpp"
#include "corelib/ConditionTrue.hpp"
#include "execution/DataSourceCondition.hpp"
#include "execution/ParsedValue.hpp"
#include "execution/EventHandle.hpp"
#include "execution/StateDescription.hpp"
#include "execution/CommandProgramEndToken.hpp"
#include "execution/ParsedStateContext.hpp"
#include "corelib/CommandEmitEvent.hpp"

#include <iostream>
#include <functional>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/call_traits.hpp>

namespace ORO_Execution
{
    using namespace boost;
    using boost::bind;
    using namespace ORO_CoreLib;
    using namespace ORO_Execution::detail;

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
        assertion<std::string> expect_semicolon("Semi colon expected after statement.");
        assertion<std::string> expect_open_parenth( "Open parenthesis expected." );
        assertion<std::string> expect_close_parenth( "Open parenthesis expected." );

        // a utility DataSourceFactory that implements the
        // DataSourceFactoryInterface using a given map of strings to
        // DataSources...
        class MapDataSourceFactory
            : public DataSourceFactoryInterface
        {
            typedef std::map<std::string, DataSourceBase::shared_ptr> map_t;
            map_t mmap;
        public:
            MapDataSourceFactory( const map_t& map );
            ~MapDataSourceFactory();

            std::vector<std::string> dataNames() const;
            bool hasData( const std::string& s ) const;
            std::vector<ArgumentDescription> getArgumentList( const std::string& method ) const;
            PropertyBag getArgumentSpec( const std::string& method ) const;
            DataSourceBase* create( const std::string& name, const PropertyBag& args ) const;
            DataSourceBase* create( const std::string& name, const std::vector<DataSourceBase*>& args ) const;
            // we don't know how to define this here -> relegating to
            // the subclass..
            virtual std::string getDescription( const std::string& source ) const = 0;
        };

        MapDataSourceFactory::MapDataSourceFactory( const map_t& map )
            : mmap( map )
        {
        }

        MapDataSourceFactory::~MapDataSourceFactory()
        {
        }

        std::vector<std::string> MapDataSourceFactory::dataNames() const
        {
            return mystd::keys( mmap );
        }

        bool MapDataSourceFactory::hasData( const std::string& s ) const
        {
            return mmap.find( s ) != mmap.end();
        }

        std::vector<ArgumentDescription> MapDataSourceFactory::getArgumentList( const std::string& method ) const
        {
            // we don't support arguments here...
            return std::vector<ArgumentDescription>();
        }

        PropertyBag MapDataSourceFactory::getArgumentSpec( const std::string& method ) const
        {
            return PropertyBag();
        }

        DataSourceBase* MapDataSourceFactory::create( const std::string& name, const PropertyBag& args ) const
        {
            if ( !args.empty() )
                throw wrong_number_of_args_exception( 0, args.getProperties().size() );
            map_t::const_iterator reti = mmap.find( name );
            if ( reti == mmap.end() )
                throw name_not_found_exception();
            else
                return reti->second.get();
        }

        DataSourceBase* MapDataSourceFactory::create( const std::string& name, const std::vector<DataSourceBase*>& args ) const
        {
            if ( !args.empty() )
                throw wrong_number_of_args_exception( 0, args.size() );
            map_t::const_iterator reti = mmap.find( name );
            if ( reti == mmap.end() )
                throw name_not_found_exception();
            else
                return reti->second.get();
        }

        class InStateDataSource
            : public DataSource<bool>
        {
            DataSource<StateContext*>::shared_ptr msc;
            DataSource<std::string>::shared_ptr mst;
        public:
            // Note that we assume that msc will always evaluate to a ParsedStateContext.
            InStateDataSource( DataSource<StateContext*>* msc, DataSource<std::string>* mst );
            ~InStateDataSource();

            bool get() const;
            void reset();
            InStateDataSource* clone() const;
            InStateDataSource* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
        };

        InStateDataSource::InStateDataSource( DataSource<StateContext*>* sc, DataSource<std::string>* st )
            : msc( sc ), mst( st )
        {
        }

        InStateDataSource::~InStateDataSource()
        {
        }

        bool InStateDataSource::get() const
        {
            return static_cast<ParsedStateContext*>( msc->get() )->inState( mst->get() );
        }

        void InStateDataSource::reset()
        {
            msc->reset();
            mst->reset();
        }

        InStateDataSource* InStateDataSource::clone() const
        {
            return new InStateDataSource( msc.get(), mst.get() );
        }

        InStateDataSource* InStateDataSource::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
        {
            return new InStateDataSource( msc->copy( alreadyCloned ), mst->copy( alreadyCloned ) );
        }

        // a DataSourceFactory that allows the user to refer to data
        // in a subcontext.
        class SubContextDataSourceFactory
            : public MapDataSourceFactory
        {
            ParsedStateContext* mpsc;
            DataSource<StateContext*>::shared_ptr mpscds;
            std::string mscn;
            static const char* instateparamname;
            static const char* instateparamdesc;
        public:
            virtual ~SubContextDataSourceFactory();
            // pscds is the DataSource that generated commands will
            // use to refer to the subcontext.
            SubContextDataSourceFactory( ParsedStateContext* psc, DataSource<StateContext*>* pscds, const std::string& name );

            std::vector<std::string> dataNames() const;
            bool hasData( const std::string& s ) const;
            std::vector<ArgumentDescription> getArgumentList( const std::string& method ) const;
            PropertyBag getArgumentSpec( const std::string& method ) const;
            DataSourceBase* create( const std::string& name, const PropertyBag& args ) const;
            DataSourceBase* create( const std::string& name, const std::vector<DataSourceBase*>& args ) const;
            std::string getDescription( const std::string& source ) const;
        };

        const char* SubContextDataSourceFactory::instateparamname =
            "state";
        const char* SubContextDataSourceFactory::instateparamdesc =
            "A string with the name of the state that this statecontext might be in.";

        SubContextDataSourceFactory::~SubContextDataSourceFactory()
        {
        }

        SubContextDataSourceFactory::SubContextDataSourceFactory(
            ParsedStateContext* psc, DataSource<StateContext*>* pscds, const std::string& name )
            : MapDataSourceFactory( psc->getReadOnlyValues() ), mpsc( psc ), mpscds( pscds ), mscn( name )
        {
          assert( !MapDataSourceFactory::hasData( "inState" ) );
        }

        std::vector<std::string> SubContextDataSourceFactory::dataNames() const
        {
            std::vector<std::string> ret = MapDataSourceFactory::dataNames();
            ret.push_back( "inState" );
            return ret;
        }

        bool SubContextDataSourceFactory::hasData( const std::string& s ) const
        {
            return s == "inState" || MapDataSourceFactory::hasData( s );
        }

        std::vector<ArgumentDescription> SubContextDataSourceFactory::getArgumentList( const std::string& method ) const
        {
            if ( method == "inState" )
            {
                std::vector<ArgumentDescription> ret;
                ret.push_back(
                    ArgumentDescription( instateparamname, instateparamdesc ) );
                return ret;
            }
            else
                return MapDataSourceFactory::getArgumentList( method );
        }

        PropertyBag SubContextDataSourceFactory::getArgumentSpec( const std::string& method ) const
        {
            if ( method == "inState" )
            {
                PropertyBag ret;
                ret.add( new Property<std::string>( instateparamname, instateparamdesc, "" ) );
                return ret;
            }
            else return MapDataSourceFactory::getArgumentSpec( method );
        }

        DataSourceBase* SubContextDataSourceFactory::create( const std::string& name, const PropertyBag& args ) const
        {
            if ( name == "inState" )
            {
                PropertyBag::PropertyContainerType props = args.getProperties();
                if ( props.size() != 1 )
                    throw wrong_number_of_args_exception( 1, props.size() );
                Property<std::string>* prop = dynamic_cast<Property<std::string>*>( props[0] );
                if ( ! prop )
                    throw wrong_types_of_args_exception( 1 );
                DataSource<std::string>* arg = new VariableDataSource<std::string>( prop->get() );
                return new InStateDataSource( mpscds.get(), arg );
            }
            else return MapDataSourceFactory::create( name, args );
        }

        DataSourceBase* SubContextDataSourceFactory::create( const std::string& name, const std::vector<DataSourceBase*>& args ) const
        {
            if ( name == "inState" )
            {
                if ( args.size() != 1 )
                    throw wrong_number_of_args_exception( 1, args.size() );
                DataSource<std::string>* arg = dynamic_cast<DataSource<std::string>*>( args[0] );
                if ( ! arg )
                    throw wrong_types_of_args_exception( 1 );
                return new InStateDataSource( mpscds.get(), arg );
            }
            else return MapDataSourceFactory::create( name, args );
        }

        std::string SubContextDataSourceFactory::getDescription( const std::string& source ) const
        {
            if ( !hasData( source ) )
                throw name_not_found_exception();
            else if ( source == "inState" )
                return "Return whether this statecontext is in the given state ?";
            else
                return "Member variable \"" + source + "\" of SubContext \"" + mscn + "\"";
        }
    }

    StateGraphParser::StateGraphParser( iter_t& positer,
                                        Processor* proc,
                                        GlobalFactory* e )
        : context( proc, e ),
          mpositer( positer ),
          curtemplatecontext( 0 ),
          curinstantiatedcontext( 0 ),
          curcontextbuilder( 0 ),
          curinitialstateflag( false ),
          curfinalstateflag( false ),
          curstate( 0 ),
          curnonprecstate( 0 ),
          curprogram( 0 ),
          curhand( 0 ),
          curevent( 0 ),
          curcondition( 0 ),
          conditionparser( context ),
          commandparser( context ),
          valuechangeparser( context ),
          expressionparser( context )
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
        BOOST_SPIRIT_DEBUG_RULE( eventhandledecl );
        BOOST_SPIRIT_DEBUG_RULE( subcontextdecl );
        BOOST_SPIRIT_DEBUG_RULE( statecontent );
        BOOST_SPIRIT_DEBUG_RULE( statecontentline );
        BOOST_SPIRIT_DEBUG_RULE( entry );
        BOOST_SPIRIT_DEBUG_RULE( preconditions );
        BOOST_SPIRIT_DEBUG_RULE( handle );
        BOOST_SPIRIT_DEBUG_RULE( transitions );
        BOOST_SPIRIT_DEBUG_RULE( exit );
        BOOST_SPIRIT_DEBUG_RULE( eeline );
        BOOST_SPIRIT_DEBUG_RULE( varchanges );
        BOOST_SPIRIT_DEBUG_RULE( eecommand );
        BOOST_SPIRIT_DEBUG_RULE( handleline );
        BOOST_SPIRIT_DEBUG_RULE( handlecommand );
        BOOST_SPIRIT_DEBUG_RULE( docommand );
        BOOST_SPIRIT_DEBUG_RULE( statecommand );
        BOOST_SPIRIT_DEBUG_RULE( transline );
        BOOST_SPIRIT_DEBUG_RULE( selectcommand );
        BOOST_SPIRIT_DEBUG_RULE( disconnectevent );
        BOOST_SPIRIT_DEBUG_RULE( connectevent );
        BOOST_SPIRIT_DEBUG_RULE( emitcommand );
        BOOST_SPIRIT_DEBUG_RULE( brancher );
        BOOST_SPIRIT_DEBUG_RULE( selector );
        BOOST_SPIRIT_DEBUG_RULE( eventbinding );
        BOOST_SPIRIT_DEBUG_RULE( contextinstarguments );
        BOOST_SPIRIT_DEBUG_RULE( contextinstargument );
        BOOST_SPIRIT_DEBUG_RULE( contextmemvar );
        BOOST_SPIRIT_DEBUG_RULE( contextvariable );
        BOOST_SPIRIT_DEBUG_RULE( contextparam );
        BOOST_SPIRIT_DEBUG_RULE( contextconstant );
        BOOST_SPIRIT_DEBUG_RULE( contextalias );
        BOOST_SPIRIT_DEBUG_RULE( subcontextvarchange );

        newline = ch_p( '\n' );

        production = *( newline | statecontext ) >> *( newline | rootcontextinstantiation );

        rootcontextinstantiation =
            str_p( "RootContext" )
            >> contextinstantiation[ bind( &StateGraphParser::seenrootcontextinstantiation, this ) ];

        statecontext =
            "StateContext"
            >> expect_ident( commonparser.identifier[ bind( &StateGraphParser::seenstatecontextname, this, _1, _2 )] )
            >> !newline
            >> expect_open( ch_p( '{' ) )
            >> statecontextcontent
            >> expect_end( ch_p( '}' ) )
            >> newline[ bind( &StateGraphParser::seenstatecontextend, this ) ];

        // Zero or more declarations and Zero or more states
        statecontextcontent = ( *varline >> *( state | newline ) );

        varline = !vardec >> newline;

        vardec = eventhandledecl | subcontextdecl | contextmemvar | contextparam;

        contextmemvar = ( contextconstant | contextvariable | contextalias )[bind( &StateGraphParser::seencontextvariable, this )];
        contextconstant = valuechangeparser.constantDefinitionParser();
        contextvariable = valuechangeparser.variableDefinitionParser();
        contextalias = valuechangeparser.aliasDefinitionParser();

        contextparam = valuechangeparser.paramDefinitionParser()[bind( &StateGraphParser::seencontextparam, this )];

        eventhandledecl = "EventHandle"
                          >> expect_ident(commonparser.identifier[ bind( &StateGraphParser::handledecl, this, _1, _2) ]);
        subcontextdecl = "SubContext"
                         >> contextinstantiation[bind( &StateGraphParser::seensubcontextinstantiation, this )];

        contextinstantiation =
            expect_ident( commonparser.identifier[ bind( &StateGraphParser::seencontexttypename, this, _1, _2 )] )
            >> expect_ident( commonparser.identifier[ bind( &StateGraphParser::seeninstcontextname, this, _1, _2 )] )
            >> ( ! ( str_p( "(" )
                     >> !contextinstarguments
                     >> expect_close_parenth( str_p( ")" ) ) ) )[ bind( &StateGraphParser::seencontextinstantiation, this )];

        contextinstarguments =
            contextinstargument >> *( "," >> contextinstargument );

        contextinstargument =
            commonparser.identifier[ bind( &StateGraphParser::seencontextinstargumentname, this, _1, _2 )]
            >> "="
            >> expressionparser.parser()[ bind( &StateGraphParser::seencontextinstargumentvalue, this )];

        state =
          !( str_p( "initial" )[bind( &StateGraphParser::seeninitialstate,this )]
             | str_p( "final" )[bind( &StateGraphParser::seenfinalstate,this )] )
          >> str_p( "state" )
          >> expect_ident(commonparser.identifier[ bind( &StateGraphParser::statedef, this, _1, _2 ) ])
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

        entry = str_p( "entry" )[ bind( &StateGraphParser::inprogram, this)]
                >> expect_open(str_p("{"))>> *eeline >> expect_end(str_p("}"))[
                    bind( &StateGraphParser::seenentry, this )];

        exit = str_p( "exit" )[ bind( &StateGraphParser::inprogram, this)]
               >> expect_open(str_p("{")) >> *eeline >> expect_end(str_p("}"))[
                   bind( &StateGraphParser::seenexit, this )];

        eeline = !( varchanges | eecommand ) >> newline;

        varchanges = subcontextvarchange | (
            valuechangeparser.constantDefinitionParser()
            | valuechangeparser.variableDefinitionParser()
            | valuechangeparser.aliasDefinitionParser()
            | valuechangeparser.variableAssignmentParser()
            )[ bind( &StateGraphParser::seenvaluechange, this ) ];

        subcontextvarchange =
            "set"
            >> commonparser.identifier[bind( &StateGraphParser::seenscvcsubcontextname, this, _1, _2 )]
            >> "."
            >> commonparser.identifier[bind( &StateGraphParser::seenscvcparamname, this, _1, _2 )]
            >> ( str_p( "=" ) | "to" )
            >> expressionparser.parser()[bind( &StateGraphParser::seenscvcexpression, this )];

        handle = str_p( "handle" )[ bind( &StateGraphParser::inprogram, this)]
                 >> expect_open(str_p("{"))>> *handleline >> expect_end(str_p("}"))[
                     bind( &StateGraphParser::seenhandle, this )];

        handleline = !( varchanges | handlecommand) >> newline;

        handlecommand = docommand | statecommand;

        transitions = str_p( "transitions" )
                      >> expect_open(str_p("{"))>> *transline >> expect_end(str_p("}"));

        transline = !selectcommand >> newline;

        // In Entry/Exit : do something and setup the events :
        eecommand = disconnectevent | connectevent | docommand | statecommand;

        statecommand = emitcommand;

        emitcommand = str_p("emit") >> expect_open(str_p("(") )
                                    >> context.valueparser.parser()[bind( &StateGraphParser::seenemit, this ) ]
                                    >> expect_end( str_p( ")" ) );

        // You are able to do something everywhere except in transistions :
        docommand = str_p("do") >> commandparser.parser()[bind( &StateGraphParser::seencommand, this)];

        // You are only allowed to select a new state in transitions :
        selectcommand = brancher | selector;

        brancher = str_p( "if") >> conditionparser.parser()[ bind( &StateGraphParser::seencondition, this)]
                                >> expect_if(str_p( "then" ))>> !newline >> expect_if(selector);

        selector = str_p( "select" ) >> commonparser.identifier[ bind( &StateGraphParser::seenselect, this, _1, _2) ];

        connectevent = str_p( "connect" )
                       >> expect_ident(commonparser.identifier[ bind( &StateGraphParser::seenconnecthandler, this, _1, _2) ])
                       >> eventbinding[ bind( &StateGraphParser::seenconnect, this)];

        disconnectevent = str_p( "disconnect" ) >>
                          expect_ident(commonparser.identifier[ bind( &StateGraphParser::seendisconnecthandler, this, _1, _2) ]);

        eventbinding = expect_open(str_p("("))
                       // We use the valueparser to get the const_string actually.
                       >> context.valueparser.parser()[ bind( &StateGraphParser::eventselected, this )]
                       >> expect_comma(str_p(","))
                       >> commandparser.parser()[ bind( &StateGraphParser::seensink, this)]
                       >> expect_end( str_p(")") );
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
        }
        else
        {
            curstate = new StateDescription(def); // create an empty state
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
        assert( curprogram == 0 );
        assert( curhand == 0 );
        assert( curstate );
        curstate->setDefined( true );
        curstate = 0;
        curhandles.clear();
        curinitialstateflag = false;
        curfinalstateflag = false;
    }

    void StateGraphParser::inprogram()
    {
        assert( curprogram == 0 );
        curprogram = new ProgramGraph();
        curprogram->startProgram();
        curprogram->setName( "FIXME-GIVE_STATE_PROGRAMS_NAMES" );
    }

    ProgramGraph* StateGraphParser::finishProgram()
    {
        curprogram->returnProgram( new ConditionTrue );
        curprogram->proceedToNext( mpositer.get_position().line );
        curprogram->endProgram( context.processor, new CommandProgramEndToken );
        curprogram->reset();
        ProgramGraph* ret = curprogram;
        curprogram = 0;
        return ret;
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
        curcondition = conditionparser.getParseResult();
        assert( curcondition );
        conditionparser.reset();
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
            next_state = new StateDescription(state_id); // create an empty state
            curtemplatecontext->addState( state_id, next_state );
        }
        assert( next_state );

        if (curcondition == 0)
            curcondition = new ConditionTrue;

        // this transition has a lower priority than the previous one
        curtemplatecontext->transitionSet( curstate, next_state, curcondition, rank-- );
        curcondition = 0;
    }

    void StateGraphParser::seenemit()
    {
        const ParsedAliasValue<std::string>* res = dynamic_cast<const ParsedAliasValue<std::string>* >( context.valueparser.lastParsed()) ;

        if ( !res )
            throw parse_exception_semantic_error("Please specify a string containing the Event's name. e.g. \"eventname\".");

        std::string event_id( res->toDataSource()->get() );
        Event<void(void)>* eoi = Event<void(void)>::nameserver.getObject(event_id);
        if (eoi == 0 )
            throw parse_exception_semantic_error("Event \""+ event_id+ "\" can not be emitted because it is not created yet.");

        assert( curprogram );
        curprogram->setCommand( new CommandEmitEvent( eoi ) );
        curprogram->proceedToNext( new ConditionTrue, mpositer.get_position().line );
    }

    void StateGraphParser::handledecl( iter_t s, iter_t f)
    {
        std::string h_name(s, f);
        if ( curhandles.count( h_name ) != 0 )
            throw parse_exception_semantic_error("Event Handle " + h_name + " redefined.");

        curhandles[ h_name ] = new detail::EventHandle;
    }

    void StateGraphParser::seenconnecthandler( iter_t s, iter_t f)
    {
        std::string h_name(s, f);
        if ( curhandles.count( h_name ) == 0 )
            throw parse_exception_semantic_error("Event Handle " + h_name + " not declared.");

        curhand = curhandles[ h_name ];
    }

    void StateGraphParser::seenconnect()
    {
        assert( curhand );
        curhand->init( curevent, cureventsink );
        assert( curprogram );
        curprogram->setCommand( curhand->createConnect() );
        curprogram->proceedToNext( new ConditionTrue, mpositer.get_position().line );
        curhand = 0;
    }

    void StateGraphParser::seendisconnecthandler( iter_t s, iter_t f)
    {
        std::string h_name(s, f);
        if ( curhandles.count( h_name ) == 0 )
            throw parse_exception_semantic_error("Event Handle " + h_name + " not declared.");

        assert( curprogram );
        curprogram->setCommand( curhandles[ h_name ]->createDisconnect() );
        curprogram->proceedToNext( new ConditionTrue, mpositer.get_position().line );
    }

    void StateGraphParser::eventselected()
    {
        assert( !curevent );
        const ParsedAliasValue<std::string>* res = dynamic_cast<const ParsedAliasValue<std::string>* >( context.valueparser.lastParsed()) ;

        if ( !res )
            throw parse_exception_syntactic_error("Please specify a string containing the Event's name. e.g. \"eventname\".");

        std::string ev_name( res->toDataSource()->get() );
        if ( !Event<void(void)>::nameserver.isNameRegistered(ev_name) )
            throw parse_exception_semantic_error("Event " + ev_name + " not known.");

        curevent = Event<void(void)>::nameserver.getObject( ev_name );
    }

    void StateGraphParser::seensink()
    {
        assert( !cureventsink );
        CommandInterface *cresult = commandparser.getCommand();
        cureventsink = boost::bind( &CommandInterface::execute, cresult );
        delete commandparser.getImplTermCondition(); // we do not use this here
        commandparser.reset();
    }

    void StateGraphParser::seencommand()
    {
        CommandInterface *cresult = commandparser.getCommand();
        delete commandparser.getImplTermCondition(); // we do not use this here
        commandparser.reset();
        assert( curprogram );
        curprogram->setCommand( cresult );
        curprogram->proceedToNext( new ConditionTrue, mpositer.get_position().line );
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
            // an assign or definition in the entry, handle or
            // exit nodes...
            curprogram->setCommand( ac );
            // Since a valuechange does not add edges, we use this variant
            // to create one.
            curprogram->proceedToNext( new ConditionTrue, mpositer.get_position().line );
        }
    }

    void StateGraphParser::seenstatecontextend()
    {
        assert( curtemplatecontext );
        assert( ! curstate );
        assert( curhandles.empty() );

        // Check if the Initial and Final States are ok.
        if ( curtemplatecontext->getInitialState() == 0 )
            throw parse_exception_semantic_error("No initial state defined.");
        if ( curtemplatecontext->getFinalState() == 0 )
            throw parse_exception_semantic_error("No final state defined.");
        assert( dynamic_cast<StateDescription*>( curtemplatecontext->getInitialState() ) );
        StateDescription* initstate = static_cast<StateDescription*>( curtemplatecontext->getInitialState() );

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

        ProgramGraph* initentryprogram = initstate->getEntryProgram();
        if ( ! initentryprogram )
        {
            initentryprogram = emptyProgram();
            initstate->setEntryProgram( initentryprogram );
        }
        // prepend the commands for initialising the subcontext
        // parameters..
        for ( std::vector<CommandInterface*>::iterator i = preentrycommands.begin();
              i != preentrycommands.end(); ++i )
            initentryprogram->prependCommand( *i, mpositer.get_position().line );
        preentrycommands.clear();

        // remove the data factories we added to the context again..
        std::vector<std::string> subcontextnames = curtemplatecontext->getSubContextList();
        for ( std::vector<std::string>::iterator i = subcontextnames.begin();
              i != subcontextnames.end(); ++i )
            context.globalfactory->dataFactory().unregisterObject( *i );

        StateContextBuilder* scb = new StateContextBuilder( curtemplatecontext );
        contextbuilders[curcontextname] = scb;

        curcontextname.clear();
        curtemplatecontext = 0;
        curcontextname.clear();
        context.valueparser.clear();
    }

    std::vector<ParsedStateContext*> StateGraphParser::parse( iter_t& begin, iter_t end )
    {
        skip_parser_t skip_parser = SKIP_PARSER;
        iter_pol_t iter_policy( skip_parser );
        scanner_pol_t policies( iter_policy );
        scanner_t scanner( begin, end, policies );

        context.valueparser.clear();

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
            std::vector<ParsedStateContext*> ret = mystd::values( rootcontexts );
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
    }

    StateGraphParser::~StateGraphParser() {
        clear();
    }

    void StateGraphParser::clear() {
        delete curcondition;
        curcondition = 0;
        cureventsink = 0;
        // we don't own curevent, so let's not delete it...
        curevent = 0;
        // we own curhand, but not through this pointer...
        curhand = 0;
        for ( handlemap::iterator i = curhandles.begin();
              i != curhandles.end(); ++i )
            delete i->second;
        curhandles.clear();
        // we own curstate, but not through this pointer...
        curstate = 0;
        delete curnonprecstate;
        curnonprecstate = 0;
        // we own curcontextbuilder, but not through this pointer...
        curcontextbuilder = 0;
        delete curinstantiatedcontext;
        curinstantiatedcontext = 0;
        if ( curtemplatecontext )
        {
          // remove the data factories we added to the context again..
          std::vector<std::string> subcontextnames = curtemplatecontext->getSubContextList();
          for ( std::vector<std::string>::iterator i = subcontextnames.begin();
                i != subcontextnames.end(); ++i )
            context.globalfactory->dataFactory().unregisterObject( *i );
          delete curtemplatecontext;
          curtemplatecontext = 0;
        }
        for ( std::vector<CommandInterface*>::iterator i = preentrycommands.begin();
              i != preentrycommands.end(); ++ i )
          delete *i;
        preentrycommands.clear();
        for ( contextbuilders_t::iterator i = contextbuilders.begin();
              i != contextbuilders.end(); ++i )
          delete i->second;
        contextbuilders.clear();
    }

    void StateGraphParser::seenstatecontextname( iter_t begin, iter_t end ) {
        assert( curcontextname.empty() );
        curcontextname = std::string ( begin, end );
        curtemplatecontext = new ParsedStateContext();
    }

    void StateGraphParser::inpreconditions() {
        // we postpone the current state
        assert( curnonprecstate == 0 );
        curnonprecstate = curstate->postponeState();
    }

    void StateGraphParser::seenpreconditions() {
        curtemplatecontext->transitionSet( curstate, curnonprecstate, new ConditionTrue, rank-- );
        curstate->setDefined( true );
        curstate = curnonprecstate;
        curnonprecstate = 0;
    }

    void StateGraphParser::seenrootcontextinstantiation() {
        if( rootcontexts.find( curinstcontextname ) != rootcontexts.end() )
            throw parse_exception_semantic_error( "Root context \"" + curinstcontextname + "\" already defined." );
        rootcontexts[curinstcontextname] = curinstantiatedcontext;
        curinstantiatedcontext->setName( curinstcontextname );
        assert( curinstantiatedcontext->getInitialState() );
        StateDescription* initialstate = dynamic_cast<StateDescription*>( curinstantiatedcontext->getInitialState() );
        assert( initialstate );
        ProgramGraph* entryprog = initialstate->getEntryProgram();
        if ( !entryprog )
        {
            // it's possible that the initial state does not have an entryprogram
            entryprog = emptyProgram();
            initialstate->setEntryProgram( entryprog );
        }

        // prepend the commands for initialising the subcontext
        // parameters ( for root contexts, we do it in their own
        // initstate's entry method instead of in the entry method of the
        // initstate of their parent.
        for ( std::vector<CommandInterface*>::iterator i = preentrycommands.begin();
              i != preentrycommands.end(); ++i )
            entryprog->prependCommand( *i, mpositer.get_position().line );
        preentrycommands.clear();
        curinstantiatedcontext = 0;
        curinstcontextname.clear();
    }

    void StateGraphParser::seensubcontextinstantiation() {
        if( curtemplatecontext->getSubContext( curinstcontextname ) != 0 )
            throw parse_exception_semantic_error( "SubContext \"" + curinstcontextname + "\" already defined." );
        if ( context.globalfactory->dataFactory().factory( curinstcontextname ) != 0 )
            throw parse_exception_semantic_error(
                "Name clash: name of instantiated context \"" + curinstcontextname +
                "\"  already used." );
        DataSource<StateContext*>* dsc = curtemplatecontext->addSubContext( curinstcontextname, curinstantiatedcontext );
        // we add this statecontext to the list of variables, so that the
        // user can refer to it by its name...
        ParsedAliasValue<std::string>* pv = new ParsedAliasValue<std::string>( curinstantiatedcontext->getNameDS() );
        context.valueparser.setValue( curinstcontextname, pv );

        // we add a SubContextDataSourceFactory for this subcontext to
        // the global data source factory, so that we can support
        // subcontext introspection...
        SubContextDataSourceFactory* scdsf =
            new SubContextDataSourceFactory( curinstantiatedcontext, dsc, curinstcontextname );
        context.globalfactory->dataFactory().registerObject( curinstcontextname, scdsf );

        curinstantiatedcontext = 0;
        curinstcontextname.clear();
    }

    void StateGraphParser::seencontexttypename( iter_t begin, iter_t end ) {
        assert( curcontextbuilder == 0 );
        std::string name( begin, end );
        contextbuilders_t::iterator i = contextbuilders.find( name );
        if ( i == contextbuilders.end() )
            throw parse_exception_semantic_error( "StateContext \"" + name + "\" not defined." );
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
        DataSourceBase* value = expressionparser.getResult();
        // let's not forget this...
        expressionparser.dropResult();
        if ( curinstcontextparams.find( curcontextinstargumentname ) != curinstcontextparams.end() )
            throw parse_exception_semantic_error(
                "In initialisation of StateContext \"" + curinstcontextname +
                "\": Parameter \"" + curcontextinstargumentname +"\" initialised twice..." );
        curinstcontextparams[curcontextinstargumentname] = value;
        curcontextinstargumentname.clear();
    }

    void StateGraphParser::seencontextinstantiation() {
        ParsedStateContext* nsc = curcontextbuilder->build();

        contextparams_t params = nsc->getParameters();

        // first run over the given parameters to see if they all exist in
        // the context we're instantiating...
        for ( contextparamvalues_t::iterator i = curinstcontextparams.begin(); i != curinstcontextparams.end(); ++i )
        {
            contextparams_t::iterator j = params.find( i->first );
            if ( j == params.end() )
                throw parse_exception_semantic_error( "No argument \"" + i->first + "\" in this state context." );
        }

        for ( contextparams_t::iterator i = params.begin(); i != params.end(); ++i )
        {
            contextparamvalues_t::iterator j = curinstcontextparams.find( i->first );
            if ( j == curinstcontextparams.end() )
                throw parse_exception_semantic_error(
                    "No value given for argument \"" + i->first + "\" in instantiation of this state context." );
            preentrycommands.push_back( i->second->assignCommand( j->second, true ) );
        }

        curinstantiatedcontext = nsc;
        curcontextbuilder = 0;
        curinstcontextparams.clear();
    }

    void StateGraphParser::seencontextvariable() {
        CommandInterface* assigncommand = valuechangeparser.assignCommand();
        preentrycommands.push_back( assigncommand );
        curtemplatecontext->addReadOnlyVar( valuechangeparser.lastParsedDefinitionName(), valuechangeparser.lastDefinedValue()->toDataSource() );
    }

  void StateGraphParser::seencontextparam() {
    curtemplatecontext->addParameter( valuechangeparser.lastParsedDefinitionName(), valuechangeparser.lastDefinedValue()->clone() );
  }

  ProgramGraph* StateGraphParser::emptyProgram() {
    ProgramGraph* ret = new ProgramGraph();
    ret->startProgram();
    ret->setName( "FIXME-GIVE_STATE_PROGRAMS_NAMES" );
    ret->returnProgram( new ConditionTrue );
    ret->proceedToNext( mpositer.get_position().line );
    ret->endProgram( context.processor, new CommandProgramEndToken );
    ret->reset();
    return ret;
  }

  void StateGraphParser::seenscvcsubcontextname( iter_t begin, iter_t end )
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

  void StateGraphParser::seenscvcexpression()
  {
    assert( curtemplatecontext != 0 );
    ParsedStateContext* psc = curtemplatecontext->getSubContext( curscvccontextname );
    if ( ! psc )
      throw parse_exception_semantic_error(
        "Use of unknown subcontext \"" + curscvccontextname +
        "\"in subcontext parameter assignment." );
    ParsedValueBase* pvb = psc->getParameter( curscvcparamname );
    if ( !pvb )
      throw parse_exception_semantic_error(
          "Subcontext \"" + curscvccontextname +
          "\" does not have a parameter by the name \"" +
          curscvcparamname + "\"." );

    DataSourceBase* rhs = expressionparser.getResult();
    assert( rhs );
    CommandInterface* ac = pvb->assignCommand( rhs, false );
    // parameters should always be assignable...
    assert( ac );

    assert( curprogram );
    curprogram->setCommand( ac );
    curprogram->proceedToNext( new ConditionTrue, mpositer.get_position().line );

    curscvccontextname.clear();
    curscvcparamname.clear();
  }
}
