#include "corelib/TaskTimerLockFree.hpp"
#include "corelib/PeriodicTask.hpp"

#include "corelib/Logger.hpp"
#include <boost/bind.hpp>
#include <assert.h>

namespace ORO_CoreLib
{
    using namespace detail;

    TaskTimerLockFree::TaskTimerLockFree( nsecs task_period ) 
        : trig_per( task_period), task_per( task_period ), turn(1), tasks(TaskTimerInterface::MAX_TASKS)
        {
        }

    TaskTimerLockFree::~TaskTimerLockFree()
    {}

    bool TaskTimerLockFree::addTask( PeriodicTask* t ) {
        if ( tasks.size() == TaskTimerInterface::MAX_TASKS ) {
//             Logger::log() << Logger:: << "TaskTimerLockFree : tasks queue full, failed to add Task : "<< t << Logger::endl;
            return false;
        }
        tasks.append( t );
//         Logger::log() << Logger::Debug << "TaskTimerLockFree : successfully started Task : "<< t  << Logger::endl;
        return true;
    }

    bool TaskTimerLockFree::removeTask( PeriodicTask* t ) {
//             Logger::log() << Logger::Debug << "TaskTimerLockFree : successfully stopped Task : "<< t  << Logger::endl;
        return tasks.erase_and_blank(t, 0 );
    }

    void TaskTimerLockFree::start() {
        turn = 1;
    }

    void TaskTimerLockFree::stop() {
        tasks.apply( boost::bind( &PeriodicTask::stop, _1 ) );
    }

    void TaskTimerLockFree::tick()
    {
        // apply_and_blank will not apply if an item is blanked.
        if ( turn == 1 )
            tasks.apply_and_blank( boost::bind( &PeriodicTask::doStep, _1 ), 0 );

        if ( turn * trig_per == task_per )
            turn = 1;
        else
            ++turn;
    }

}
