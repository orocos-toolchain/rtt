
#include "corelib/EventInterfaces.hpp"

namespace ORO_CoreLib
{
    NameServer<EventInterface*> EventInterface::nameserver;
    NameServer<EventOperationInterface*> EventOperationInterface::nameserver;

    /**
     * The empty completer.
     */
    struct CompleterDummy :public EventCompleterInterface
    {
        virtual void completeEvent() { /*NOP*/ }
    };

    /**
     * @see EventCompleterInterface.hpp
     */
    CompleterDummy EmptyCompleter;
    EventCompleterInterface* EventCompleterInterface::None = &EmptyCompleter; 
    
    /**
     * The empty listener.
     */
    struct ListenerDummy :public EventListenerInterface
    {
        virtual void handleEvent( CallbackInterface* ) { /*NOP*/ }
    };

    /**
     * @see EventListenerInterface.hpp
     */
    ListenerDummy EmptyListener;
    EventListenerInterface* EventListenerInterface::None = &EmptyListener; 
}
