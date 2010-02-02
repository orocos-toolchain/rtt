#ifndef ORO_METHODBASEINVOKER_HPP_
#define ORO_METHODBASEINVOKER_HPP_

#include <string>
#include "DisposableInterface.hpp"
#include "../interface/rtt-interface-fwd.hpp"

namespace RTT
{
    namespace base {
        /**
         * An invoker can be given an implementation and is identified by a name.
         */
        struct MethodBaseInvoker
        {
            virtual bool ready() const = 0;
            virtual bool setImplementation(boost::shared_ptr<base::DisposableInterface> impl, ExecutionEngine* caller = 0) = 0;
            virtual bool setImplementation(interface::OperationRepositoryPart* orp, ExecutionEngine* caller = 0) = 0;
            virtual const std::string& getName() const = 0;
        };

    }
}

#endif /* ORO_METHODBASEINVOKER_HPP_ */
