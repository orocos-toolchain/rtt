#include "execution/ProcessorStateConfig.hpp"
#include "execution/ProcessorStateLoad.hpp"
#include "execution/Processor.hpp"

namespace ORO_Execution
{

	ProcessorStateConfig::ProcessorStateConfig(Processor* proc)
       : ProcessorState(proc), systemContext(0)
	{
	}

	ProcessorStateConfig::~ProcessorStateConfig()
	{
		delete systemContext;
	}

	bool ProcessorStateConfig::abort()
	{
		resetState();
        return true;
	}

    bool ProcessorStateConfig::endConfiguration()
    {
    	if ( isValidConfig(systemContext))
    	{
                // we pass the systemContext's ownership to
                // ProcessorStateLoad, so we set it to 0, to make sure
                // we don't delete it twice
    		ProcessorStateLoad* newState = new ProcessorStateLoad(processor, systemContext);
            systemContext = 0;
    		changeState(newState);
            return true;
    	}
    	else
    	{
    		output("A valid system configuration needs to be defined first");
            return false;
    	}
    }

    bool ProcessorStateConfig::loadStateContext(StateContext* sc)
    {
    	if (isValidConfig(sc)){systemContext = sc; return true;} return false;
    }


    bool ProcessorStateConfig::isValidConfig(StateContext* sc)
    {
    	return (sc != 0 && sc->currentState() != 0);
    }

	void ProcessorStateConfig::doStep()
	{
        // If stepping is requested, we process the systemContext;
        if ( systemContext )
            systemContext->requestNextState( );
	}

}

