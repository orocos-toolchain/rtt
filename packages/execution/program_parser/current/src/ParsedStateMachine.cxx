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
#include <iostream>

#include <boost/lambda/lambda.hpp>

namespace ORO_Execution {
    using namespace detail;
    using namespace std;
    using namespace boost::lambda;
    /**
     * @todo 
     * 1. add copy/clone semantics to StateInterface and StateMachine.
     * 2a. rewrite ParsedStateMachine::copy to use (1)
     * 2b. remove all old StateMachine in DataSource code ( code is deprecated by StateMachineCommands implementation)
     * 3. refactor SM copying code in StateGraphParser to this file (and vice versa).
     * 4. in the end, no dynamic_casts should be needed anymore.
     */
    namespace detail {
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
            fact->add("activate",
                      command_ds(&StateMachineCommands::activate, &StateMachineCommands::isStrictlyActive,
                                 "Activate this StateMachine to initial state and enter request Mode."));
            fact->add("deactivate",
                      command_ds(&StateMachineCommands::deactivate, &StateMachineCommands::isActive,
                                 "Deactivate this StateMachine", true));
            fact->add("start",
                      command_ds(&StateMachineCommands::start, &StateMachineCommands::isRunning,
                                 "Start this StateMachine, enter run Mode."));
            fact->add("pause",
                      command_ds(&StateMachineCommands::pause, &StateMachineCommands::isPaused,
                                 "Pause this StateMachine, enter paused Mode."));
            fact->add("step",
                      command_ds(&StateMachineCommands::step, &StateMachineCommands::stepDone,
                                 "Step this StateMachine. When paused, step a single instruction or transition evaluation. \n"
                                 "When in requestMode, evaluate transitions and go to a next state, or if none, run handle."));
            fact->add("reset",
                      command_ds(&StateMachineCommands::reset, &StateMachineCommands::inInitial,
                                 "Reset this StateMachine to the initial state"));
            fact->add("stop",
                      command_ds(&StateMachineCommands::stop, &StateMachineCommands::inFinal,
                                 "Stop this StateMachine to the final state and enter request Mode."));
            fact->add("requestMode",
                      command_ds(&StateMachineCommands::waitForRequest, &StateMachineCommands::isStrictlyActive,
                                 "Enter state request mode (see requestState() and step() ). Command is done if ready for requestState() or step() command."));
            fact->add("requestState",
                      command_ds(&StateMachineCommands::requestState, &StateMachineCommands::inRequest,
                                 "Request to go to a particular state. Will succeed if there exists a valid transition from this state to the requested state.",
                                 "State", "The state to make the transition to."));
            return fact;
        }

