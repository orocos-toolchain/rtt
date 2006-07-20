/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  NonRealTimeActivity.hpp 

                        NonRealTimeActivity.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
#ifndef TASKNONREALTIME_HPP
#define TASKNONREALTIME_HPP

#include "PeriodicActivity.hpp"

namespace RTT
{
    /**
     * @brief A NonRealTimeActivity is a PeriodicActivity which will never preempt
     * another PeriodicActivity.
     *
     * Typical NonRealTimeActivity instances are
     * not realtime user communications, GUI updates and so on. 
     * They will run in the NonRealTimeThread.
     */
    class NonRealTimeActivity
        : public PeriodicActivity
    {
    public:
        /**
         * Construct a NonRealTimeActivity with a periodicity \a period
         * Seconds
         */
        NonRealTimeActivity(Seconds period, RunnableInterface* r=0, bool private_event_processor = false  );

        /**
         * Construct a NonRealTimeActivity with a periodicity of
         * \a s seconds and \a ns nanoseconds
         */
        NonRealTimeActivity(secs s, nsecs ns, RunnableInterface* r=0, bool private_event_processor = false  );

        /**
         * Destroys this instance. The Activity is stopped.
         */
        virtual  ~NonRealTimeActivity();
                                 
    };

}

#endif
