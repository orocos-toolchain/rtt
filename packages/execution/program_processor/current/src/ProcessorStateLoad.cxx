#include "execution/ProcessorStateLoad.hpp"
#include "execution/ProcessorStateConfig.hpp"
#include "execution/ProcessorStateExec.hpp"
#include "execution/Processor.hpp"

#include <iostream>

namespace ORO_Execution
{

	ProcessorStateLoad::ProcessorStateLoad(Processor* proc, SystemContext* sc)
	{
		processor = proc ;
		systemContext = sc ;
		program = 0;
	}

	ProcessorStateLoad::~ProcessorStateLoad()
	{
		delete systemContext;
		delete program;
	}


	void ProcessorStateLoad::abort()
	{
		resetState(); //die roept destructor van this op
	}


	void ProcessorStateLoad::deleteProgram()
	{
		ProcessorStateConfig* newState= new ProcessorStateConfig(processor);
		newState->loadSystemContext(systemContext);
        // we pass ownership of systemContext to the config state.
        systemContext = 0;
		changeState(newState);
	}


	void ProcessorStateLoad::startExecution()
	{
		if ( isValidProgram(program))
		{
			ProcessorStateExec* newState=new ProcessorStateExec(processor, systemContext, program);
                        // we pass ownership of the programand systemContext to
                        // ProcessorStateExec.
                        program = 0;
                        systemContext = 0;
			changeState(newState);
		}
		else { std::cout << "A valid program needs to be loaded first" <<std::endl; }
	}


	void ProcessorStateLoad::loadProgram(ProgramInterface* pi)
	{
		if  ( isValidProgram(pi) )
		{
			program = pi ;
		}
	}


	void ProcessorStateLoad::resetProgram()
	{
		program->reset();
	}


	bool ProcessorStateLoad::isValidProgram(ProgramInterface* p)
	{
		return ( p != 0 );
	}
}


