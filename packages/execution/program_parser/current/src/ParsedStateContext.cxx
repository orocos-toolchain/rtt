/***************************************************************************
  tag: Peter Soetens  Tue Jul 20 17:32:42 CEST 2004  ParsedStateContext.cxx

                        ParsedStateContext.cxx -  description
                           -------------------
    begin                : Tue July 20 2004
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

#include "execution/ParsedStateContext.hpp"
#include "execution/DataSource.hpp"
#include "execution/Processor.hpp"
#include "execution/StateDescription.hpp"
#include "execution/TaskVariable.hpp"
#include "execution/mystd.hpp"
#include "execution/TaskContext.hpp"
#include "execution/TemplateCommandFactory.hpp"
#include "execution/TemplateDataSourceFactory.hpp"

namespace ORO_Execution {

    /**
     * When a ParsedStateContext is finished, these commands
     * are added to its TaskContext, such that it can be
     * controlled through scripting.
     * The commands are directed to its Processor.
     */
    struct StateContextCommands
    {
        // owns us :
        ParsedStateContext* _sc;
        VariableDataSource<StateContextCommands*>::shared_ptr _this;
        //Processor* pcache;
    public:

        // ctxname is the unscoped name 'myctxt'
        StateContextCommands(ParsedStateContext* sc)
            : _sc(sc), _this( new VariableDataSource<StateContextCommands*>(this) ) //, pcache( _sc->getTaskContext()->getProcessor() )
        {
            sc->getTaskContext()->commandFactory.registerObject("this", this->createCommandFactory() );
            sc->getTaskContext()->dataFactory.registerObject("this", this->createDataSourceFactory() );
        }

        CommandFactoryInterface* createCommandFactory() {
            // Add the state specific methods :
            // Special trick : we store the 'this' pointer in a DataSource, such that when
            // the created commands are copied, they also get the new this pointer.
            // This requires template specialisations on the TemplateFactory level.
            TemplateCommandFactory< DataSource<StateContextCommands*> >* fact = newCommandFactory( static_cast< DataSource<StateContextCommands*>* >(_this.get()) );
            fact->add("activate",command_ds(&StateContextCommands::activate, &StateContextCommands::isActive, "Activate this StateContext to initial state"));
            fact->add("deactivate",command_ds(&StateContextCommands::deactivate, &StateContextCommands::isActive, "Deactivate this StateContext from final state", false));
            fact->add("start",command_ds(&StateContextCommands::start, &StateContextCommands::isRunning, "Start this StateContext from initial state"));
            fact->add("pause",command_ds(&StateContextCommands::pause, &StateContextCommands::isPaused, "Pause this StateContext"));
            fact->add("step",command_ds(&StateContextCommands::step, &StateContextCommands::isPaused, "Pause this StateContext"));
            fact->add("reset",command_ds(&StateContextCommands::reset, &StateContextCommands::isActive, "Reset this StateContext to initial state"));
            fact->add("stop",command_ds(&StateContextCommands::stop, &StateContextCommands::isRunning, "Stop this StateContext to final state", false));
            return fact;
        }

        DataSourceFactoryInterface* createDataSourceFactory() {
            TemplateDataSourceFactory< DataSource<StateContextCommands*> >* f = newDataSourceFactory(static_cast< DataSource<StateContextCommands*>* >(_this.get()));
            f->add("inState", data_ds(&StateContextCommands::inState, "Is the StateContext in a given state ?", "State Name", "State Name") );
            f->add("getState", data_ds(&StateContextCommands::getState, "The name of the current state. An empty string if not active.") );
            f->add("isActive", data_ds(&StateContextCommands::isActive, "Is this StateContext activated ?") );
            f->add("isRunning", data_ds(&StateContextCommands::isRunning, "Is this StateContext running ?") );
            f->add("isPaused", data_ds(&StateContextCommands::isPaused, "Is this StateContext paused ?") );
            return f;
        }

        StateContextCommands* copy(ParsedStateContext* newsc, std::map<const DataSourceBase*, DataSourceBase*>& replacements ) const
        {
            // if this gets copied, all created commands will use the new instance of StateContextCommands to
            // call the member functions. Further more, all future commands for the copy will also call the new instance
            // while future commands for the original will still call the original. 
            StateContextCommands* tmp = new StateContextCommands( newsc );
            replacements[ _this.get() ] = tmp->_this.get(); // put DS in map 
            return tmp;
        }

        bool inState(const std::string& state) const {
            if ( !_sc->isActive() )
                return false;
            return _sc->currentState()->getName() == state;
        }

        std::string getState() const {
            if ( !_sc->isActive() )
                return "";
            return _sc->currentState()->getName();
        }

        bool isActive() const {
            return _sc->isActive();
        }

        bool isRunning() const {
            return _sc->getTaskContext()->getProcessor()->getStateContextStatus( _sc->getName() ) == Processor::StateContextStatus::running;
        }

        bool isPaused() const {
            return _sc->getTaskContext()->getProcessor()->getStateContextStatus( _sc->getName() ) == Processor::StateContextStatus::paused;
        }

        bool activate() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->activateStateContext( _sc->getName() );
        }
        bool start() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->startStateContext( _sc->getName() );
        }
        bool step() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->stepStateContext( _sc->getName() );
        }
        bool pause() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->pauseStateContext( _sc->getName() );
        }
        bool reset() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->resetStateContext( _sc->getName() );
        }
        bool stop() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->stopStateContext( _sc->getName() );
        }
        bool deactivate() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->deactivateStateContext( _sc->getName() );
        }
    };

    using ORO_CoreLib::ConditionInterface;

    std::vector<std::string> ParsedStateContext::getSubContextList() const {
        return mystd::keys( subcontexts );
    }

    ParsedStateContext* ParsedStateContext::getSubContext( const std::string& name ) const {
        SubContextNameMap::const_iterator i = subcontexts.find( name );
        if ( i == subcontexts.end() )
            return 0;
        assert( dynamic_cast<ParsedStateContext*>( i->second->get() ) );
        return static_cast<ParsedStateContext*>( i->second->get() );
    }

    ParsedStateContext* ParsedStateContext::copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements ) const
    {
        /* Recursive copy :
         * First copy this SC, then its child SC's
         */
        std::map<const StateInterface*, StateDescription*> statemapping;
        ParsedStateContext* ret = new ParsedStateContext( this->getText() );

        // first update the context's stack :
        // this allows us to reference all the ds's of this SC lateron.
        ret->setTaskContext( new TaskContext( this->getTaskContext()->getName(), this->getTaskContext()->getProcessor() ) );
        AttributeRepository* dummy = this->getTaskContext()->attributeRepository.copy( replacements );
        ret->getTaskContext()->attributeRepository = *dummy;
        delete dummy;
        // next, add the 'this' factories, since they were not copied with the TC.

        ret->nameds = nameds->copy( replacements );
        ret->sc_coms = sc_coms->copy( ret, replacements );

        // the parameters of the SC, similar to FunctionGraph's Arguments.
        for ( VisibleWritableValuesMap::const_iterator i = parametervalues.begin();
              i != parametervalues.end(); ++i )
        {
          TaskAttributeBase* npvb = i->second->copy( replacements );
          ret->parametervalues[i->first] = npvb;
//           replacements[i->second->toDataSource()] = npvb->toDataSource();
        }

        // TODO : these DS'es are no longer used, since all goes through the StateGraphCommands now.
        for ( SubContextNameMap::const_iterator i = subcontexts.begin(); i != subcontexts.end(); ++i )
        {
            // we first copy the subcontexts, and add the datasources
            // containing their pointers to the replacements map, so
            // that the new commands will work on the correct
            // contexts...
            assert( dynamic_cast<ParsedStateContext*>(  i->second->get() ) );
            ParsedStateContext* oldcontext = static_cast<ParsedStateContext*>(  i->second->get() );
            ParsedStateContext* newcontext = oldcontext->copy( replacements );

            DataSource<StateContextTree*>::shared_ptr ncds = new VariableDataSource<StateContextTree*>( newcontext );
            ret->subcontexts[i->first] = ncds;
            ret->addChild( newcontext ); // also copy tree info to StateContextTree !
            newcontext->setParent( ret );
            replacements[i->second.get()] = ncds.get();
        }

        // Copy the InitCommand :
        if (this->initc) {
            ret->setInitCommand( this->initc->copy(replacements) );
            // test :
            //ret->getInitCommand()->execute();
        } 

        // First make a copy of all states.  All states are either
        // known by their name or by a transition from or to them...
        for ( TransitionMap::const_iterator i = stateMap.begin(); i != stateMap.end(); ++i )
        {
            assert( dynamic_cast<StateDescription*>( i->first ) );
            StateDescription* fromState = static_cast<StateDescription*>( i->first );
            if( statemapping.find( fromState ) == statemapping.end() )
                statemapping[fromState] = fromState->copy( replacements );
            for ( TransList::const_iterator j = i->second.begin(); j != i->second.end(); ++j )
            {
                assert( dynamic_cast<StateDescription*>( j->get<1>() ) );
                StateDescription* toState = static_cast<StateDescription*>( j->get<1>() );
                if( statemapping.find( toState ) == statemapping.end() )
                    statemapping[toState] = toState->copy( replacements );
            }
        }
        for ( StateNameMap::const_iterator i = states.begin(); i != states.end(); ++i )
            if ( statemapping.find( i->second ) == statemapping.end() )
                statemapping[i->second] = i->second->copy( replacements );

        // now link the names to the new states
        for ( StateNameMap::const_iterator i = states.begin(); i != states.end(); ++i )
        {
            assert( statemapping.find( i->second ) != statemapping.end() );
            ret->states[i->first] = statemapping[i->second];
        }

        // next, copy the transitions
        for ( TransitionMap::const_iterator i = stateMap.begin(); i != stateMap.end(); ++i )
        {
            assert( statemapping.find( i->first ) != statemapping.end() );
            StateInterface* fromState = statemapping[i->first];
            for ( TransList::const_iterator j = i->second.begin(); j != i->second.end(); ++j )
            {
                ConditionInterface* condition = j->get<0>()->copy( replacements );
                assert( statemapping.find( j->get<1>() ) != statemapping.end() );
                StateInterface* toState = statemapping[j->get<1>()];
                int rank = j->get<2>();
                ret->stateMap[fromState].push_back( boost::make_tuple( condition, toState, rank ) );
            }
        }

        // put in attributeRepository
//         for ( VisibleReadOnlyValuesMap::const_iterator i = visiblereadonlyvalues.begin();
//               i != visiblereadonlyvalues.end(); ++i )
//         {
//           ret->visiblereadonlyvalues[i->first] = i->second->copy( replacements );
//         }

        ret->finistate = statemapping[finistate];
        ret->initstate = statemapping[initstate];
        ret->current = statemapping[current];

        return ret;
    }

    ParsedStateContext::~ParsedStateContext() {
        // we own our states...
        for ( StateNameMap::iterator i = states.begin();
              i != states.end(); ++i )
            delete i->second;
        // we own our subcontexts...
        for ( SubContextNameMap::iterator i = subcontexts.begin();
              i != subcontexts.end(); ++i )
            delete i->second->get();
        if ( context && context->getPeer("states") )
            context->getPeer("states")->removePeer( context->getName() );
        delete context;
        delete sc_coms;
    }

    std::vector<std::string> ParsedStateContext::getStateList() const {
        return mystd::keys( states );
    }

    StateDescription* ParsedStateContext::getState( const std::string& name ) const{
        StateNameMap::const_iterator i = states.find( name );
        if ( i == states.end() )
            return 0;
        else return i->second;
    }

    ParsedStateContext::ParsedStateContext()
        : StateContextTree( 0 ), context(0), sc_coms(0) // no parent, no task
    {
        nameds = new VariableDataSource<std::string>( "" );
    };

    ParsedStateContext::ParsedStateContext(const std::string& text)
        : StateContextTree( 0 ) // no parent, no task
          ,_text(text), context(0), sc_coms(0)
    {
        nameds = new VariableDataSource<std::string>( "" );
    };

    void ParsedStateContext::addState( const std::string& name, StateDescription* state ) {
        assert( states.find( name ) == states.end() );
        states[name] = state;
    }

    DataSource<StateContextTree*>* ParsedStateContext::addSubContext( const std::string& name, ParsedStateContext* sc ) {
        assert( subcontexts.find( name ) == subcontexts.end() );
        DataSource<StateContextTree*>* newds = new VariableDataSource<StateContextTree*>( sc );
        subcontexts[name] = newds;
        this->addChild( sc );
        sc->setParent( this );
        context->addPeer( sc->getTaskContext() );
        return newds;
    }

    void ParsedStateContext::addReadOnlyVar( const std::string& name, DataSourceBase* var )
    {
        //assert( visiblereadonlyvalues.find( name ) == visiblereadonlyvalues.end() );
        //visiblereadonlyvalues[name] = var;
    }
    void ParsedStateContext::addParameter( const std::string& name, TaskAttributeBase* var )
    {
        assert( parametervalues.find( name ) == parametervalues.end() );
        parametervalues[name] = var;
        // every parameter is also a readonly var...
        // visiblereadonlyvalues[name] = var->toDataSource();
    }
    DataSourceBase* ParsedStateContext::getReadOnlyVar( const std::string& name ) const
    {
        if( visiblereadonlyvalues.find( name ) == visiblereadonlyvalues.end() )
            return 0;
        return visiblereadonlyvalues.find(name)->second.get();
    }
    TaskAttributeBase* ParsedStateContext::getParameter( const std::string& name ) const
    {
        if( parametervalues.find( name ) == parametervalues.end() )
            return 0;
        return parametervalues.find(name)->second;
    }

    ParsedStateContext::VisibleWritableValuesMap ParsedStateContext::getParameters() const
    {
        return parametervalues;
    }

    ParsedStateContext::VisibleReadOnlyValuesMap ParsedStateContext::getReadOnlyValues() const
    {
        assert(false);
        return visiblereadonlyvalues;
    }

    std::vector<std::string> ParsedStateContext::getParameterNames() const
    {
        return mystd::keys( parametervalues );
    }

    std::vector<std::string> ParsedStateContext::getReadOnlyValuesNames() const
    {
        return mystd::keys( visiblereadonlyvalues );
    }

    bool ParsedStateContext::inState( const std::string& name ) const
    {
        if ( getState( name ) == 0 )
            return false;
        return currentState() == getState( name );
    }

    DataSource<std::string>* ParsedStateContext::getNameDS() const
    {
        return nameds.get();
    }

    void ParsedStateContext::setName( const std::string& name, bool recursive )
    {
        // set the StateContextTree name
        this->_name = name;
        // set the datasource's name
        nameds->set( name );
        if ( recursive == false )
            return;
        for ( SubContextNameMap::iterator i = subcontexts.begin(); i != subcontexts.end(); ++i )
        {
            std::string subname = name + "." + i->first;
            ParsedStateContext* psc = static_cast<ParsedStateContext*>( i->second->get() );
            psc->setName( subname, true );
            this->getTaskContext()->addPeer( psc->getTaskContext() );
        }
    }

    void ParsedStateContext::finish()
    {
        // finish is only called on newly created PSCs.
        assert( sc_coms == 0);
        // Add the factory :
        sc_coms = new StateContextCommands( this );
    }

}
