#include "execution/ProcessorStateExec.hpp"
#include "execution/ProcessorStateLoad.hpp"
#include "execution/ProcessorStateConfig.hpp"
#include "execution/Processor.hpp"

namespace ORO_Execution
{

	ProcessorStateExec::ProcessorStateExec(Processor* proc, SystemContext* sc, ProgramInterface* pi)
        :ProcessorState(proc), systemContext(sc), program(pi)
	{
	}

	ProcessorStateExec::~ProcessorStateExec()
	{
		delete systemContext;
		delete program;
	}

	bool ProcessorStateExec::abort()
	{
		resetState();
        return true;
	}

	bool ProcessorStateExec::stopExecution()
	{
		// without changing program counter == pause
		ProcessorStateLoad* newState= new ProcessorStateLoad(processor,systemContext);
		newState->loadProgram(program);
        systemContext = 0;  program = 0;
		changeState(newState);
        return true;
	}


	bool ProcessorStateExec::deleteProgram()
	{
		ProcessorStateConfig* newState= new ProcessorStateConfig(processor);
		newState->loadSystemContext(systemContext);
        // we pass ownership of the systemContext to the
        // config state
        systemContext = 0;
        delete program;
        program = 0;
		changeState(newState);
        return true;
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

