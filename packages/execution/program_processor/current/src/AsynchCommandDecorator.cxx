#include "execution/AsynchCommandDecorator.hpp"

namespace ORO_Execution
{

	AsynchCommandDecorator::AsynchCommandDecorator(CommandInterface* cmd)
		: command(cmd),executed(false)
	{
	}

	AsynchCommandDecorator::~AsynchCommandDecorator()
	{
		delete command;
	}

	void AsynchCommandDecorator::execute(){
     	if (executed){
     	}
     	else { //this clause should be executed once
     		command->execute();
     		executed=true;
     	}
    }


    std::string AsynchCommandDecorator::toString()
    {
        return command->toString();
    }

    void AsynchCommandDecorator::reset()
    {
        executed = false;
    }
}
