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

#include "ParsedStateMachine.hpp"
#include "DataSource.hpp"
#include "ExecutionEngine.hpp"
#include "StateDescription.hpp"

#include "TaskContext.hpp"
#include "StateMachineTask.hpp"
#include <iostream>

#include <boost/lambda/lambda.hpp>

namespace RTT {
    using namespace detail;
    using namespace std;
    using namespace boost::lambda;
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
        // copied.
        ret->setTaskContext( this->context->copy(ret, replacements, instantiate) );

        // the parameters of the SC, similar to FunctionGraph's Arguments.
        for ( VisibleWritableValuesMap::const_iterator i = parametervalues.begin();
              i != parametervalues.end(); ++i )
        {
            // What is sure, is that each param
            // must also be in the attributerepository.
            assert( ret->getTaskContext()->attributes()->getValue( i->first ) );
            ret->parametervalues[i->first] = ret->getTaskContext()->attributes()->getValue( i->first );
        }

        //**********************
        // TODO add copy method to StateMachine itself where all stuff below belongs :
        // but, not so easy since copy makes new instance...
        for ( ChildList::const_iterator i = getChildren().begin(); i != getChildren().end(); ++i )
        {
            // copy the submachines....
            assert( dynamic_cast<ParsedStateMachine*>( i->get() ) == static_cast<ParsedStateMachine*>( i->get() ));
            ParsedStateMachine* oldmachine = static_cast<ParsedStateMachine*>( i->get() );
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
                EventService* es = j->get<0>();
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

                bool eresult = ret->createEventTransition(es, ename, newargs, fromState, toState, condition, tprog, elseState, eprog );
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
        this->handleUnload();

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
        : StateMachine( StateMachinePtr() ), context(0) // no parent, no task
    {
        _text.reset( new string("No Text Set.") );
    }

    void ParsedStateMachine::handleUnload()
    {
        // just kill off the interface.
        if ( context && context->getPeer("states") )
            context->getPeer("states")->removePeer( context->getName() );
        delete context;
        context = 0;
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
        //this->getTaskContext()->addPeer( this->getTaskContext()->getPeer("states")->getPeer("task") );
        for ( ChildList::const_iterator i = getChildren().begin(); i != getChildren().end(); ++i )
        {
            std::string subname = name + "." + (*i)->getName();
            ParsedStateMachine* psc = static_cast<ParsedStateMachine*>( i->get() );
            psc->setName( subname, true );
            this->getTaskContext()->addPeer( psc->getTaskContext() );
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

    TaskContext* ParsedStateMachine::getTaskContext() const {
        return context;
    }
    void ParsedStateMachine::setTaskContext(StateMachineTask* tc) {
        context = tc;
        this->eproc = tc->engine()->events();
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
