/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:29 CEST 2004  ConditionVariableTimed.cxx 

                        ConditionVariableTimed.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
