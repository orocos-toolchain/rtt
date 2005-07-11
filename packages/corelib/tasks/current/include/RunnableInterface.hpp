/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  RunnableInterface.hpp 

                        RunnableInterface.hpp -  description
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
 
#ifndef CORELIB_RUNNABLE_INTERFACE_HPP
#define CORELIB_RUNNABLE_INTERFACE_HPP

#include "os/RunnableInterface.hpp"
#include "TaskInterface.hpp"

namespace ORO_CoreLib
{
    /**
     * This class adds
     * Task specific semantics to ORO_OS::RunnableInterface and sits in the ORO_CoreLib
     * namespace. Applications should always use this class
     * instead of the ORO_OS version.
     *
     * The getTask() method is guaranteed to return a valid task
     * pointer during initialize(), step() or loop() and finalize(). This allows
     * the RunnableInterface to query the task's period(icity), ThreadInterface
     * and EventProcessor. Consequently, initialize() knows whether
     * step() or loop() will be called ( depending on TaskInterface::isPeriodic() ).
     * @see TaskInterface
     */
    class RunnableInterface
        : public ORO_OS::RunnableInterface
    {
        TaskInterface* owner_task;
    public:
        RunnableInterface() : owner_task(0) {}

        /**
         * @brief Query for the task this interface is run in.
         *
         * Zero denotes that no task is present to run
         * it, and hence no detailed information is available.
         *
         * @return The Task which runs this RunnableInterface.
         */
        TaskInterface* getTask() const { return owner_task; }

        /**
         * @brief Set the task this interface is run in.
         *
         * A Zero means no task is running it.
         *
         * @param task The TaskInterface running this interface.
         */
        void setTask( TaskInterface* task ) {
            owner_task = task;
        }
    };
}

#endif
