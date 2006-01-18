/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  TaskTimerLockFree.cxx 

                        TaskTimerLockFree.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
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
