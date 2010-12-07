#include "ServiceRequesterC.h"
#include <string>
#include "../../base/OperationCallerBaseInvoker.hpp"
#include "CorbaOperationCallerFactory.hpp"

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
                return msrq->callerReady( mname.c_str() );
            }

            virtual void disconnect() {
                msrq->disconnectCaller( mname.c_str() );
            }

            virtual bool setImplementation(boost::shared_ptr<base::DisposableInterface> impl, ExecutionEngine* caller = 0) {
                // impossible for remote operation callers.
                return false;
            }

            virtual bool setImplementationPart(OperationInterfacePart* orp, ExecutionEngine* caller = 0) {
                // todo: forward to remote side.
                // if orp is local, we need to provide the remote side with a service to this operation part.
                // if orp is remote, we need to pass on the service
                CorbaOperationCallerFactory* cocf = dynamic_cast<CorbaOperationCallerFactory*>(orp);
                if (cocf) {
                    // good: it's already a remote service, just pass on the misery
                    return msrq->connectCallerTo(mname.c_str(), cocf->getService() );
                } else {
                    // bad: we need to create or lookup
                    log(Error) <<"Can't connect an operation caller proxy directly to a local service."<<endlog();
                    log(Error) <<"Use CServiceRequester::connectTo() or CServiceRequester::connectCallerTo(). "<<endlog();
                }
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
