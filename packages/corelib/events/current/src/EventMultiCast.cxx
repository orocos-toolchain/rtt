
#include "corelib/EventMultiCast.hpp"
#include "corelib/CompletionProcessor.hpp"

namespace ORO_CoreLib
{
    using namespace detail;

        EventMultiCast::EventMultiCast(EventType t)
            : EventStrategyMethods(t), myList(0)
        {
            cp = CompletionProcessor::Instance();
        }

        EventMultiCast::~EventMultiCast()
        {}

        void EventMultiCast::fire()
        {
            strategy->notify(this);
        }

        void EventMultiCast::complete()
        {
            strategy->complete(this, myList);
        }

        void EventMultiCast::notifySyn()
        {
            myList->handleEvent(this);
        }

        void EventMultiCast::notifyAsyn()
        {
            cp->queue( myList, this );
        }

            struct completorSyn : public std::unary_function<EventCompleterInterface*,void>
            {
                void operator()(EventCompleterInterface* eci) const { eci->completeEvent(); }
            };

        void EventMultiCast::completeSyn( EventListenerInterface* eli )
        {
            for_each(myComp.begin(), myComp.end(), completorSyn() );
        }

            struct completorASyn : public std::binary_function<EventCompleterInterface*,CompletionProcessor*, void>
            {
                void operator()( EventCompleterInterface* eci, CompletionProcessor* cp) const { cp->queue(eci); }
            };

        void EventMultiCast::completeAsyn( EventListenerInterface* eli )
        {
            std::for_each(myComp.begin(), myComp.end(), std::bind2nd(completorASyn(),cp) );
        }
}
