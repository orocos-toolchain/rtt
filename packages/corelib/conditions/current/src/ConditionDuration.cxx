
#include "corelib/ConditionDuration.hpp"

namespace ORO_CoreLib
{

    ConditionDuration::ConditionDuration( nsecs t, bool invert )
        : time( t ), hb( HeartBeatGenerator::Instance() ),
          mark( 0 ), _invert( invert )
    {}

    ConditionDuration::~ConditionDuration()
    {}

    bool ConditionDuration::evaluate()
    {
        return _invert != ( time < HeartBeatGenerator::ticks2nsecs( hb->ticksSince( mark ) ) );
    }

    void ConditionDuration::reset()
    {
        mark = hb->ticksGet();
    }

    ConditionInterface* ConditionDuration::clone() const
    {
        return new ConditionDuration( time, _invert );
    }
}
