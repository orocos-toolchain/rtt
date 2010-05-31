/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:07 CEST 2009  Activity.hpp

                        Activity.hpp -  description
                           -------------------
    begin                : Thu October 22 2009
    copyright            : (C) 2009 Peter Soetens
    email                : peter@thesourcworks.com

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


#ifndef ORO_ACTIVITY_HPP
#define ORO_ACTIVITY_HPP

#include "Time.hpp"
#include "base/RunnableInterface.hpp"
#include "base/ActivityInterface.hpp"
#include "os/Thread.hpp"

namespace RTT
{

    /**
     * @brief An Activity is an object that represents a thread.
     *
     * This object implements the base::ActivityInterface and maps that to an
     * OS thread, using the RTT::os::Thread class. One Activity object
     * maps to one OS thread. It can execute periodically the step() function
     * or the loop() function in case of non-periodic execution.
     *
     * When provided one, it will execute a base::RunnableInterface object, or the equivalent methods in
     * it's own interface when none is given.
     *
     * @ingroup CoreLibActivities
     */
    class RTT_API Activity
        : public base::ActivityInterface, public os::Thread
    {
    public:
        /**
         * @brief Create a not real-time Activity.
         * This creates a not real-time, non-periodic thread.
         *
         * @param _r The optional runner, if none, this->loop() is called.
         * @param name The name of the underlying thread.
         */
        Activity(base::RunnableInterface* r = 0, const std::string& name ="Activity" );

        /**
         * @brief Create a real-time Activity with a given priority.
         * The thread is run in the ORO_SCHED_RT scheduler.
         *
         * @param priority
         *        The priority of this activity.
         * @param r
         *        The optional base::RunnableInterface to run exclusively within this Activity
         * @param name The name of the underlying thread.
         */
        Activity(int priority,
                 base::RunnableInterface* r = 0, const std::string& name ="Activity");

        /**
         * @brief Create a real-time Activity with a given priority and period.
         * The thread is run in the ORO_SCHED_RT scheduler.
         *
         * @param priority
         *        The priority of this activity.
         * @param period
         *        The periodicity of the Activity. A zero is interpreted as non periodic.
         * @param r
         *        The optional base::RunnableInterface to run exclusively within this Activity
         * @param name The name of the underlying thread.
         */
        Activity(int priority, Seconds period,
                 base::RunnableInterface* r = 0, const std::string& name ="Activity");

        /**
         * @brief Create an Activity with a given scheduler type, priority and period.
         *
         * @param scheduler
         *        The scheduler in which the activity's thread must run. Use ORO_SCHED_OTHER or
         *        ORO_SCHED_RT.
         * @param priority
         *        The priority of this activity.
         * @param period
         *        The periodicity of the Activity
         * @param r
         *        The optional base::RunnableInterface to run exclusively within this Activity
         * @param name The name of the underlying thread.
         */
        Activity(int scheduler, int priority, Seconds period,
                 base::RunnableInterface* r = 0, const std::string& name ="Activity");

        /**
         * Stops and terminates a Activity
         */
        virtual ~Activity();

        virtual bool start();

        virtual bool execute();

        virtual bool trigger();

        virtual bool stop();

        virtual bool isRunning() const;

        virtual bool isActive() const;

        virtual bool isPeriodic() const;

        virtual Seconds getPeriod() const;

        virtual bool setPeriod(Seconds period);

        virtual os::ThreadInterface* thread();

        /**
         * @see base::RunnableInterface::initialize()
         */
        virtual bool initialize();

        /**
         * @see base::RunnableInterface::step()
         */
        virtual void step();

        /**
         * @see base::RunnableInterface::loop()
         */
        virtual void loop();

        /**
         * @see base::RunnableInterface::breakLoop()
         */
        virtual bool breakLoop();

        /**
         * @see base::RunnableInterface::finalize()
         */
        virtual void finalize();

    };

}

#endif
