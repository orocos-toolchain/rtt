/***************************************************************************
  tag: Peter Soetens  Tue Jul 20 17:32:42 CEST 2004  ParsedStateMachine.cxx

                        ParsedStateMachine.cxx -  description
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

#include "execution/ParsedStateMachine.hpp"
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
     * @todo 
     * 1. add copy/clone semantics to StateInterface and StateMachine.
     * 2a. rewrite ParsedStateMachine::copy to use (1)
     * 2b. remove all old StateMachine in DataSource code ( code is deprecated by StateMachineCommands implementation)
     * 3. refactor SM copying code in StateGraphParser to this file (and vice versa).
     * 4. in the end, no dynamic_casts should be needed anymore.
     */

    /**
     * When a ParsedStateMachine is finished, these commands
     * are added to its TaskContext, such that it can be
     * controlled through scripting.
     * The commands are directed to its Processor.
     */
    struct StateMachineCommands
    {
        // owns us :
        ParsedStateMachine* _sc;
        VariableDataSource<StateMachineCommands*>::shared_ptr _this;
        //Processor* pcache;
    public:

        // ctxname is the unscoped name 'myctxt'
        StateMachineCommands(ParsedStateMachine* sc)
            : _sc(sc), _this( new VariableDataSource<StateMachineCommands*>(this) ) //, pcache( _sc->getTaskContext()->getProcessor() )
        {
            sc->getTaskContext()->commandFactory.registerObject("this", this->createCommandFactory() );
            sc->getTaskContext()->dataFactory.registerObject("this", this->createDataSourceFactory() );
        }

        CommandFactoryInterface* createCommandFactory() {
            // Add the state specific methods :
            // Special trick : we store the 'this' pointer in a DataSource, such that when
            // the created commands are copied, they also get the new this pointer.
            // This requires template specialisations on the TemplateFactory level.
            TemplateCommandFactory< DataSource<StateMachineCommands*> >* fact = newCommandFactory( static_cast< DataSource<StateMachineCommands*>* >(_this.get()) );
            fact->add("activate",command_ds(&StateMachineCommands::activate, &StateMachineCommands::isActive, "Activate this StateMachine to initial state"));
            fact->add("deactivate",command_ds(&StateMachineCommands::deactivate, &StateMachineCommands::isActive, "Deactivate this StateMachine from final state", true));
            fact->add("start",command_ds(&StateMachineCommands::start, &StateMachineCommands::isRunning, "Start this StateMachine from initial state"));
            fact->add("pause",command_ds(&StateMachineCommands::pause, &StateMachineCommands::isPaused, "Pause this StateMachine"));
            fact->add("step",command_ds(&StateMachineCommands::step, &StateMachineCommands::isPaused, "Step this StateMachine"));
            fact->add("reset",command_ds(&StateMachineCommands::reset, &StateMachineCommands::inInitial, "Reset this StateMachine to the initial state"));
            fact->add("stop",command_ds(&StateMachineCommands::stop, &StateMachineCommands::inFinal, "Stop this StateMachine to the final state"));
            return fact;
        }

        DataSourceFactoryInterface* createDataSourceFactory() {
            TemplateDataSourceFactory< DataSource<StateMachineCommands*> >* f = newDataSourceFactory(static_cast< DataSource<StateMachineCommands*>* >(_this.get()));
            f->add("inState", data_ds(&StateMachineCommands::inState, "Is the StateMachine in a given state ?", "State Name", "State Name") );
            f->add("getState", data_ds(&StateMachineCommands::getState, "The name of the current state. An empty string if not active.") );
            f->add("isActive", data_ds(&StateMachineCommands::isActive, "Is this StateMachine activated ?") );
            f->add("isRunning", data_ds(&StateMachineCommands::isRunning, "Is this StateMachine running ?") );
            f->add("isPaused", data_ds(&StateMachineCommands::isPaused, "Is this StateMachine paused ?") );
            f->add("inInitial", data_ds(&StateMachineCommands::inInitial, "Is this StateMachine in the initial state ?") );
            f->add("inFinal", data_ds(&StateMachineCommands::inFinal, "Is this StateMachine in the final state ?") );
            return f;
        }

        StateMachineCommands* copy(ParsedStateMachine* newsc, std::map<const DataSourceBase*, DataSourceBase*>& replacements ) const
        {
            // if this gets copied, all created commands will use the new instance of StateMachineCommands to
            // call the member functions. Further more, all future commands for the copy will also call the new instance
            // while future commands for the original will still call the original. 
            StateMachineCommands* tmp = new StateMachineCommands( newsc );
            replacements[ _this.get() ] = tmp->_this.get(); // put DS in map 
            return tmp;
        }

        bool inState(const std::string& state) const {
            if ( !this->isActive() )
                return false;
            return _sc->currentState()->getName() == state;
        }

        std::string getState() const {
            if ( !this->isActive() )
                return "";
            return _sc->currentState()->getName();
        }

        bool isActive() const {
            return _sc->isActive() && !_sc->inTransition();
        }

        bool inInitial() const {
            return _sc->getInitialState() == _sc->currentState() && !_sc->inTransition();
        }

        bool inFinal() const {
            return _sc->getFinalState() == _sc->currentState()  && !_sc->inTransition();
        }

        bool isRunning() const {
            return _sc->getTaskContext()->getProcessor()->getStateMachineStatus( _sc->getName() ) == Processor::StateMachineStatus::running;
        }

        bool isPaused() const {
            return _sc->getTaskContext()->getProcessor()->getStateMachineStatus( _sc->getName() ) == Processor::StateMachineStatus::paused;
        }

        bool activate() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->activateStateMachine( _sc->getName() );
        }
        bool start() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->startStateMachine( _sc->getName() );
        }
        bool step() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->stepStateMachine( _sc->getName() );
        }
        bool pause() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->pauseStateMachine( _sc->getName() );
        }
        bool reset() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->resetStateMachine( _sc->getName() );
        }
        bool stop() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->stopStateMachine( _sc->getName() );
        }
        bool deactivate() {
            // getName returns the scoped name eg : root.subst.myctxt.
            return _sc->getTaskContext()->getProcessor()->deactivateStateMachine( _sc->getName() );
        }
    };

    using ORO_CoreLib::ConditionInterface;

    std::vector<std::string> ParsedStateMachine::getSubMachineList() const {
        return mystd::keys( subMachines );
    }

    ParsedStateMachine* ParsedStateMachine::getSubMachine( const std::string& name ) const {
        SubMachineNameMap::const_iterator i = subMachines.find( name );
        if ( i == subMachines.end() )
            return 0;
        assert( dynamic_cast<ParsedStateMachine*>( i->second->get() ) );
        return static_cast<ParsedStateMachine*>( i->second->get() );
    }

    ParsedStateMachine* ParsedStateMachine::copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements ) const
    {
        /* Recursive copy :
         * First copy this SC, then its child SC's
         */
        std::map<const StateInterface*, StateDescription*> statemapping;
        ParsedStateMachine* ret = new ParsedStateMachine( this->getText() );

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
            // What is sure, is that each param
            // must also be in the attributerepository.
          assert( replacements.find( i->second->toDataSource() ) != replacements.end() );
          TaskAttributeBase* npvb = i->second->copy( replacements );
          ret->parametervalues[i->first] = npvb;
//           assert( replacements[i->second->toDataSource()] == npvb->toDataSource() );
          // XXX Why do we need to do this ?? the above assertion should be true already,
          // when npvb is made a copy of i->second.
          replacements[i->second->toDataSource()] = npvb->toDataSource();
          
        }

        // TODO : these DS'es are no longer used, since all goes through the StateGraphCommands now.
        for ( SubMachineNameMap::const_iterator i = subMachines.begin(); i != subMachines.end(); ++i )
        {
            // we first copy the subMachines, and add the datasources
            // containing their pointers to the replacements map, so
            // that the new commands will work on the correct
            // contexts...
            assert( dynamic_cast<ParsedStateMachine*>(  i->second->get() ) );
            ParsedStateMachine* oldcontext = static_cast<ParsedStateMachine*>(  i->second->get() );
            ParsedStateMachine* newcontext = oldcontext->copy( replacements );

            DataSource<StateMachine*>::shared_ptr ncds = new VariableDataSource<StateMachine*>( newcontext );
            ret->subMachines[i->first] = ncds;
            ret->addChild( newcontext ); // also copy tree info to StateMachine !
            newcontext->setParent( ret );
            replacements[i->second.get()] = ncds.get();
        }

        // Copy the InitCommand :
        if (this->getInitCommand()) {
            ret->setInitCommand( this->getInitCommand()->copy(replacements) );
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
                int line = j->get<3>();
                ret->transitionSet(fromState, toState, condition, rank, line );
            }
        }

        // put in attributeRepository
