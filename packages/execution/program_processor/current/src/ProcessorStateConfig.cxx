#include "execution/ProcessorStateConfig.hpp"
#include "execution/ProcessorStateLoad.hpp"
#include "execution/Processor.hpp"

namespace ORO_Execution
{

	ProcessorStateConfig::ProcessorStateConfig(Processor* proc)
	{
		processor = proc;
		systemContext = 0;
	}

	ProcessorStateConfig::~ProcessorStateConfig()
	{
		delete systemContext;
	}

	void ProcessorStateConfig::abort()
	{
		resetState(); //roept destructor van this op
	}

    void ProcessorStateConfig::endConfiguration()
    {
    	if ( isValidConfig(systemContext))
    	{
                // we pass the systemContext's ownership to
                // ProcessorStateLoad, so we set it to 0, to make sure
                // we don't delete it twice
    		ProcessorStateLoad* newState = new ProcessorStateLoad(processor, systemContext);
            systemContext = 0;
    		changeState(newState);
    	}
    	else
    	{
    		output("A valid system configuration needs to be defined first");
    	}
    }

    void ProcessorStateConfig::loadSystemContext(SystemContext* sc)
    {
    	if (isValidConfig(sc)){systemContext = sc;}
    }


    bool ProcessorStateConfig::isValidConfig(SystemContext* sc)
    {
    	return (sc != 0 && sc->currentState() != 0);
    }

}

