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
            /**
             * Returns true if this Method has received a correct implementation.
             */
            virtual bool ready() const = 0;
            /**
             * Sets a new implementation for this method.
             * @param impl An implementation object that can be upcast to the MethodBase
             * class type of this method.
             * @param caller The engine of the calling TaskContext.
             * @return true if the impl could be upcast to a matching MethodBase type, false otherwise.
             */
            virtual bool setImplementation(boost::shared_ptr<base::DisposableInterface> impl, ExecutionEngine* caller = 0) = 0;
            /**
             * Sets a new implementation for this method by using a service part.
             * @param orp A part obtained from a service's OperationRepository.
             * @param caller The engine of the calling TaskContext.
             * @return true if the part could produce the correct MethodBase type, false otherwise.
             */
            virtual bool setImplementationPart(interface::OperationRepositoryPart* orp, ExecutionEngine* caller = 0) = 0;
            /**
             * Sets the caller of this method after the implementation was set.
             * @param caller
             */
            virtual void setCaller(ExecutionEngine* caller) = 0;
            /**
             * Returns the name of this method instance.
             */
            virtual const std::string& getName() const = 0;
        };

    }
}

#endif /* ORO_METHODBASEINVOKER_HPP_ */
