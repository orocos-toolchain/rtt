
#include "corelib/EventSimple.hpp"
#include "corelib/CompletionProcessor.hpp"

namespace ORO_CoreLib
{
    using namespace detail;

        EventSimple::EventSimple(EventType t)
            : EventStrategyMethods(t), myList( Listener::None ), myComp( Completer::None )
        {
            cp = CompletionProcessor::Instance();
        }

        EventSimple::~EventSimple()
        {}

        void EventSimple::fire()
        {
            strategy->notify(this);
        }

        void EventSimple::complete()
        {
            strategy->complete(this, myList);
        }

        void EventSimple::notifySyn()
        {
            myList->handleEvent(this);
        }

        void EventSimple::notifyAsyn()
        {
            cp->queue( myList, this );
        }

        void EventSimple::completeSyn( EventListenerInterface* eli )
        {
            myComp->completeEvent();
        }

        void EventSimple::completeAsyn( EventListenerInterface* eli )
        {
            cp->queue( myComp );
        }
}
