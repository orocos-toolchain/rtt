#ifndef ORO_MESSAGE_BASE_HPP
#define ORO_MESSAGE_BASE_HPP

#include "Invoker.hpp"
#include "ActionInterface.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * The base class for all message implementations. Both local and remote
         * message implementations must inherit from this class.
         */
        template<class F>
        struct MessageBase
            : public InvokerBase<F>,
              public ActionInterface
        {
            virtual ~MessageBase() {}
            virtual MessageBase<F>* cloneI() const = 0;
        };
    }
}

#endif
