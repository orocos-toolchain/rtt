#include "execution/Processor.hpp"

namespace ORO_Execution
{

    Processor::Processor()
    {
        currentState = new ProcessorStateInit(this);
    }

    Processor::~Processor()
    {
        delete currentState;
    }

    void Processor::changeState(ProcessorState* newState)
    {
        delete currentState;
    	currentState = newState;
    }

    void Processor::resetState()
    {
        delete currentState;
    	currentState = new ProcessorStateInit( this );
    }

	//forward every action to its state
    void Processor::startConfiguration(){ currentState->startConfiguration();}
	void Processor::abort(){ currentState->abort();}
	void Processor::endConfiguration(){currentState->endConfiguration();}
	void Processor::deleteProgram() {currentState->deleteProgram();}
	void Processor::startExecution() {currentState->startExecution();}
	void Processor::stopExecution() {currentState->stopExecution();}

	void Processor::loadSystemContext(SystemContext* sc){currentState->loadSystemContext(sc);}
	void Processor::loadProgram(ProgramInterface* pi){currentState->loadProgram(pi);}
	void Processor::resetProgram() {currentState->resetProgram();}
	void Processor::doStep(){currentState->doStep();}

}

