/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  SimulationThread.hpp 

                        SimulationThread.hpp -  description
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
 
 

#ifndef SIMULATIONTHREAD_HPP
#define SIMULATIONTHREAD_HPP

#include "TimerThread.hpp"


namespace RTT
{

    class SimulationActivity;
    class SimulationThread;
    class TimeService;
    typedef boost::shared_ptr<SimulationThread> SimulationThreadPtr;

    /**
     * This thread is the simulated real-time periodic thread in the
     * Orocos system. It behaves (to the SimulationActivity) like a
     * perfect scheduler by executing all activities one after the
     * other in a continuous loop and updating the system time, such
     * that it appears to the activities as they are executed periodically.
     * 
     * All your activities in the same program must be a SimulationActivity for this
     * to work, since the TimeService global time is updated when this thread runs.
     *
     * By default, the update period is 0.001 seconds. If you want to run
     * with a finer or coarser grained time step, use the Instance() method and
     * supply another period before SimulationActivities are created.
     * @see TimerThread, ZeroTimeThread
     */
    class SimulationThread
        : public TimerThread
    {
    public:
        /**
         * Create the SimulationThread with a given period.  Only one
         * SimulationThread can be created.
         *
         * @param period The period in seconds at which the simulation
         * takes steps and updates the TimeService. Only the first
         * invocation of Instance will consider this parameter. The
         * others will ignore it.
         */
        static SimulationThreadPtr Instance(double period = 0.001);

        /**
         * Releases the SimulationThread
         * Reference counting might aid in making this call safe
         *
         * @return true on success, false on failure
         */
        static bool Release();

        /**
         * Destructor
         */
        virtual ~SimulationThread();

        virtual bool start()
        {
            maxsteps_ = 0;
            return OS::PeriodicThread::start();
        }

        /**
         * Only run the simulation \a maxsteps time steps, then stop the SimulationThread.
         * @return false if maxsteps == 0 or if thread could not be started.
         */
        virtual bool start(unsigned int maxsteps)
        {
            if (maxsteps == 0)
                return false;
            maxsteps_ = maxsteps;
            return OS::PeriodicThread::start();
        }

        using OS::PeriodicThread::run;
        /**
         * Execute \a maxsteps steps immediately. This function will
         * call the step() functions \a maxsteps times, without requiring
         * start() or stop() to be called before or after. The thread is
         * thus not used.
         * @return false if \a maxsteps == 0
         * @post When run() returns, step() has been called \a maxsteps times.
         */
        virtual bool run(unsigned int maxsteps);

    protected:
        bool initialize();
        void step();
        void finalize();

        /**
         * Constructor
         */
        SimulationThread(double period);

    private:

        /**
         * Our only instance of the SimulationThread
         */
        static SimulationThreadPtr _instance;

        /**
         * The System clock.
         */
        TimeService* beat;

        unsigned int maxsteps_, cursteps;
    };
} // namespace RTT

#endif
