#include "corelib/StateContext.hpp"
#include "boost/tuple/tuple.hpp"

namespace ORO_CoreLib
{
        StateContext::StateContext() : current( 0 )
        {}

        StateContext::StateContext( StateInterface* s_init ) : current( 0 )
        {
            enterState( s_init );
        }

        void StateContext::initState( StateInterface* s_init )
        {
            if ( current == 0 )
                enterState( s_init );
        }

        StateInterface* StateContext::requestNextState()
        {
            TransitionMap::iterator it1, it2;
            boost::tie(it1,it2) = stateMap.equal_range( current );

            for ( ; it1 != it2; ++it1 )
                    if ( it1->second.first->evaluate() )
                        if ( it1->second.second == current )
                            {
                                current->handle();
                                return current;
                            }
                        else 
                            {
                                leaveState(current);
                                enterState(it1->second.second);
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
            TransitionMap::iterator it1, it2;
            boost::tie(it1,it2) = stateMap.equal_range( current );


            for ( ; it1 != it2; ++it1 )
                if ( it1->second.first->evaluate() )
                    return it1->second.second;

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
            TransitionMap::iterator it1, it2;
            boost::tie(it1,it2) = stateMap.equal_range( current );
            for ( ; it1 != it2; ++it1 )
                if ( it1->second.second == s_n
                     && it1->second.first->evaluate() )
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

        void StateContext::transitionSet( StateInterface* from, StateInterface* to, ConditionInterface* cnd )
        {
            stateMap.insert(std::make_pair( from, std::make_pair( cnd, to ) ));
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
            s->onEntry();
            current = s;

            current->handle();
        }
}

