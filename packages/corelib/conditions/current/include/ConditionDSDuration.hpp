#ifndef CONDITION_DS_DURATION_HPP
#define CONDITION_DS_DURATION_HPP

#include "ConditionInterface.hpp"
#include "TimeService.hpp"
#include "DataSource.hpp"

namespace ORO_CoreLib
{

    /**
     * A conditional that evaluates true after a certain time (in seconds) has passed,
     * given by a DataSource.
     * The behaviour can be inverted upon construction.
     */
    class ConditionDSDuration
        : public ConditionInterface
    {
        typedef TimeService::nsecs nsecs;
        typedef TimeService::ticks ticks;
        
    public:
        /**
         * Create a condition that changes after \a ds->get() seconds.
         * @param ds A DataSource containing the period of time starting from \a reset()
         * @param invert If \a false (default) then the condition will
         * raise to true after \a ds->get() seconds. If \a true then the condition will
         * fall to false after \a ds->get() seconds.
         */
        ConditionDSDuration( DataSource<double>::shared_ptr ds, bool invert = false );

        virtual ~ConditionDSDuration();

        virtual bool evaluate();

        virtual void reset();

        ConditionInterface* clone() const;

        virtual ConditionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const;
    private:

        /**
         * The expiration time, expressed in ticks
         */
        DataSource<double>::shared_ptr time;

        /**
         * A local reference to the HeartBeat Generator
         */
        TimeService *hb;
        /**
         * The time the evaluation is called the first time
         */
        ticks mark;

        /**
         * Invert the time comparison to 'smaller then'
         */
        bool _invert;
    };

}

#endif
