#include "execution/ProcessorState.hpp"
#include "execution/Processor.hpp"

#include <iostream>

namespace ORO_Execution
{

    bool ProcessorState::startConfiguration(){ return false; }
    bool ProcessorState::endConfiguration(){ return false; }
    bool ProcessorState::deleteProgram(){ return false; }
    bool ProcessorState::startExecution(){ return false; }
    bool ProcessorState::startStepping(){ return false; }
    bool ProcessorState::stopExecution(){ return false; }
    bool ProcessorState::abort(){ return false; }
    bool ProcessorState::loadStateContext(StateContext* sc){ return false; } 
    bool ProcessorState::loadProgram(ProgramInterface* pi){ return false; }
    bool ProcessorState::resetProgram(){ return false; } 

    void ProcessorState::doStep() {}
    bool ProcessorState::nextStep() { return false;}  
    bool ProcessorState::process( CommandInterface* c) { return false; }

	void ProcessorState::output(const std::string& _string)
	{
		std::cout << _string << std::endl;
	}     
            
    void ProcessorState::changeState(ProcessorState* newState)
    {
       	processor->changeState(newState);
    }
    
    void ProcessorState::resetState()
    {
    	processor->resetState();
    }

}
