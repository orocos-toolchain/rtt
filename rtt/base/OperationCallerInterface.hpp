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

            OperationCallerInterface();

            virtual ~OperationCallerInterface();

            /**
             * Available such that implementations have a way to
             * expose their ready-ness, ie being able to do the call.
             */
            virtual bool ready() const = 0;

            /**
             * Set the ExecutionEngine of the task which owns this method.
             * @param ee The ExecutionEngine of the component that
             * owns this operation.
             */
            void setOwner(ExecutionEngine* ee);

            /**
             * Set an executor which will execute this method
             * when it is called or sent. If ee is set to 0,
             * the method will be executed in the client's thread or
             * the GlobalExecutionEngine.
             * @param ee The ExecutionEngine of the component that
             * is executing this operation.
             */
            void setExecutor(ExecutionEngine* ee);

            /**
             * Sets the caller's engine of this operation.
             * This object will be used to test if asynchronous
             * messaging is required.
             * @param ee The ExecutionEngine of the component that
             * is calling this operation.
             */
            void setCaller(ExecutionEngine* ee);

            /**
             * Sets the Thread execution policy of this object.
             * @param et OwnThread or ClientThread.
             * @param executor The engine of the component owning this
             * operation. In case it is not yet owned by a component,
             * executor may be null.
             * @return false if it may not be modified.
             */
            bool setThread(ExecutionThread et,
                           ExecutionEngine* executor);

            /**
             * Executed when the operation execution resulted in a
             * C++ exception. Must report the error to the ExecutionEngine
             * of the owner of this operation.
             */
            void reportError();

            /**
             * Helpful function to tell us if this operations is to be sent or not.
             */
            bool isSend() { return met == OwnThread && myengine != caller; }
        protected:
            ExecutionEngine* myengine;
            ExecutionEngine* caller;
            ExecutionEngine* ownerEngine;
            ExecutionThread met;
        };
    }
}

#endif
