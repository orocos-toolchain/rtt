#include "execution/CommandString.hpp"

#include <iostream>

namespace ORO_Execution
{
	CommandString::CommandString(std::string s)
	{
		command = s;
	}	

    void CommandString::execute()
    {
    	std::cout<< "Command executed: " <<command <<std::endl;
    }

    std::string CommandString::toString()
    {
    	return command;
    }


}
