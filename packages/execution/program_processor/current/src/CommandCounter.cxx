#include "execution/CommandCounter.hpp"

namespace ORO_Execution
{
	 
	 CommandCounter::CommandCounter()
	 	:counter(0)
	 {
	 }

     CommandCounter::~CommandCounter()
     {
     }

     void CommandCounter::execute()
     {
     	counter++;
     }

     std::string CommandCounter::toString()
     {
     	return "CommandCounter";
     }

	 void CommandCounter::reset()
	 {
	 	counter = 0; 
	 }
	 
	 int CommandCounter::getCounter()
	 {
	 	return counter;
	 }

}

