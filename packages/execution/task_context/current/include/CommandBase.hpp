#ifndef ORO_COMMAND_BASE_HPP
#define ORO_COMMAND_BASE_HPP

#include "Invoker.hpp"
#include "DispatchInterface.hpp"
        
namespace RTT
{
    namespace detail 
    {
        /**
         * The base class for all command implementations. Both local and remove
         * command implementations must inherit from this class.
         */
        template<class F>
        struct CommandBase
            : public InvokerBase<F>,
              public DispatchInterface
        {
            virtual ~CommandBase() {}
            virtual CommandBase<F>* cloneI() const = 0;
        };

    }
}

#endif
