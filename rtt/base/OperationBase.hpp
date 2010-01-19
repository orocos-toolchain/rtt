#ifndef ORO_OPERATIONBASE_HPP_
#define ORO_OPERATIONBASE_HPP_

#include "../rtt-fwd.hpp"
#include "rtt-base-fwd.hpp"
#include <string>
#include <vector>
#include "DisposableInterface.hpp"

namespace RTT
{
    template<class T>
    class Operation;

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
            void mdoc(const std::string& description);
            void marg(const std::string& name, const std::string& description);
        public:
            OperationBase(const std::string& name);
            virtual ~OperationBase();

            enum ExecutionThread { OwnThread, ClientThread };

            const std::string& getName() { return mname; }

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
             * @return
             */
            bool ready() const {
                return mowner && getImplementation();
            }

            void setOwner(ExecutionEngine* ee) { mowner = ee; }
        };
    }
}

#endif /* ORO_OPERATIONBASE_HPP_ */
