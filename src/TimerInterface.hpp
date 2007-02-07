/***************************************************************************
  tag: Peter Soetens  Fri Feb 11 15:59:12 CET 2005  TimerInterface.hpp 

                        TimerInterface.hpp -  description
                           -------------------
    begin                : Fri February 11 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ACTIVITYTIMERINTERFACE_HPP
#define ACTIVITYTIMERINTERFACE_HPP

#include "Time.hpp"

namespace RTT
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
