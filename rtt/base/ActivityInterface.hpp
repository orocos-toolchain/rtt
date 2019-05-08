/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  RunnableActivityInterface.hpp

                        ActivityInterface.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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

#ifndef ORO_ACTIVITY_INTERFACE_HPP
#define ORO_ACTIVITY_INTERFACE_HPP

#include "../Time.hpp"
#include "../os/ThreadInterface.hpp"
#include <boost/shared_ptr.hpp>
#include "rtt-base-fwd.hpp"

namespace RTT
{ namespace base {

    /**
     * @brief Interface to start/stop and query a Activity.
     *
     * A ActivityInterface provides the control methods
     * for activities. An activity can be periodic, non periodic, event driven
     * or any activity object which can be started, stopped
     * and be queried for their state and (optional) period.
     *
     * It is complementary to the RunnableInterface, which
     * defines the methods for the functionality that is executed.
     * @see RunnableInterface
     * @ingroup CoreLibActivities
     */
    class RTT_API ActivityInterface
    {
    protected:
        RunnableInterface* runner;

        /**
         * This method is only meant for RunnableInterface (ie \a runner)
         * in order to inform the ActivityInterface it should
         * no longer be used. run(0) can not be used in this case
         * because it would recurse.
         */
        void disableRun(RunnableInterface* caller) { if (caller == runner) runner = 0; }
    public:
        friend class RunnableInterface;

        typedef boost::shared_ptr<ActivityInterface> shared_ptr;

        ActivityInterface() : runner(0) {}

        ActivityInterface(RunnableInterface* run);

        virtual ~ActivityInterface();

        /**
         * Run exclusively this RunnableInterface.
         * @pre this->isRunning() == false
         * @param r
         *        The RunnableInterface to run exclusively.
         * @return true if succeeded, false otherwise
         */
        virtual bool run( RunnableInterface* r );

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
         * is executed. More-over, an Activity may decide to be
         * temporarily not running (not executing code), \a waiting for a signal
         * to proceed. If this->isActive() and !this->isRunning()
         * then the Activity is in a \a waiting state.
         *
         * @return true if it is running, false otherwise
         */
        virtual bool isRunning() const = 0;

        /**
         * Query if the activity is started.
         * This is less strict than isRunning(), it is true during
         * initialize(), step() or loop() and finalize(). Use
         * this method to check if an activity was start()ed.
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
         * Set the periodicity of this activity in Seconds.
         * Note that not all activity implementation support periods. A period
         * of s == 0 indicates non periodic. A non supported setting returns false.
         *
         * @return true if it could be updated, false otherwise.
         */
        virtual bool setPeriod(Seconds s)  = 0;


        /**
         * Get the cpu affinity of this activity
         *
         * @return The cpu affinity of this activity.
         */
        virtual unsigned getCpuAffinity() const = 0;

        /**
         * Set the cpu affinity of this activity.
         * @return true if it could be updated, false otherwise.
         */
        virtual bool setCpuAffinity(unsigned cpu)  = 0;

        /**
         * Execute this activity such that it \a executes a step or loop of the RunnableInterface.
         * When you invoke execute() you intend to call the step() or loop() methods.
         * Some activity implementations allow a user controlled execute, others ignore it,
         * in which case execute() returns false.
         *
         * Semantics: If execute() returns true, the activity has been executed exactly once
         * during execute().
         *
         * @retval true When this->isActive() and the implementation allows external
         * executes.
         * @retval false When !this->isActive() or the implementation does not
         * allow external updating.
         * @see trigger() for use in callbacks which want execute() to be executed.
         */
        virtual bool execute() = 0;

        /**
         * Trigger that work has to be done. When you invoke
         * trigger(), you intend to notify the mechanism that calls
         * execute(), that execute() should be called.  This allows a
         * separation between actually executing code (execute()) and
         * notifying that code must be executed (trigger()). A trigger
         * may be ignored by the implementation, in which case trigger
         * returns false.
         *
         * Semantics: If trigger() returns true, the activity will be executed at least
         * once from the moment trigger() is called.
         *
         * Requests this Activity to wakeup and call step() + work(Trigger).
         * If the thread is periodic, it will continue sleeping for the remainder of the time
         * after the work() has finished.
         *
         * @retval true When this->isActive() and the implementation allows external
         * triggers.
         * @retval false When !this->isActive() or the implementation does not
         * allow external triggering.
         */
        virtual bool trigger() = 0;

        /**
         * Requests this Activity to wakeup and call step() + work(Timeout).
         * Will be ignored for periodic activities, since they use an internal
         * timing mechanism, but can be used for non-periodic activities which
         * want to emulate a timeout happening towards the base::RunnableInterface.
         */
        virtual bool timeout()  = 0;

        /**
         * Returns a pointer to the thread which will
         * run this activity. Will not be null.
         */
        virtual os::ThreadInterface* thread() = 0;

        /**
         * Returns a pointer to the RunnableInterface instance
         */
        virtual RunnableInterface* getRunner() const;
    };

}}

#endif
