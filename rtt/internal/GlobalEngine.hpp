#ifndef ORO_GLOBALENGINE_HPP_
#define ORO_GLOBALENGINE_HPP_

/**
 *
 */
#include "../ExecutionEngine.hpp"
#include <boost/shared_ptr.hpp>

namespace RTT
{

    namespace internal
    {

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
