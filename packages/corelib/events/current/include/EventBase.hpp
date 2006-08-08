
#ifndef ORO_CORELIB_EVENT_BASE_HPP
#define ORO_CORELIB_EVENT_BASE_HPP

#include "Invoker.hpp"
#include "Subscriber.hpp"
#include "ActionInterface.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * The base class of Event implementations.
         */
        template<
            typename Signature // function type R (T1, T2, ..., TN)
        >
        class EventBase
            : public detail::InvokerBase<Signature>,
              public detail::SubscriberBase<Signature>,
              public ActionInterface
        {
        public:
            typedef typename detail::SubscriberBase<Signature>::SlotFunction SlotFunction;

            void readArguments() {}
            bool execute() {return false;}

            virtual int arity() const = 0;
        };
    }
}

#endif
