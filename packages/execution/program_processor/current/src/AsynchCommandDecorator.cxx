#include "execution/AsynchCommandDecorator.hpp"

namespace ORO_Execution
{

	AsynchCommandDecorator::AsynchCommandDecorator(CommandInterface* cmd)
		: command(cmd),executed(false)
	{
	}

	AsynchCommandDecorator::AsynchCommandDecorator(const AsynchCommandDecorator& orig)
		: command( orig.getCommand()->clone() ),
          executed(false)
	{
	}

	AsynchCommandDecorator::~AsynchCommandDecorator()
	{
		delete command;
	}

     CommandInterface* AsynchCommandDecorator::clone() const
     {
         return new AsynchCommandDecorator(*this);
     }

    const CommandInterface* AsynchCommandDecorator::getCommand() const
    {
        return command;
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
