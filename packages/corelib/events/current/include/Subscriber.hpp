
#ifndef ORO_CORELIB_SUBSCRIBER_BASE_HPP
#define ORO_CORELIB_SUBSCRIBER_BASE_HPP

#include "Handle.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * The base class of Subscriber implementations.
         */
        template<
            typename Signature //! function type R (T1, T2, ..., TN)
        >
        class SubscriberBase
        {
        public:
            typedef boost::function<Signature> SlotFunction;

            virtual ~SubscriberBase() {}

            /**
             * @brief Connect a function callback to this subscriber.
             */
            virtual Handle connect(const SlotFunction& f) = 0;

            /**
             * @brief Setup a function callback to this subscriber.
             */
            virtual Handle setup(const SlotFunction& f) = 0;

        };
    }
}

#endif
