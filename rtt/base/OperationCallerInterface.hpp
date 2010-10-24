#ifndef ORO_OPERATION_CALLER_INTERFACE_HPP
#define ORO_OPERATION_CALLER_INTERFACE_HPP

#include "../rtt-fwd.hpp"
#include "DisposableInterface.hpp"
#include "OperationBase.hpp"

namespace RTT
{
    namespace base
    {
        /**
         * The interface class for operation callers.
         */
        struct OperationCallerInterface: public DisposableInterface
        {
            /**
             * Use this type for shared pointer storage of an
             * OperationCallerInterface object.
             */
            typedef boost::shared_ptr<OperationCallerInterface> shared_ptr;

            virtual ~OperationCallerInterface()
            {
            }

            /**
             * Available such that implementations have a way to
             * expose their ready-ness, ie being able to do the call.
             */
            virtual bool ready() const = 0;

            /**
             * Set an executor which will execute this method
             * when it is called or sent. If ee is set to 0,
             * the method will be executed in the client's thread or
             * the GlobalExecutionEngine.
             * @param ee The ExecutionEngine of the component that
             * is executing this operation.
             */
            virtual void setExecutor(ExecutionEngine* ee) = 0;

            /**
             * Sets the caller's engine of this operation.
             * This object will be used to test if asynchronous
             * messaging is required.
             * @param ee The ExecutionEngine of the component that
             * is calling this operation.
             */
            virtual void setCaller(ExecutionEngine* ee) = 0;

            /**
             * Sets the Thread execution policy of this object.
             * @param et OwnThread or ClientThread.
             * @param executor The engine of the component owning this
             * operation. In case it is not yet owned by a component,
             * executor may be null.
             * @return false if it may not be modified.
             */
            virtual bool setThread(ExecutionThread et,
                    ExecutionEngine* executor) = 0;
        };
    }
}

#endif
