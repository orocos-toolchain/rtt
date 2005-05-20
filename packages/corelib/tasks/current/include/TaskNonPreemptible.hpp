/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  TaskNonPreemptible.hpp 

                        TaskNonPreemptible.hpp -  description
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
 
#ifndef TASKNONPREEMPTIBLE_HPP
#define TASKNONPREEMPTIBLE_HPP

#include "PeriodicTask.hpp"

namespace ORO_CoreLib
{
    /**
     * @brief A TaskNonPreemptible is a PeriodicTask which will never be preempted by
     * another PeriodicTask.
     *
     * Typical TaskNonPreemptible instances are
     * fast calculations with fast device input and output communication.
     * 
     * They will be run in the ZeroTimeThread.
     */
    class TaskNonPreemptible
        : public PeriodicTask
    {
    public:
        /**
         * Construct a TaskNonPreemptible with a periodicity <period>
         * Seconds
         */
        TaskNonPreemptible(Seconds period, RunnableInterface* r=0, bool private_event_processor = false );

        /**
         * Construct a TaskNonPreemptible with a periodicity of
         * <s> seconds and <ns> nanoseconds
         */
        TaskNonPreemptible(secs s, nsecs ns, RunnableInterface* r=0, bool private_event_processor = false );

        /**
         * Destroys this instance. The Task is stopped.
         */
        virtual  ~TaskNonPreemptible();
                                 
    };

}

#endif
