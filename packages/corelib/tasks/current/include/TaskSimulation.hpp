/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  TaskSimulation.hpp 

                        TaskSimulation.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#ifndef TASKSIMULATION_HPP
#define TASKSIMULATION_HPP

#include "SimulationThread.hpp"
#include "RealTimeTask.hpp"

namespace ORO_CoreLib
{
    /**
     * @brief A TaskSimulation is a RealTimeTask which is used
     * for simulation.
     *
     * They all run in one thread, which updates the 
     * the system clock such that the task thinks it runs
     * in realtime, while it runs 'as fast as possible'.
     * 
     * They will run in the SimulationThread.
     */
    class TaskSimulation
        : public RealTimeTask
    {
    public:
        /**
         * Construct a TaskSimulation with a periodicity <period>
         * Seconds
         */
        TaskSimulation(Seconds period, RunnableInterface* r=0 );
        /**
         * Construct a TaskSimulation with a periodicity of
         * <s> seconds and <ns> nanoseconds
         */
        TaskSimulation(secs s, nsecs ns, RunnableInterface* r=0 );

        /**
         * Destroys this instance. The Task is stopped.
         */
        virtual  ~TaskSimulation();
                                 
    protected:
        virtual bool taskAdd();

        virtual void taskRemove(); 

        virtual TaskThreadInterface* thread() const { return simt; }

        /**
         * The SimulationThread used for scheduling this task
         */
        SimulationThread* simt;

    };

}

#endif
