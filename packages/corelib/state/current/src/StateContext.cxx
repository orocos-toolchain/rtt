/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  StateContext.cxx

                        StateContext.cxx -  description
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
#include "corelib/StateContext.hpp"
#include "boost/tuple/tuple.hpp"

#include <functional>

namespace ORO_CoreLib
{
    using boost::tuples::get;

    StateContext::StateContext()
        : initstate(0), finistate(0), current( 0 )
    {}

    StateContext::StateContext( StateInterface* s_init, StateInterface* s_fini )
        : initstate(s_init), finistate(s_fini), current( 0 )
    {
        enterState( s_init );
    }

    bool StateContext::requestInitialState()
    {
        // first check if we are in initstate, so this
        // even works if current == initstate == finistate,
        // which is legal.
        if ( current == initstate )
        {
            current->handle();
            return true;
        }
        else if ( current == finistate )
        {
            leaveState( current );
            enterState( initstate );
            return true;
        }
        return false;
    }

    void StateContext::requestFinalState()
    {
        // If we are already in Final state, just handle again.
        if ( current == finistate )
            current->handle();
        else
        {
            leaveState( current );
            enterState( finistate );
        }
    }

    StateInterface* StateContext::requestNextState()
    {
        TransList::iterator it1, it2;
        it1 = stateMap[ current ].begin();
        it2 = stateMap[ current ].end();
        for ( ; it1 != it2; ++it1 )
            if ( get<0>(*it1)->evaluate() )
                if ( get<1>(*it1) == current )
                {
                    current->handle();
                    return current;
                }
                else
                {
                    leaveState(current);
                    enterState( get<1>(*it1) );
                    return current;
                }
        // handle the current state if nothing found :
        current->handle();

        return current;
    }

    StateInterface* StateContext::nextState()
    {
        TransList::iterator it1, it2;
        it1 = stateMap[ current ].begin();
        it2 = stateMap[ current ].end();

        for ( ; it1 != it2; ++it1 )
            if ( get<0>(*it1)->evaluate() )
                return get<1>(*it1);

        return current;
    }

    bool StateContext::requestState( StateInterface * s_n )
    {
        // to current state
        if ( current == s_n )
        {
            current->handle();
            return true;
        }

        // between 2 specific states
        TransList::iterator it1, it2;
        it1 = stateMap[ current ].begin();
        it2 = stateMap[ current ].end();

        for ( ; it1 != it2; ++it1 )
            if ( get<1>(*it1) == s_n
                 && get<0>(*it1)->evaluate() )
            {
                leaveState( current );
                enterState( s_n );
                return true;
            }

        return false;
    }

    void StateContext::transitionSet( StateInterface* from, StateInterface* to, ConditionInterface* cnd, int priority )
    {
        TransList::iterator it;
        for ( it= stateMap[from].begin(); it != stateMap[from].end() && get<2>(*it) >= priority; ++it);
        stateMap[from].insert(it, boost::make_tuple( cnd, to, priority ) );
    }

    StateInterface* StateContext::currentState()
    {
        return current;
    }

    void StateContext::leaveState( StateInterface* s )
    {
        s->onExit();
        current = 0;
    }

    void StateContext::enterState( StateInterface* s )
    {
        // Before a state is entered, all transitions are reset !
        TransList::iterator it;
        for ( it= stateMap[s].begin(); it != stateMap[s].end(); ++it)
            get<0>(*it)->reset();

        s->onEntry();
        current = s;

        current->handle();
    }

    StateContext::~StateContext() {
    }

    void StateContext::setInitialState( StateInterface* s )
    {
        initstate = s;
    }

    void StateContext::setFinalState( StateInterface* s )
    {
        finistate = s;
    }

    bool StateContext::isActive() const
    {
        return current != 0;
    }

    void StateContext::startRunning()
    {
        if ( !isActive() )
        {
            enterState( getInitialState() );
        }
    }

    void StateContext::stopRunning()
    {
        if ( current == getFinalState() )
        {
            leaveState( current );
        }
    }
}

