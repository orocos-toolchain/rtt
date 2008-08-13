/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  SimulationActivity.hpp

                        SimulationActivity.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
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


#ifndef TASKSIMULATION_HPP
#define TASKSIMULATION_HPP

#include "PeriodicActivity.hpp"

namespace RTT
{
    /**
     * @brief A SimulationActivity is a PeriodicActivity which is used
     * for simulation.
     *
     * They all run in the same thread, which updates the
     * the system clock such that the SimulationActivity appears to run
     * in realtime, while it runs 'as fast as possible'.
     *
     * They will run in the SimulationThread. See SimulationThread::Instance
     * for setting the smallest activity period in the system.
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
         * executed by the user as well.
         */
        virtual bool execute();

        /**
         * Destroys this instance. The Activity is stopped.
         */
        virtual  ~SimulationActivity();

    };

}

#endif
