#ifndef ORO_RTT_CORBA_TRANSPORT
#define ORO_RTT_CORBA_TRANSPORT

#include "../TaskObject.hpp"
#include "ControlTaskServer.hpp"

namespace RTT {
    namespace Corba {
        /**
         * A service that is installed in each component
         * that runs as a CORBA server.
         */
        class CorbaService
            : public TaskObject
        {
            TaskContext* mtc;
        public:
            CorbaService(TaskContext* tc)
                : TaskObject("corbaservice", "This component is available as a CORBA service"),
                  mtc(tc)
            {
            }

            ~CorbaService() {
                // CTS destructor will do cleanup of CORBA related objects.
                ControlTaskServer::CleanupServer( mtc );
            }
        };
    }
}

#endif