//         for ( VisibleReadOnlyValuesMap::const_iterator i = visiblereadonlyvalues.begin();
//               i != visiblereadonlyvalues.end(); ++i )
//         {
//           ret->visiblereadonlyvalues[i->first] = i->second->copy( replacements );
//         }

        // init the StateMachine itself :
        ret->setFinalState( statemapping[ getFinalState() ]);
        ret->setInitialState( statemapping[ getInitialState() ]);

        return ret;
    }

    ParsedStateMachine::~ParsedStateMachine() {
        // we own our states...
        for ( StateNameMap::iterator i = states.begin();
              i != states.end(); ++i )
            delete i->second;
        // we own our subMachines...
        for ( SubMachineNameMap::iterator i = subMachines.begin();
              i != subMachines.end(); ++i )
            delete i->second->get();
        if ( context && context->getPeer("states") )
            context->getPeer("states")->removePeer( context->getName() );
        delete context;
        delete sc_coms;
    }

    std::vector<std::string> ParsedStateMachine::getStateList() const {
        return mystd::keys( states );
    }

    StateDescription* ParsedStateMachine::getState( const std::string& name ) const{
        StateNameMap::const_iterator i = states.find( name );
        if ( i == states.end() )
            return 0;
        else return i->second;
    }

    ParsedStateMachine::ParsedStateMachine()
        : StateMachine( 0 ), context(0), sc_coms(0) // no parent, no task
    {
        nameds = new VariableDataSource<std::string>( "" );
    };

    ParsedStateMachine::ParsedStateMachine(const std::string& text)
        : StateMachine( 0 ) // no parent, no task
          ,_text(text), context(0), sc_coms(0)
    {
        nameds = new VariableDataSource<std::string>( "" );
    };

    void ParsedStateMachine::addState( const std::string& name, StateDescription* state ) {
        assert( states.find( name ) == states.end() );
        states[name] = state;
    }

    DataSource<StateMachine*>* ParsedStateMachine::addSubMachine( const std::string& name, ParsedStateMachine* sc ) {
        assert( subMachines.find( name ) == subMachines.end() );
        DataSource<StateMachine*>* newds = new VariableDataSource<StateMachine*>( sc );
        subMachines[name] = newds;
        this->addChild( sc );
        sc->setParent( this );
        context->addPeer( sc->getTaskContext() );
        return newds;
    }

    void ParsedStateMachine::addReadOnlyVar( const std::string& name, DataSourceBase* var )
    {
        //assert( visiblereadonlyvalues.find( name ) == visiblereadonlyvalues.end() );
        //visiblereadonlyvalues[name] = var;
    }
    void ParsedStateMachine::addParameter( const std::string& name, TaskAttributeBase* var )
    {
        assert( parametervalues.find( name ) == parametervalues.end() );
        parametervalues[name] = var;
        // every parameter is also a readonly var...
        // visiblereadonlyvalues[name] = var->toDataSource();
    }
    DataSourceBase* ParsedStateMachine::getReadOnlyVar( const std::string& name ) const
    {
        if( visiblereadonlyvalues.find( name ) == visiblereadonlyvalues.end() )
            return 0;
        return visiblereadonlyvalues.find(name)->second.get();
    }
    TaskAttributeBase* ParsedStateMachine::getParameter( const std::string& name ) const
    {
        if( parametervalues.find( name ) == parametervalues.end() )
            return 0;
        return parametervalues.find(name)->second;
    }

    ParsedStateMachine::VisibleWritableValuesMap ParsedStateMachine::getParameters() const
    {
        return parametervalues;
    }

    ParsedStateMachine::VisibleReadOnlyValuesMap ParsedStateMachine::getReadOnlyValues() const
    {
        assert(false);
        return visiblereadonlyvalues;
    }

    std::vector<std::string> ParsedStateMachine::getParameterNames() const
    {
        return mystd::keys( parametervalues );
    }

    std::vector<std::string> ParsedStateMachine::getReadOnlyValuesNames() const
    {
        return mystd::keys( visiblereadonlyvalues );
    }

    bool ParsedStateMachine::inState( const std::string& name ) const
    {
        if ( getState( name ) == 0 )
            return false;
        return currentState() == getState( name );
    }

    DataSource<std::string>* ParsedStateMachine::getNameDS() const
    {
        return nameds.get();
    }

    void ParsedStateMachine::setName( const std::string& name, bool recursive )
    {
        // set the StateMachine name
        this->_name = name;
        // set the datasource's name
        nameds->set( name );
        if ( recursive == false )
            return;
        for ( SubMachineNameMap::iterator i = subMachines.begin(); i != subMachines.end(); ++i )
        {
            std::string subname = name + "." + i->first;
            ParsedStateMachine* psc = static_cast<ParsedStateMachine*>( i->second->get() );
            psc->setName( subname, true );
            this->getTaskContext()->addPeer( psc->getTaskContext() );
        }
    }

    void ParsedStateMachine::finish()
    {
        // finish is only called on newly created PSCs.
        assert( sc_coms == 0);
        // Add the factory :
        sc_coms = new StateMachineCommands( this );
    }

}
