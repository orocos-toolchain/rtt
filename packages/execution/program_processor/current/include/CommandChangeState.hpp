#include "CommandInterface.hpp"
#include "SystemContext.hpp"
#include "SystemStateInterface.hpp"
#include <exception>

namespace ORO_Execution
{
    /**
     * @brief A command that changes the State of the System.
     */
	class CommandChangeState
        : public CommandInterface
	{
		
		public:
		CommandChangeState(SystemContext* _sc, SystemStateInterface * _sti)
            : sc(_sc), sti(_sti) {
		}
		
		~CommandChangeState()
		{}
		
		void execute() {
			sc->requestState(sti);
		}
		
		std::string toString() {
			return "CommandChangeState";
		}
	
        virtual CommandInterface* clone() const
        {
            // shalow copy, we do not own the pointers
            return new CommandChangeState(*this);
        }

		private:
			SystemContext * sc;
			SystemStateInterface * sti;

	};

}
