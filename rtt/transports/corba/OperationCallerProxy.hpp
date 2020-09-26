/***************************************************************************
  tag: Peter Soetens Thu Dec 2 12:51:36 2010 +0100 OperationCallerProxy.hpp

                        OperationCallerProxy.hpp -  description
                           -------------------
    begin                : Thu Dec 2 2010
    copyright            : (C) 2010 Peter Soetens
    email                : peter@thesourceworks.com

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "ServiceRequesterC.h"
#include <string>
#include "rtt/base/OperationCallerBaseInvoker.hpp"
#include "rtt/transports/corba/CorbaOperationCallerFactory.hpp"

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
