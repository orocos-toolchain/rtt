#ifndef ORO_COMMAND_STOP_EXECUTION_HPP
#define ORO_COMMAND_STOP_EXECUTION_HPP

#include <corelib/CommandInterface.hpp>
#include "ProcessorControlInterface.hpp"

namespace ORO_Execution 
{
	using  ORO_CoreLib::CommandInterface;

	/**
     * @brief This command will instruct the Processor to
     * stop execution if it is executed.
     */
	class CommandStopExecution : public CommandInterface
	{
		
		public:
		CommandStopExecution(ProcessorControlInterface * _pci) : pci(_pci)
		{
		}
		
		~CommandStopExecution()
		{}
		
		void execute() 
        {
			pci->stopExecution();
		}
	
		std::string toString() {
			return "CommandStopExecution";
		}
	
        virtual CommandInterface* clone() const
        {
            return new CommandStopExecution( *this );
        }

		private:
			ProcessorControlInterface * pci;
	};

}
#endif
