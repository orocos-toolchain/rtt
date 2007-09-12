/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  SimulationThread.hpp 

                        SimulationThread.hpp -  description
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
     * @see TimerThread
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

        virtual bool start();

        /**
         * Returns true if thread is running or run( unsigned int ) is being
         * invoked.
         */
        virtual bool isRunning() const;

        /**
         * Only run the simulation \a maxsteps time steps, then stop the SimulationThread.
         * @return false if maxsteps == 0 or if thread could not be started.
         */
        virtual bool start(unsigned int maxsteps);

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

        bool sim_running;
    };
} // namespace RTT

#endif
