/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  SimulationActivity.hpp 

                        SimulationActivity.hpp -  description
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

#include "PeriodicActivity.hpp"

namespace RTT
{
    /**
     * @brief A SimulationActivity is a PeriodicActivity which is used
     * for simulation.
     *
     * They all run in one thread, which updates the 
     * the system clock such that the task thinks it runs
     * in realtime, while it runs 'as fast as possible'.
     * 
     * They will run in the SimulationThread.
     * @ingroup CoreLibActivities
     */
    class SimulationActivity
        : public PeriodicActivity
    {
    public:
        /**
         * Construct a SimulationActivity with a periodicity \a period
         * Seconds
         */
        SimulationActivity(Seconds period, RunnableInterface* r=0 );
        /**
         * Construct a SimulationActivity with a periodicity of
         * \a s seconds and \a ns nanoseconds
         */
        SimulationActivity(secs s, nsecs ns, RunnableInterface* r=0 );

        /**
         * A simulated activity can be started also if the
         * SimulationThread is not running yet. This allows
         * all tasks to be set ready in advance before the whole
         * simulation is started.
         */
        virtual bool start();

        /**
         * When a SimulationActivity is started, it may be
         * updated by the user as well.
         */
        virtual bool update();

        /**
         * Destroys this instance. The Activity is stopped.
         */
        virtual  ~SimulationActivity();
                                 
    };

}

#endif
