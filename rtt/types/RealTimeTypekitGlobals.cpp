#include "rtt-config.h"
#include "RealTimeToolkit.hpp"
#include "Types.hpp"
#include "GlobalsRepository.hpp"
#include "../FlowStatus.hpp"
#include "../SendStatus.hpp"
#include "../ConnPolicy.hpp"

namespace RTT
{
    using namespace types;
    bool RealTimeToolkitPlugin::loadGlobals() {
        GlobalsRepository::shared_ptr globals = GlobalsRepository::Instance();

        // Data Flow enums:
        globals->setValue( new Constant<FlowStatus>("NoData",NoData) );
        globals->setValue( new Constant<FlowStatus>("OldData",OldData) );
        globals->setValue( new Constant<FlowStatus>("NewData",NewData) );
        globals->setValue( new Constant<SendStatus>("SendFailure",SendFailure) );
        globals->setValue( new Constant<SendStatus>("SendNotReady",SendNotReady) );
        globals->setValue( new Constant<SendStatus>("SendSuccess",SendSuccess) );
#ifndef ORO_EMBEDDED
        globals->setValue( new Constant<int>("DATA",ConnPolicy::DATA) );
        globals->setValue( new Constant<int>("BUFFER",ConnPolicy::BUFFER) );
        globals->setValue( new Constant<int>("LOCKED",ConnPolicy::LOCKED) );
        globals->setValue( new Constant<int>("LOCK_FREE",ConnPolicy::LOCK_FREE) );
        globals->setValue( new Constant<int>("UNSYNC",ConnPolicy::UNSYNC) );
#endif
        return true;
    }
}
