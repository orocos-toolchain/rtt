#include <corelib/CommandInterface.hpp>
#include "ProcessorControlInterface.hpp"
//#include <exception>

namespace ORO_Execution 
{
	using namespace ORO_CoreLib;
	
	class CommandStopExecution : public CommandInterface
	{
		
		public:
		CommandStopExecution(ProcessorControlInterface * _pci) : pci(_pci)
		{
			//if (!pci) {
			//	throw exception();
			//}
		}
		
		~CommandStopExecution()
		{}
		
		void execute() {
			pci->stopExecution();
		}
	
		std::string toString() {
			return "CommandStopExecution";
		}
	
		private:
			ProcessorControlInterface * pci;
	};

}
