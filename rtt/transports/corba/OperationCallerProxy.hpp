#include "ServiceRequesterC.h"
#include <string>
#include "../../base/OperationCallerBaseInvoker.hpp"

namespace RTT
{
    namespace corba {
        /**
         * Mirrors a remote operation caller in a ServiceRequestor.
         * Will forward our 'connect to' -> 'setImplementationPart'
         * requests to the remote side.
         */
        struct OperationCallerProxy
            : public base::OperationCallerBaseInvoker
        {
            std::string mname;
            CServiceRequester_var msrq;
            OperationCallerProxy(const std::string& opname, CServiceRequester_ptr srq)
            : mname(opname), msrq(srq) {}

            virtual bool ready() const {
                return msrq->ready();
            }

            virtual bool setImplementation(boost::shared_ptr<base::DisposableInterface> impl, ExecutionEngine* caller = 0) {
                // impossible for remote operation callers.
                return false;
            }

            virtual bool setImplementationPart(OperationInterfacePart* orp, ExecutionEngine* caller = 0) {
                // todo: forward to remote side.
                return false;
            }

            virtual void setCaller(ExecutionEngine* caller) {
                    // ignored.
            }

            virtual const std::string& getName() const {
                return mname;
            }
        };
    }
}
