
#include "corelib/EventSimple.hpp"

namespace ORO_CoreLib
{
    using namespace detail;

        EventSimple::EventSimple(EventType t)
            : EventStrategyMethods(t), myList(0), myComp(0)
        {
            cp = CompletionProcessor::Instance();
        }
//         switch ( t )
//             {

//             case SYNSYN:
//                 strategy = &ss;
//                 break;

//             case SYNASYN:
//                 strategy = &sa;
//                 break;
//             case ASYNSYN:
//                 strategy = &as;
//                 break;
//             case ASYNASYN:
//                 strategy = &aa;
//                 break;
//             default:
//                 strategy = &ss;
//             }

//             cp = CompletionProcessor::Instance();
//         }

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
