#ifndef EVENTMULTICAST_HPP
#define EVENTMULTICAST_HPP

#include "EventInterfaces.hpp"
#include "EventStrategy.hpp"

namespace ORO_CoreLib
{
    using namespace detail;

    class CompletionProcessor;

    /** 
     * Defines an Event for multicasting the completion of a unique
     * Listener to all subscribed Completers.
     */
    class EventMultiCast
        :public EventMultiCastInterface,
         public EventOperationInterface,
         public CallbackInterface,
         protected EventStrategyMethods
    {
    public:
        EventMultiCast(EventType t);

        virtual ~EventMultiCast();

        void fire();
        
        void complete();
    protected:

        virtual void notifySyn();

        virtual void notifyAsyn();

        virtual void completeSyn( EventListenerInterface* eli );

        virtual void completeAsyn( EventListenerInterface* eli );

        CompletionProcessor* cp;

        EventListenerInterface* myList;
    };
}

#endif
