#include "execution/SystemContext.hpp"

#include "execution/SystemStateInterface.hpp"

namespace ORO_Execution
{

	SystemContext::SystemContext()
          : StateContext()
    {
    }

	SystemContext::SystemContext(SystemStateInterface* s_init)
          : StateContext( s_init )
	{
            states.push_back( s_init );
	}


    void SystemContext::initState(SystemStateInterface* s_init)
    {
    	StateContext::initState((StateInterface*)s_init);
    }

	bool SystemContext::requestState(SystemStateInterface* s_n)
	{
		bool result = StateContext::requestState((StateInterface*)s_n);
		return result;
	}

    void SystemContext::transitionSet(SystemStateInterface* from, SystemStateInterface* to, ConditionInterface* cnd )
    {
    	StateContext::transitionSet((StateInterface*)from,(StateInterface*)to,cnd);
    }

    void SystemContext::transitionSet(SystemStateInterface* target, ConditionInterface* cnd )
    {
    	StateContext::transitionSet((StateInterface*)target,cnd);
    }

    SystemStateInterface* SystemContext::currentState()
    {
    	StateInterface* result = StateContext::currentState();
    	return ( (SystemStateInterface*) result);
    }

    namespace
    {
        template<typename iter>
        void delete_all( iter begin, iter end )
        {
            for ( ; begin != end; ++begin )
                delete *begin;
        }
    }

    SystemContext::~SystemContext()
    {
        delete_all( states.begin(), states.end() );
    }

}

