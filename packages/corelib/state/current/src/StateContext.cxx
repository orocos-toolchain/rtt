#include "corelib/StateContext.hpp"
#include "boost/tuple/tuple.hpp"

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

        void StateContext::initState( StateInterface* s_init )
        {
            initstate=s_init;
            if ( current == 0 )
                enterState( s_init );
        }

        void StateContext::finalState( StateInterface* s_fini )
        {
            finistate = s_fini;
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
            //boost::tie(it1,it2) = stateMap.equal_range( current );

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

            TransitionAnyMap::iterator it3 = stateAnyMap.begin();

            for ( ; it3 != stateAnyMap.end(); ++it3 )
                if ( it3->second->evaluate() )
                    if ( it3->first == current )
                        {
                            current->handle();
                            return current;
                        }
                    else 
                        {
                            leaveState(current);
                            enterState( it3->first);
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
            //boost::tie(it1,it2) = stateMap.equal_range( current );


            for ( ; it1 != it2; ++it1 )
                if ( get<0>(*it1)->evaluate() )
                    return get<1>(*it1);

            TransitionAnyMap::iterator it3 = stateAnyMap.begin();

            for ( ; it3 != stateAnyMap.end(); ++it3 )
                if ( it3->second->evaluate() )
                    return it3->first;

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
            //boost::tie(it1,it2) = stateMap.equal_range( current );
            for ( ; it1 != it2; ++it1 )
                if ( get<1>(*it1) == s_n
                     && get<0>(*it1)->evaluate() )
                    {
                        leaveState( current );
                        enterState( s_n );
                        return true;
                    }

            // between any state and a specific state
            TransitionAnyMap::iterator itA = stateAnyMap.find( s_n );

            if ( itA != stateAnyMap.end()
                 && ( *itA ).second->evaluate() )
                {
                    leaveState( current );
                    enterState( s_n );
                    return true;
                }

            return false;
        }

        void StateContext::transitionSet( StateInterface* from, StateInterface* to, ConditionInterface* cnd, int priority )
        {
            //stateMap.insert(std::make_pair( from, boost::make_tuple( cnd, to, priority ) ));
            TransList::iterator it;
            for ( it= stateMap[from].begin(); it != stateMap[from].end() && get<2>(*it) >= priority; ++it);
            stateMap[from].insert(it, boost::make_tuple( cnd, to, priority ) );
        }

        void StateContext::transitionSet( StateInterface* target, ConditionInterface* cnd )
        {
            stateAnyMap[ target ] = cnd;
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
}

