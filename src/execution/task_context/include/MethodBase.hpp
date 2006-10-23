#ifndef ORO_METHOD_BASE_HPP
#define ORO_METHOD_BASE_HPP

#include "Invoker.hpp"
#include "ActionInterface.hpp"
        
namespace RTT
{
    namespace detail 
    {
        /**
         * The base class for all method implementations. Both local and remove
         * method implementations must inherit from this class.
         */
        template<class F>
        struct MethodBase
            : public InvokerBase<F>,
              public ActionInterface
        {
            virtual ~MethodBase() {}
            virtual MethodBase<F>* cloneI() const = 0;
        };
    }
}

#endif
