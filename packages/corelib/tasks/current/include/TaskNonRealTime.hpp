/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  TaskNonRealTime.hpp 

                        TaskNonRealTime.hpp -  description
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

#include "NonRealTimeThread.hpp"
#include "RealTimeTask.hpp"

namespace ORO_CoreLib
{
    /**
     * A TaskNonRealTime is a RealTimeTask which will never preempt
     * another RealTimeTask. Typical TaskNonRealTime instances are
     * not realtime user communications, GUI updates and so on. 
     * 
     * They can only be run in the NonRealTimeThread.
     */
    class TaskNonRealTime
        : public RealTimeTask
    {
    public:
        /**
         * Construct a TaskNonRealTime with a periodicity <period>
         * Seconds
         */
        TaskNonRealTime(Seconds period, RunnableInterface* r=0 );

        /**
         * Construct a TaskNonRealTime with a periodicity of
         * <s> seconds and <ns> nanoseconds
         */
        TaskNonRealTime(secs s, nsecs ns, RunnableInterface* r=0 );

        /**
         * Destroys this instance. The Task is stopped.
         */
        virtual  ~TaskNonRealTime();
                                 
    protected:
        virtual bool taskAdd();

        virtual void taskRemove(); 

        /**
         * The NonRealTimeThread used for scheduling this task
         */
        NonRealTimeThread* cproc;

    };

}

#endif
