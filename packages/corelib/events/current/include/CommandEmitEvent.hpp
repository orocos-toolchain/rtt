
#include "CommandInterface.hpp"
#include "Event.hpp"

namespace ORO_CoreLib
{

    class CommandEmitEvent
        : public CommandInterface
    {
        EventOperationInterface* event;
    public:
        CommandEmitEvent( EventOperationInterface* ev )
            : event(ev) {}
        void execute()
        {
            event->fire();
        }
        CommandInterface* clone() const {
            return new CommandEmitEvent( event );
        }
        std::string toString()
        {
            return std::string("CommandEmitEvent");
        }
    };
}
