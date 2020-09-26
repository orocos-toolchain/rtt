/***************************************************************************
  tag: Peter Soetens  Tue Jul 20 17:32:42 CEST 2004  ParsedStateMachine.cxx

                        ParsedStateMachine.cxx -  description
                           -------------------
    begin                : Tue July 20 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#include "rtt/scripting/ParsedStateMachine.hpp"
#include "rtt/internal/DataSource.hpp"
#include "rtt/ExecutionEngine.hpp"
#include "rtt/scripting/StateDescription.hpp"

#include "rtt/Service.hpp"
#include "rtt/scripting/StateMachineService.hpp"
#include "rtt/TaskContext.hpp"
#include "rtt/internal/mystd.hpp"

#include <cassert>

#include <boost/lambda/lambda.hpp>

namespace RTT {
    using namespace detail;
    using namespace std;
    using boost::tuples::get;
    /**
     * @todo
     * 1. add copy/clone semantics to StateInterface and StateMachine.
     * 2a. rewrite ParsedStateMachine::copy to use (1)
     * 2b. remove all old StateMachine in DataSource code ( code is deprecated by StateMachineCommands implementation)
     * 3. refactor SM copying code in StateGraphParser to this file (and vice versa).
     * 4. in the end, no dynamic_casts should be needed anymore.
     */
    ParsedStateMachinePtr ParsedStateMachine::copy( std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate ) const
    {
        /* Recursive copy :
         * First copy this SC, then its child SC's
         */
        std::map<const StateInterface*, StateInterface*> statemapping;
        ParsedStateMachinePtr ret(new ParsedStateMachine());
        ret->_text = this->_text;
        ret->setName( this->_name, false);

        if (instantiate)
            Logger::log() <<Logger::Debug <<"Creating an instance of "<< this->_name << Logger::endl;

        // First copy the task such that commands and attributes can be correctly
        // copied. This also sets the EventProcessor for the SM.
        ret->setService( this->object->copy(ret, replacements, instantiate) );

        // the parameters of the SC, similar to FunctionGraph's Arguments.
        for ( VisibleWritableValuesMap::const_iterator i = parametervalues.begin();
              i != parametervalues.end(); ++i )
        {
            // What is sure, is that each param
            // must also be in the ConfigurationInterface.
            assert( ret->getService()->getValue( i->first ) );
            ret->parametervalues[i->first] = ret->getService()->getValue( i->first );
        }

        //**********************
        // TODO add copy method to StateMachine itself where all stuff below belongs :
        // but, not so easy since copy makes new instance...
        for ( ChildList::const_iterator i = getChildren().begin(); i != getChildren().end(); ++i )
        {
            // copy the submachines....
            assert( dynamic_cast<ParsedStateMachine*>( i->get() ) == static_cast<ParsedStateMachine*>( i->get() ));
            ParsedStateMachinePtr oldmachine = boost::dynamic_pointer_cast<ParsedStateMachine>( *i );
            ParsedStateMachinePtr newmachine(oldmachine->copy( replacements, instantiate ));
            // I would think that providing 'instantiate' would not hurt...
            // XXX? previously, the instantiate flag was not given to copy, does it now break apps ?

            ret->addChild( newmachine ); // also copy tree info to StateMachine !
            newmachine->setParent( ret );
        }

        // Copy the InitCommand :
        if (this->getInitCommand()) {
            ret->setInitCommand( this->getInitCommand()->copy(replacements) );
            // test :
            //ret->getInitCommand()->execute();
        }

        // First make a copy of all states.  All states are either
        // known by their name or by a transition from or to them...
        statemapping[0] = 0; // insert null element.
        for ( TransitionMap::const_iterator i = stateMap.begin(); i != stateMap.end(); ++i )
        {
            if( statemapping.find( i->first ) == statemapping.end() && i->first != 0 ) {
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
                boost::shared_ptr<ProgramInterface> transprog( j->get<4>() );
                if (transprog)
                    transprog.reset( j->get<4>()->copy(replacements) );
                ret->transitionSet(fromState, toState, condition, transprog, rank, line );
            }
        }

        // next, copy/recreate the events
        for ( EventMap::const_iterator i = eventMap.begin(); i != eventMap.end(); ++i )
        {
            assert( statemapping.find( i->first ) != statemapping.end() );
            StateInterface* fromState = statemapping[i->first];
            for ( EventList::const_iterator j = i->second.begin(); j != i->second.end(); ++j )
            {
                ServicePtr sp = j->get<0>();
                string ename = j->get<1>();
                vector<DataSourceBase::shared_ptr> origargs( j->get<2>() );
                vector<DataSourceBase::shared_ptr> newargs;
                for ( vector<DataSourceBase::shared_ptr>::const_iterator vit = origargs.begin();
                      vit != origargs.end(); ++vit)
                    newargs.push_back( (*vit)->copy(replacements) );
                StateInterface* toState = statemapping[j->get<3>()];
                ConditionInterface* condition = j->get<4>()->copy( replacements );
                ProgramInterfacePtr tprog;
                ProgramInterfacePtr tgraph( j->get<5>() );
                if (tgraph)
                    tprog.reset( tgraph->copy(replacements) );
                StateInterface* elseState = statemapping[j->get<7>()];
                ProgramInterfacePtr eprog;
                ProgramInterfacePtr egraph( j->get<8>() );
                if (egraph)
                    eprog.reset( egraph->copy(replacements) );
#ifndef NDEBUG
                bool eresult =
#endif
                    ret->createEventTransition(sp, ret->getService()->getOwner()->engine(), ename, newargs, fromState, toState, condition, tprog, elseState, eprog );
                assert( eresult );
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
        this->smStatus = Status::unloaded;
        if ( this->isLoaded() ){
            getEngine()->removeFunction(this);
        }

        // we own our states...
        for ( TransitionMap::iterator i = stateMap.begin();
              i != stateMap.end(); ++i )
            delete i->first;
        // we own our conditions...
        for ( TransitionMap::iterator i = stateMap.begin();
              i != stateMap.end(); ++i )
            for ( TransList::iterator i2 = i->second.begin(); i2 != i->second.end(); ++i2 )
                delete get<0>( *i2 );  // delete the condition.

        // we own our event guards...
        for ( EventMap::iterator i = eventMap.begin();
              i != eventMap.end(); ++i )
            for ( EventList::iterator i2 = i->second.begin(); i2 != i->second.end(); ++i2 )
                delete get<4>( *i2 );  // delete the condition.

    }

    ParsedStateMachine::ParsedStateMachine()
        : StateMachine( StateMachinePtr() ) // no parent, no task
    {
        _text.reset( new string("No Text Set.") );
    }

    void ParsedStateMachine::unloading()
    {
        StateMachine::unloading();
        // just kill off the interface.
        if ( !object )
            return;
        if ( object->getParent() && object->getParent()->hasService( object->getName() ) ){
            assert( object == object->getParent()->getService( object->getName() ) );
            object->getParent()->removeService( object->getName() );
        }
        object.reset();
    }

    void ParsedStateMachine::addParameter( const std::string& name, AttributeBase* var )
    {
        assert( parametervalues.find( name ) == parametervalues.end() );
        parametervalues[name] = var;
        // every parameter is also a readonly var...
        // visiblereadonlyvalues[name] = var->toDataSource();
    }

    AttributeBase* ParsedStateMachine::getParameter( const std::string& name ) const
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
        return keys( parametervalues );
    }

    void ParsedStateMachine::setName( const std::string& name, bool recursive )
    {
        // XXX BIG NOTE :
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
        //this->getService()->addPeer( this->getService()->getPeer("states")->getPeer("task") );
        for ( ChildList::const_iterator i = getChildren().begin(); i != getChildren().end(); ++i )
        {
            std::string subname = name + "." + (*i)->getName();
            ParsedStateMachine* psc = static_cast<ParsedStateMachine*>( i->get() );
            psc->setName( subname, true );
            // we own our child:
            psc->getService()->setOwner( 0 );
            this->getService()->addService( psc->getService() );
        }
    }

    std::string ParsedStateMachine::getText() const
    {
        return *_text;
    }

    void ParsedStateMachine::setText( std::string text)
    {
        *_text = text;
    }

    StateMachineServicePtr ParsedStateMachine::getService() const {
        return object;
    }
    void ParsedStateMachine::setService(StateMachineServicePtr tc) {
        object = tc;
    }

    bool ParsedStateMachine::inState( const std::string& name ) {
        StateInterface* copy = this->currentState();
        if (copy == 0)
            return false;
        return copy->getName() == name;
    }

    void ParsedStateMachine::finish()
    {
    }

}
