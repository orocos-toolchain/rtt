#include "os/ConditionVariableTimed.hpp"
#include "os/MutexLock.hpp"

namespace ORO_OS
{

    typedef HeartBeatGenerator::nsecs nsecs;

    ConditionVariableTimed::ConditionVariableTimed( ConditionInterface* ci, Mutex* mutex, double timeout ) :
            time( timeout ), condition( ci )
    {
        mut = mutex;
        rtos_cond_init( &cond, 0 );
    }

    ConditionVariableTimed::~ConditionVariableTimed()
    {
        rtos_cond_destroy( &cond );
    }

    ConditionInterface* ConditionVariableTimed::conditionGet()
    {
        /*
         * The timeout is relative, but rtos_cond_timedwait expects an absolute time.
         * Therefore we add the current time to the timeout.
         */
        int retcode = 0;

        TIME_SPEC ts( *hrt2ts( systemTimeGet() ) ) ;

        int temp = ( int ) time;
        ts.tv_sec += temp;
        ts.tv_nsec += ( int ) ( ( time - temp ) * 1000000000 );

        if ( ts.tv_nsec >= 1000000000 )
        {
            ts.tv_nsec -= 1000000000;
            ts.tv_sec += 1;
        }

        // Begin critical section: we use the value of <condition>.
        MutexLock locker( *mut );

        // Save the value of <condition>, so we can see when it changes.
        boolValue = condition->evaluate();

        /* Now we wait until the condition evaluates to a different
         * value than the initial value, or timeout occurs.
         */
        while ( condition->evaluate() == boolValue && retcode != ETIMEDOUT )
        {
            retcode = rtos_cond_timedwait( &cond, &(mut->m), &ts );
        }
        
        return condition;

    }

    void ConditionVariableTimed::broadcast()
    {
        rtos_cond_broadcast( &cond );
    }

}
