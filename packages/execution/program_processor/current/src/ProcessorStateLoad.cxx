#include "execution/ProcessorStateLoad.hpp"
#include "execution/ProcessorStateConfig.hpp"
#include "execution/ProcessorStateExec.hpp"
#include "execution/Processor.hpp"

#include <iostream>

namespace ORO_Execution
{

	ProcessorStateLoad::ProcessorStateLoad(Processor* proc, SystemContext* sc)
       : ProcessorState(proc), systemContext(sc), program(0)
	{
	}

	ProcessorStateLoad::~ProcessorStateLoad()
	{
		delete systemContext;
		delete program;
	}


	bool ProcessorStateLoad::abort()
	{
		resetState(); // calls destructor of this
        return true;
	}


	bool ProcessorStateLoad::deleteProgram()
	{
		ProcessorStateConfig* newState = new ProcessorStateConfig(processor);
		newState->loadSystemContext(systemContext);
        // we pass ownership of systemContext to the config state.
        systemContext = 0;
        delete program;
        program = 0;
		changeState(newState);
        return true;
	}


	bool ProcessorStateLoad::startExecution()
	{
		if ( isValidProgram(program))
		{
            // Reset the program, just before execution is entered.
            program->reset();
			ProcessorStateExec* newState=new ProcessorStateExec(processor, systemContext, program);
            // we pass ownership of the programand systemContext to
            // ProcessorStateExec.
            program = 0;
            systemContext = 0;
			changeState(newState);
            return true;
		}
		else { std::cout << "A valid program needs to be loaded first" <<std::endl; return false; }
	}


	bool ProcessorStateLoad::loadProgram(ProgramInterface* pi)
	{
		if  ( isValidProgram(pi) )
		{
			program = pi ;
            return true;
		}
        return false;
	}


	bool ProcessorStateLoad::resetProgram()
	{
		program->reset();
        return true;
	}


	bool ProcessorStateLoad::isValidProgram(ProgramInterface* p)
	{
		return ( p != 0 );
	}
}


