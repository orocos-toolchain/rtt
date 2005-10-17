#include "TimeService.hpp"
#include "corelib/DataSource.hpp"

namespace ORO_CoreLib
{
    /**
     * A DataSource which returns the time elapsed since the last reset
     * in Seconds.
     */
    class DataSourceTime
        : public DataSource<TimeService::Seconds>
    {
        TimeService::ticks stamp;
        TimeService* ts;
    public:
        DataSourceTime()
            : stamp( 0 ), ts( TimeService::Instance() )
        {}
        
        TimeService::Seconds get() const {
            return ts->secondsSince(stamp);
        }

        void reset() {
            // get new pointer to ts also ?
            stamp = ts->getTicks();
        }

        DataSourceTime* clone() const {
            return new DataSourceTime();
        }

        DataSourceTime* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) {
            return new DataSourceTime();
        }
        
    };
}
