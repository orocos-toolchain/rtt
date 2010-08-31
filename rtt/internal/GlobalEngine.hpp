#ifndef ORO_GLOBALENGINE_HPP_
#define ORO_GLOBALENGINE_HPP_

#include "../ExecutionEngine.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT
{

    namespace internal
    {

        /**
         * A process-wide execution engine that processes every
         * asynchronous operation that is not being processed by
         * a TaskContext.
         *
         * In practice, this only occurs when a client does a 
         * send() on an OperationCaller which calls an Operation
         * with the ClientThread policy. 
         */
        class GlobalEngine: public RTT::ExecutionEngine
        {
            static GlobalEngine* mengine;
            boost::shared_ptr<base::ActivityInterface> mact;
            GlobalEngine();
            virtual ~GlobalEngine();
        public:
            RTT_API static ExecutionEngine* Instance();
            static void Release();
        };

    }

}

#endif /* ORO_GLOBALENGINE_HPP_ */
