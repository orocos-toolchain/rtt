#ifndef EVENTEMERGENCYSTOP_HPP
#define EVENTEMERGENCYSTOP_HPP

#include"Event.hpp"

namespace ORO_CoreLib
{
    /**
     * This event can be used as a cascading event.
     * It is itself a Listener and a Completer, so that it
     * fires itself when the methods of the
     * EventListenerInterface and EventCompleterInterface are called
     */
    class EventEmergencyStop 
        : public Event, 
          public EventListenerInterface,
          public EventCompleterInterface
    {
    public:
        EventEmergencyStop(EventType t, const std::string name="") : Event(t,name) {}
        virtual void handleEvent( CallbackInterface* cb)
        {
            fire();
            cb->complete();
        }

        virtual void completeEvent()
        {
            complete(this);
        }
    };
}

#endif
