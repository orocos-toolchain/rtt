/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  RunnableInterface.hpp

                        RunnableInterface.hpp -  description
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

#ifndef CORELIB_RUNNABLE_INTERFACE_HPP
#define CORELIB_RUNNABLE_INTERFACE_HPP

#include "ActivityInterface.hpp"

namespace RTT
{ namespace base {
    /**
     * @brief A class for running a certain piece of code in a thread.
     *
     * It defines three methods which can execute functionality.
     *
     * In a start-run-stop cycle, before step() is called the first time,
     * initialize() will be called in the thread that started this RunnableInterface.
     * When step() is ran the last time in this cycle, finalize() will be
     * called, after it finishes, in the threaad that stopped this RunnableInterface.
     *
     * A non periodic thread will call \a loop(), which indicates that the
     * RunnableInterface is allowed to block ( step() is not allowed to block ).
     * By default, loop() calls step(), but a subclass may override the loop() method
     * to put its own blocking functionality in. To break out of the loop() method,
     * reimplement \a breakLoop() such that loop() returns when breakLoop() is called.
     *
     * The getActivity() method is guaranteed to return a valid task
     * pointer during initialize(), step() or loop() and finalize(). This allows
     * the RunnableInterface to query the task's period(icity) and ThreadInterface.
     * Consequently, initialize() knows whether
     * step() or loop() will be called ( depending on ActivityInterface::isPeriodic() ).
     * @see ActivityInterface
     * @ingroup CoreLibActivities
     */
    class RTT_API RunnableInterface
    {
        /**
         * The Activityobject which owns this RunnableInterface.
         */
        ActivityInterface* owner_act;
    public:
        enum WorkReason { TimeOut = 0, Trigger, IOReady };
        /**
         * Create a runnable object. The optional constructor parameter
         * allows the object to attach directly to a thread. Otherwise,
         * os::ThreadInterface::run(RunnableInterface*) must be used to
         * attach this object to a thread. A thread can only run one
         * RunnableInterface object, use CoreLib tasks otherwise.
         * @param t The thread this object must attach to.
         */
        RunnableInterface();

        /**
         * Checks if this is still in a task and if so, issues a critical warning.
         */
        virtual ~RunnableInterface();

        /**
         * The method that will be called before the first periodical
         * execution of \a step() ( or non periodical execution of \a loop() ),
         * when the thread is started.
         */
        virtual bool initialize() = 0;

        /**
         * The method that will be (periodically) executed when this
         * object is run in an Activity.
         */
        virtual void step() = 0;

        /**
         * Identical to step() but gives a reason why the function was called.
         * Both step() and work() will be called an equal amount of times,
         * so you need to use only one, but work gives you the reason why.
         */
        virtual void work(WorkReason reason);

        /**
         * The method that will be executed once when this
         * class is run in a non periodic Activity. The default
         * implementation calls step() once.
         */
        virtual void loop();

        /**
         * This method is called by the framework to break out of the \a loop() method.
         * Reimplement this method to signal \a loop() to return and return
         * true on success. When this method is not reimplemented by you, it
         * will always return \a false, denoting that the loop can not
         * be broken. If breakLoop() returns \a true, the caller will wait
         * until loop() returns.
         * @return true if the loop could be notified to return.
         */
        virtual bool breakLoop();

        /**
         * The method that will be called after the last periodical
         * execution of \a step() ( or non periodical execution of \a loop() ),
         * when the RunnableInterface is stopped.
         */
        virtual void finalize() = 0;

        /**
         * Get the thread this object is run in.
         * @return a pointer to the thread or 0 if not run by a thread.
         */
        virtual os::ThreadInterface* getThread() const;

        /**
         * This method is for 'intelligent' activity implementations
         * that wish to see if it is required to call step() (again).
         * By default, \a false is returned. You should only return \a true in
         * case there is a temporary reason to (re-)run step.
         * @return \a true if this object should be run.
         * @see extras::SequentialActivity implementation to see how this can be
         * of use.
         */
        virtual bool hasWork();

        /**
         * @brief Query for the task this interface is run in.
         *
         * Zero denotes that no task is present to run
         * it, and hence no detailed information is available.
         *
         * @return The Activity which runs this RunnableInterface.
         */
        inline ActivityInterface* getActivity() const;

        /**
         * @brief Set the task this interface is run in.
         *
         * A Zero means no task is running it.
         *
         * @param task The ActivityInterface running this interface.
         */
        virtual void setActivity( ActivityInterface* task );
    };


    ActivityInterface* RunnableInterface::getActivity() const { return owner_act; }
}}

#endif
