#include "execution/ProcessorState.hpp"
#include "execution/Processor.hpp"

#include <iostream>

namespace ORO_Execution
{

	void ProcessorState::output(std::string _string)
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
