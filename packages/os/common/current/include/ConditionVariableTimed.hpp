/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionVariableTimed.hpp 

                        ConditionVariableTimed.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 
#ifndef CONDITIONVARIABLETIMED_HPP
#define CONDITIONVARIABLETIMED_HPP

#include <os/fosi.h>
#include <errno.h>
#include "corelib/HeartBeatGenerator.hpp"
#include "ConditionVariableInterface.hpp"
#include "Mutex.hpp"



namespace ORO_OS
{

    /**
     * An object of this class is a condition variable that tests if a condition
     * is changed in a specified time.
     * @verbatim
     * EXAMPLE:
     *
     * ConditionBool condition;  // The initial value is false.
     * Mutex lock;
     * double timeout = 0.1; // Wait 0.1 second for a change of <condition>.
     * ConditionVariableTimed cv(&condition, &lock, timeout);
     * 
     * ...
     * 
     * { // The critical section where the condition changes.
     *  MutexLock locker(lock);
     *  condition.setTrue();
     *  cv.broadcast();  // Broadcast the possible change.
     * }
     *
     * ...
     * 
     * // Now wait 0.1 second for a change. For example, a change can happen in another thread. 
     * value = (cv.conditionGet())->evaluate();
     *@endverbatim
     * */

    class ConditionVariableTimed :
                public ConditionVariableInterface
    {

        public:

            /**
             * Construct a new ConditionVariableTimed object that tests a ConditionInterface object.
             * @param cond  The condition to test.
             * @param mutex  The mutex to secure <ci>.
             * @param timeout The number of seconds valueGet() waits for a change of <ci>.
             */
            ConditionVariableTimed( ORO_CoreLib::ConditionInterface* cond, Mutex* mutex, double timeout );

            /**
             * Destroy the ConditionVariableTimed object.
             */
            virtual ~ConditionVariableTimed();

            /**
             * Wait until the value of the ConditionInterface object changes or timeout is reached,
             * and return the value.
             */
            virtual ORO_CoreLib::ConditionInterface* conditionGet();

            virtual void broadcast();

        protected:
            // Forbidden for mortals.
            ConditionVariableTimed();

            /**
             * The timeout value.
             */
            double time;

            /**
             * The ConditionInterface object.
             */
            ORO_CoreLib::ConditionInterface* condition;

            /**
             * The evaluation of the condition.
             */
            bool boolValue;

            /**
             * The mutex to secure the condition variable.
             */
            Mutex* mut;

            /**
             * The condition variable. 
             */
            rt_cond_t cond;
    };

}

#endif // CONDITIONVARIABLETIMED_HPP
