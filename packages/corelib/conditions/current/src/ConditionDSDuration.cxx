
#include "corelib/ConditionDSDuration.hpp"

namespace ORO_CoreLib
{

    ConditionDSDuration::ConditionDSDuration( DataSource<double>::shared_ptr ds, bool invert )
        : time( ds ), hb( TimeService::Instance() ),
          mark( 0 ), _invert( invert )
    {}

    ConditionDSDuration::~ConditionDSDuration()
    {}

    bool ConditionDSDuration::evaluate()
    {
        return _invert != ( time->get() > hb->secondsSince( mark ) );
    }

    void ConditionDSDuration::reset()
    {
        mark = hb->getTicks();
    }

    ConditionInterface* ConditionDSDuration::clone() const
    {
        return new ConditionDSDuration( time, _invert );
    }

    ConditionInterface* ConditionDSDuration::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
    {
        return new ConditionDSDuration( time->copy(alreadyCloned), _invert );
    }
}
