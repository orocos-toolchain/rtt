/***************************************************************************
  tag: Peter Soetens  Fri Feb 11 15:59:12 CET 2005  TimerInterface.hpp 

                        TimerInterface.hpp -  description
                           -------------------
    begin                : Fri February 11 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
#ifndef ACTIVITYTIMERINTERFACE_HPP
#define ACTIVITYTIMERINTERFACE_HPP

#include "Time.hpp"

namespace ORO_CoreLib
{
    class PeriodicActivity;
    namespace detail {
    /**
     * An TimerInterface is an object that will step() a PeriodicActivity every
     * n'th time it is tick()'ed such that when n activities are present,
     * each activity will be stepped on a different tick call of one period.
     * It will try to spread the stepping
     * depending on the activity's periods and tick period.
     * The former is the period between two consecutive step() calls,
     *  the latter, the elapsed time between two tick() calls.
     *
     */
    class TimerInterface
    {
    public:
        virtual ~TimerInterface() {}

        /**
         * The maximum number of activities each timer will handle, thus,
         * the max number of activities of the same priority.
         */
        static const unsigned int MAX_ACTIVITIES = 64;

        /**
         * Startup the timer.
         */
        virtual void start() = 0;

        /**
         * Stop the timer and all PeriodicActivities running in this timer.
         */
        virtual void stop() = 0;

        /**
         * Add an activity which must be step()'ed.
         */
        virtual bool addActivity( PeriodicActivity* t ) = 0;

        /**
         * Remove an activity which must no longer be step()'ed.
         */
        virtual bool removeActivity( PeriodicActivity* t ) = 0;

        /**
         * Execute a PeriodicActivity, according to the average period.
         */
        virtual void tick() = 0;

        /**
         * Returns the (average) periodicity ( in nano s ) with which the activities are step()'ed.
         */
        virtual nsecs getPeriod() const = 0;

        /**
         * Returns the (average) periodicity ( in nano s ) with which the timer is tick()'ed.
         */
        virtual nsecs getTrigger() const = 0;

        /**
         * Sets the (average) period at which this timer will be ticked.
         */
        virtual void setTrigger(nsecs ns) = 0;
        
    };
    }
}

#endif
