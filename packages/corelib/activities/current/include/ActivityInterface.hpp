/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  RunnableActivityInterface.hpp 

                        ActivityInterface.hpp -  description
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
 
#ifndef ORO_ACTIVITY_INTERFACE_HPP
#define ORO_ACTIVITY_INTERFACE_HPP

#include "Time.hpp"
#include "os/ThreadInterface.hpp"

namespace ORO_CoreLib
{
    class EventProcessor;
    class RunnableInterface;

    /**
     * @brief Interface to start/stop and query a Activity.
     *
     * A ActivityInterface provides the control methods
     * for activities. A activity can be periodic, non periodic, event driven
     * or any activity object which can be started, stopped
     * and be queried for their state and (optional) period.
     *
     * It is complementary to the RunnableInterface, which
     * defines the methods for the functionality that is executed.
     * @see RunnableInterface
     */
    class ActivityInterface
    {

    public:
        virtual ~ActivityInterface() {}

        /**
         * Run exclusively this RunnableInterface.
         * @pre this->isRunning() == false
         * @param r
         *        The RunnableInterface to run exclusively.
         * @return true if succeeded, false otherwise
         */
        virtual bool run( RunnableInterface* r ) = 0;

        /**
         * Start the activity.
         * This will call RunnableInterface::initialize() and upon
         * success, effectively start the activity, by running the
         * RunnableInterface::step() or RunnableInterface::loop() in
         * a thread.
         * @see isPeriodic()
         * 
         * @return true if the activity is started, false otherwise
         */
        virtual bool start() = 0;

        /**
         * Stop the activity
         * This will stop the activity by removing it from the 'run-queue'
         * of a thread or call RunnableInterface::breakLoop().
         * If no errors occured, RunnableInterface::finalize() is called.
         * @see isPeriodic()
         *
         * @return true if the activity is stopped, false otherwise
         */
        virtual bool stop() = 0;

        /**
         * Query if the activity is initialized and executing.
         * This is more strict than isActive(), it is only true
         * after initialize() is executed and before finalize()
         * is executed.
         *
         * @return true if it is running, false otherwise
         */
        virtual bool isRunning() const = 0;

        /**
         * Query if the activity is started.
         * This is less strict than isRunning(), it is true during
         * initialize(), step() or loop() and finalize().
         *
         * @return true if it is active, false otherwise
         */
        virtual bool isActive() const = 0;

        /**
         * Get the periodicity of this activity in Seconds
         *
         * @return The execution period of this activity (zero if !this->isPeriodic() ).
         */
        virtual Seconds getPeriod() const = 0;

        /**
         * Inspect if this activity is periodic. If so, it will call RunnableInterface::step().
         * If the activity is not periodic, it will call RunnableInterface::loop().
         *
         * @return true if periodic.
         */
        virtual bool isPeriodic() const = 0;

        /**
         * Returns a pointer to the EventProcessor which will
         * process the asynchronous Events of this activity. Will not be null.
         */
        virtual EventProcessor* getEventProcessor() const = 0;

        /**
         * Returns a pointer to the thread which will
         * run this activity. Will not be null.
         */
        virtual ORO_OS::ThreadInterface* thread() = 0;
    };

}

#endif
