#ifndef EVENTBARRIER_HPP
#define EVENTBARRIER_HPP

#include "EventInterfaces.hpp"
#include "EventStrategy.hpp"
#include "CompletionProcessor.hpp"

namespace ORO_CoreLib
{
    using namespace detail;

    /**
     * Defines an Event for synchronisation purposes. The Completer will
     * only be called when all Listeners completed the Event. In this
     * way, the Completers execution is on hold until all Listeners
     * have signalled completion.
     */ 
    class EventBarrier
        : public EventBarrierInterface,
          public EventOperationInterface,
          public EventBoundToListenerInterface,
          protected EventStrategyMethods
    {
    public:
        EventBarrier(EventType t);

        virtual ~EventBarrier();

        void fire();

        virtual void listenerAdd( EventListenerInterface* eli )
        {
            EventBarrierInterface::listenerAdd(eli);
            callBacks.push_back(EventCallbackStub(this,eli));
        }

        virtual void listenerRemove( EventListenerInterface* eli )
        {
            EventBarrierInterface::listenerRemove(eli);
            callBacks.erase( find_if( callBacks.begin(), callBacks.end(), bind2nd(EventCallbackStub::FindStub(),eli) ) );
        }

        void complete( EventListenerInterface* eli );

    protected:
        virtual void notifySyn();

        virtual void notifyAsyn();

        virtual void completeSyn( EventListenerInterface* eli );

        virtual void completeAsyn( EventListenerInterface* eli );

        CompletionProcessor* cp;

        EventCompleterInterface* myComp;
        
        unsigned int count;

        std::vector<EventCallbackStub> callBacks;
    };
}

#endif
