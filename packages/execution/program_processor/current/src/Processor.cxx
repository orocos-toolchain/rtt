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
    bool Processor::startConfiguration(){ return currentState->startConfiguration();}
	bool Processor::abort(){ return currentState->abort();}
	bool Processor::endConfiguration(){return currentState->endConfiguration();}
	bool Processor::deleteProgram() {return currentState->deleteProgram();}
	bool Processor::startExecution() {return currentState->startExecution();}
	bool Processor::stopExecution() {return currentState->stopExecution();}

	bool Processor::loadStateContext(StateContext* sc){return currentState->loadStateContext(sc);}
	bool Processor::loadProgram(ProgramInterface* pi){return currentState->loadProgram(pi);}
	bool Processor::resetProgram() {return currentState->resetProgram();}
	void Processor::doStep(){currentState->doStep();}

}

