#include "execution/ProcessorStateExec.hpp"
#include "execution/ProcessorStateLoad.hpp"
#include "execution/ProcessorStateConfig.hpp"
#include "execution/Processor.hpp"

namespace ORO_Execution
{

	ProcessorStateExec::ProcessorStateExec(Processor* proc, SystemContext* sc, ProgramInterface* pi)
	{
		processor = proc;
		systemContext = sc;
		program = pi;
	}

	ProcessorStateExec::~ProcessorStateExec()
	{
		delete systemContext;
		delete program;
	}

	void ProcessorStateExec::abort()
	{
		resetState(); //die roept destructor van this op
	}

	void ProcessorStateExec::stopExecution()
	{
		//zonder programmateller te veranderen =  pauze
		ProcessorStateLoad* newState= new ProcessorStateLoad(processor,systemContext);
		newState->loadProgram(program);
		changeState(newState);
	}


	void ProcessorStateExec::deleteProgram()
	{
		ProcessorStateConfig* newState= new ProcessorStateConfig(processor);
		newState->loadSystemContext(systemContext);
                // we pass ownership of the systemContext to the
                // config state
                systemContext = 0;
		changeState(newState);
	}


	void ProcessorStateExec::doStep()
	{
        // request to stay in current state, and thus handle the 
        // resulting state.
		systemContext->requestState( systemContext->currentState());

		//Execute next program node
		program->execute();
	}

}

