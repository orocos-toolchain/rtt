#include "CommandInterface.hpp"
#include "SystemContext.hpp"
#include "SystemStateInterface.hpp"
#include <exception>

namespace ORO_Execution
{
	class CommandChangeState : public CommandInterface
	{
		
		public:
		CommandChangeState(SystemContext* _sc, SystemStateInterface * _sti) : sc(_sc), sti(_sti)	{
			//if (!(sc && sti)) {
			//	throw new exception();//smerige nulpointers
			//}
		}
		
		~CommandChangeState()
		{}
		
		void execute() {
			sc->requestState(sti);
		}
		
		std::string toString() {
			return "CommandChangeState";
		}
	
	
		private:
			SystemContext * sc;
			SystemStateInterface * sti;

	};

}
