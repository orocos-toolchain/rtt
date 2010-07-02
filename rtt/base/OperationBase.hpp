#ifndef ORO_OPERATIONBASE_HPP_
#define ORO_OPERATIONBASE_HPP_

#include "../rtt-config.h"
#include "../rtt-fwd.hpp"
#include "rtt-base-fwd.hpp"
#include <string>
#include <vector>
#include "DisposableInterface.hpp"

namespace RTT
{
    template<class T>
    class Operation;

    /**
     * Users can choose if an operation is executed in the component's thread
     * or in the thread of the caller.
     */
    enum ExecutionThread { OwnThread, ClientThread };

    namespace base
    {
        /**
         * This base class serves as a template-less handle for operation
         * objects and also stores the name and documentation of an operation.
         */
        class OperationBase
        {
        protected:
            std::string mname;
            std::vector<std::string> descriptions;
            ExecutionEngine* mowner;
            RTT_API void mdoc(const std::string& description);
            RTT_API void marg(const std::string& name, const std::string& description);
            virtual void ownerUpdated() = 0;
        public:
            RTT_API OperationBase(const std::string& name);
            RTT_API virtual ~OperationBase();

            /**
             * Returns the name of this operation.
             * @return The name.
             */
            const std::string& getName() { return mname; }

            /**
             * Returns all the descriptions of this operation, as a single list,
             * starting with the operation's description, followed by the name,
             * description pairs of each argument.
             * @return A vector with all descriptions.
             */
            const std::vector<std::string>& getDescriptions() { return descriptions; }

            /**
             * Returns the implementation object of this operation.
             * @return
             */
            virtual DisposableInterface::shared_ptr getImplementation() = 0;

            virtual const DisposableInterface::shared_ptr getImplementation() const = 0;

            /**
             * An operation is ready if it has an implementation and is
             * present in a TaskContext.
             * @return true if ready.
             */
            bool ready() const {
                return mowner && getImplementation();
            }

            /**
             * Updates the owning ExecutionEngine that might execute this operation.
             * @param ee
             */
            void setOwner(ExecutionEngine* ee) {
                mowner = ee;
                // inform Operation<T> to update the implementation.
                this->ownerUpdated();
            }
        };
    }
}

#endif /* ORO_OPERATIONBASE_HPP_ */