        DataSourceFactoryInterface* createDataSourceFactory() {
            TemplateDataSourceFactory< DataSource<StateMachineCommands*> >* f = newDataSourceFactory(static_cast< DataSource<StateMachineCommands*>* >(_this.get()));
            f->add("inState", data_ds(&StateMachineCommands::inState, "Is the StateMachine in a given state ?", "State", "State Name") );
            f->add("inError", data_ds(&StateMachineCommands::inError, "Is this StateMachine in error ?") );
            f->add("getState", data_ds(&StateMachineCommands::getState, "The name of the current state. An empty string if not active.") );
            f->add("isActive", data_ds(&StateMachineCommands::isActive, "Is this StateMachine activated (possibly in transition) ?") );
            f->add("isRunning", data_ds(&StateMachineCommands::isRunning, "Is this StateMachine running ?") );
            f->add("isPaused", data_ds(&StateMachineCommands::isPaused, "Is this StateMachine paused ?") );
            f->add("inInitial", data_ds(&StateMachineCommands::inInitial, "Is this StateMachine in the initial state ?") );
            f->add("inFinal", data_ds(&StateMachineCommands::inFinal, "Is this StateMachine in the final state ?") );
            f->add("inRequest", data_ds(&StateMachineCommands::inRequest, "Is this StateMachine ready and waiting for requests ?") );
            f->add("inTransition", data_ds(&StateMachineCommands::inTransition, "Is this StateMachine executing a entry|handle|exit program ?") );
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

        bool waitForRequest() {
            return _sc->getTaskContext()->getProcessor()->requestModeStateMachine( _sc->getName() );
        }

        bool inRequest() const {
            // inRequest means ready for the next requestState command.
            return this->isStrictlyActive() && ! this->isRunning() && ! this->isPaused();
        }

        bool inError() const {
            // check if an error occured
            return _sc->inError();
        }

        bool requestState(const std::string& state) {
            return _sc->getTaskContext()->getProcessor()->requestStateStateMachine( _sc->getName(), state );
        }

        bool inState(const std::string& state) const {
            StateInterface* copy = _sc->currentState();
            if (copy == 0)
                return false;
            return copy->getName() == state;
        }

        std::string emptyString;
        const std::string& getState() const {
            StateInterface* copy = _sc->currentState();
            if (copy == 0)
                return emptyString;
            return copy->getName();
        }

        // Strictly active, means active and not in a transition.
        bool isStrictlyActive() const {
            return _sc->isActive() && !_sc->inTransition();
        }

        // just the fact of being activated, possibly in transition.
        bool isActive() const {
            return _sc->isActive();
        }

        // active and executing a program (entry, handle, exit)
        bool inTransition() const {
            return _sc->isActive() && _sc->inTransition();
        }

        bool inInitial() const {
            return _sc->getInitialState() == _sc->currentState() && !_sc->inTransition();
        }

        bool inFinal() const {
            return _sc->getFinalState() == _sc->currentState()  && !_sc->inTransition();
        }

        bool isRunning() const {
            // are we in run Mode ?
            return _sc->getTaskContext()->getProcessor()->getStateMachineStatus( _sc->getName() ) == Processor::StateMachineStatus::running;
        }

        bool stepDone() const {
            // in paused, return isPaused, in requestmode, return isStrictlyActive
            if ( isPaused() )
                return true;
            return isStrictlyActive();
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

    }

    ParsedStateMachine* ParsedStateMachine::copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate ) const
    {
        /* Recursive copy :
         * First copy this SC, then its child SC's
         */
        std::map<const StateInterface*, StateInterface*> statemapping;
        ParsedStateMachine* ret = new ParsedStateMachine();
        ret->_text = this->_text;
        ret->setName( this->_name, false);

        // first update the context's stack :
        // this allows us to reference all the ds's of this SC lateron.
        ret->setTaskContext( new TaskContext( this->getTaskContext()->getName(), this->getTaskContext()->getProcessor() ) );
        AttributeRepository* dummy = this->getTaskContext()->attributeRepository.copy( replacements, instantiate );
        ret->getTaskContext()->attributeRepository = *dummy;
        delete dummy;

        // next, add the 'this' factories, since they were not copied with the TC.

        ret->sc_coms = sc_coms->copy( ret, replacements );

        // the parameters of the SC, similar to FunctionGraph's Arguments.
        for ( VisibleWritableValuesMap::const_iterator i = parametervalues.begin();
              i != parametervalues.end(); ++i )
        {
            // What is sure, is that each param
            // must also be in the attributerepository.
            assert( ret->getTaskContext()->attributeRepository.getValue( i->first ) );
            ret->parametervalues[i->first] = ret->getTaskContext()->attributeRepository.getValue( i->first );
        }

        //**********************
        // TODO add copy method to StateMachine itself where all stuff below belongs :

        for ( ChildList::const_iterator i = getChildren().begin(); i != getChildren().end(); ++i )
        {
            // copy the submachines....
            assert( dynamic_cast<ParsedStateMachine*>( *i ) == static_cast<ParsedStateMachine*>( *i ));
            ParsedStateMachine* oldcontext = static_cast<ParsedStateMachine*>( *i );
            ParsedStateMachine* newcontext = oldcontext->copy( replacements );

            ret->addChild( newcontext ); // also copy tree info to StateMachine !
            newcontext->setParent( ret );
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
            if( statemapping.find( i->first ) == statemapping.end() ) {
                StateInterface* cpy = i->first->copy( replacements );
                ret->addState( cpy );
                statemapping[i->first] = cpy;
            }
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

        // finally, copy the preconditions
        for ( PreConditionMap::const_iterator i = precondMap.begin(); i != precondMap.end(); ++i )
        {
            assert( statemapping.find( i->first ) != statemapping.end() );
            StateInterface* tgtState = statemapping[i->first];
            ConditionInterface* condition = i->second.first->copy( replacements );
            int line = i->second.second;
            ret->preconditionSet( tgtState, condition, line );
        }

        // init the StateMachine itself :
        ret->setFinalState( statemapping[ getFinalState() ]);
        ret->setInitialState( statemapping[ getInitialState() ]);

        return ret;
    }

    ParsedStateMachine::~ParsedStateMachine() {
        // we own our states...
        for ( TransitionMap::iterator i = stateMap.begin();
              i != stateMap.end(); ++i )
            delete i->first;
        // we own our subMachines...
        for ( ChildList::const_iterator i = getChildren().begin();
              i != getChildren().end(); ++i )
            delete *i;
        // we own our conditions...
        for ( TransitionMap::iterator i = stateMap.begin();
              i != stateMap.end(); ++i )
            for ( TransList::iterator i2 = i->second.begin(); i2 != i->second.end(); ++i2 )
                delete get<0>( *i2 );  // delete the condition.
        if ( context && context->getPeer("states") )
            context->getPeer("states")->removePeer( context->getName() );
        delete context;
        delete sc_coms;
    }

    ParsedStateMachine::ParsedStateMachine()
        : StateMachine( 0 ), context(0), sc_coms(0) // no parent, no task
    {
        _text.reset( new string("No Text Set.") );
    }


    void ParsedStateMachine::addParameter( const std::string& name, TaskAttributeBase* var )
    {
        assert( parametervalues.find( name ) == parametervalues.end() );
        parametervalues[name] = var;
        // every parameter is also a readonly var...
        // visiblereadonlyvalues[name] = var->toDataSource();
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

    std::vector<std::string> ParsedStateMachine::getParameterNames() const
    {
        return ORO_std::keys( parametervalues );
    }

    void ParsedStateMachine::setName( const std::string& name, bool recursive )
    {
        // BIG NOTE :
        // this function should me named 'instantiate' or so because it does more than
        // settting the name, it also recursively arranges names of children and
        // sets the parent-child TC connections. Reed the 'recursive' flag as 'instantiate'.
        // it is used only recursively for instantiating root contexts.
        //cerr << "Setting name "<< _name << " to " << name<<" rec: "<<recursive<<endl;
        // set the StateMachine name
        this->_name = name;
        // set the datasource's name
        //nameds->set( name );

        if ( recursive == false )
            return;
        //this->getTaskContext()->addPeer( this->getTaskContext()->getPeer("states")->getPeer("task") );
        for ( ChildList::const_iterator i = getChildren().begin(); i != getChildren().end(); ++i )
        {
            std::string subname = name + "." + (*i)->getName();
            ParsedStateMachine* psc = static_cast<ParsedStateMachine*>( *i );
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
