#include "execution/ProcessorStateInit.hpp"
#include "execution/Processor.hpp"
#include "execution/ProcessorStateConfig.hpp"

namespace ORO_Execution
{
	ProcessorStateInit::ProcessorStateInit(Processor* proc)
	{
		processor = proc;		
	}
	
	void ProcessorStateInit::startConfiguration()
	{
		ProcessorStateConfig* newState = new ProcessorStateConfig(processor);
		changeState(newState);	
	}
	

}

