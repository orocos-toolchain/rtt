#ifndef EVENTSIMPLE_HPP
#define EVENTSIMPLE_HPP

#include "EventInterfaces.hpp"
#include "EventStrategy.hpp"

namespace ORO_CoreLib
{
    using namespace detail;

    class CompletionProcessor;

    /**
     * Defines an Event in its most simple form. It has one Listener
     * and one Completer.
     */
    class EventSimple
        : public EventSimpleInterface,
          public EventOperationInterface,
          public CallbackInterface,
          protected EventStrategyMethods
    {
    public:
        EventSimple(EventType t);

        virtual ~EventSimple();

        void fire();

        void complete();

    protected:

        virtual void notifySyn();

        virtual void notifyAsyn();

        virtual void completeSyn( EventListenerInterface* eli );

        virtual void completeAsyn( EventListenerInterface* eli );

        EventListenerInterface* myList;
        EventCompleterInterface* myComp;
        CompletionProcessor* cp;
    };
}

#endif
