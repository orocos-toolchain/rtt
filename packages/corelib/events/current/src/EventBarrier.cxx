
#include "corelib/EventBarrier.hpp"
#include "corelib/CompletionProcessor.hpp"

namespace ORO_CoreLib
{
    using namespace detail;

    EventBarrier::EventBarrier(EventType t)
        : EventStrategyMethods(t), myComp(0), count(0)
    {
        callBacks.reserve(128);
        cp = CompletionProcessor::Instance();
    }

    EventBarrier::~EventBarrier()
    {}

    void EventBarrier::fire()
    {
        count = 0;
        strategy->notify(this);
    }

    void EventBarrier::complete(EventListenerInterface* eli)
    {
        strategy->complete(this,eli);
    }

    void EventBarrier::notifySyn()
    {
        for(unsigned int i=0; i< myList.size(); ++i)
            myList[i]->handleEvent( &callBacks[i] );
    }

    void EventBarrier::notifyAsyn()
    {
        for(unsigned int i=0; i< myList.size(); ++i)
            cp->queue(myList[i], &callBacks[i] );
    }

    void EventBarrier::completeSyn( EventListenerInterface* eli )
    {
        // @todo : instead of counting, keep track of all Listeners.
        ++count;
        if ( count == myList.size() )
            myComp->completeEvent();
    }

    void EventBarrier::completeAsyn( EventListenerInterface* eli )
    {
        ++count;
        if ( count == myList.size() )
            cp->queue(myComp);
    }
}
