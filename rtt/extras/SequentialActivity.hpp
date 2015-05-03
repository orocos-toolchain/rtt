/***************************************************************************
  tag: Peter Soetens  Thu Oct 22 11:59:08 CEST 2009  SequentialActivity.hpp

                        SequentialActivity.hpp -  description
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


#ifndef ORO_SEQUENTIAL_ACTIVITY_HPP
#define ORO_SEQUENTIAL_ACTIVITY_HPP

#include "../base/ActivityInterface.hpp"
#include "../base/RunnableInterface.hpp"
#include "../os/Mutex.hpp"

namespace RTT
{ namespace extras {


    /**
     * @brief The default, thread-less activity for any newly created TaskContext.
     *
     * The presence of this activity object indicates that the TaskContext does
     * not have its own active thread, but executes all asynchronous operations
     * (such as commands and events) in the thread of the caller, so in fact
     * synchronously. The SequentialActivity uses a os::Mutex lock to guard against
     * concurrent executions and makes your TaskContext equally thread-safe as
     * the other activity implementations.
     *
     * This activity emulates the same behaviour as a NonPeriodicActivity, but
     * without using a thread.
     *
     * \section ExecReact Reactions to execute():
     * Always returns false.
     *
     * \section TrigReact Reactions to trigger():
     * This causes step() to be executed.
     *
     * @ingroup CoreLibActivities
     */
    class RTT_API SequentialActivity
        :public base::ActivityInterface
    {
    public:
        /**
         * Create an activity which is the Sequential. The period will be 0.0.
         * @param run Run this instance.
         */
        SequentialActivity( base::RunnableInterface* run = 0 );

        /**
         * Cleanup and notify the base::RunnableInterface that we are gone.
         */
        ~SequentialActivity();

        Seconds getPeriod() const;

        bool setPeriod(Seconds s);

        unsigned getCpuAffinity() const;

        bool setCpuAffinity(unsigned cpu);

        os::ThreadInterface* thread();

        bool initialize();
        void step();
        void loop();
        bool breakLoop();
        void finalize();

        bool start();

        bool stop();

        bool isRunning() const;

        bool isPeriodic() const;

        bool isActive() const;

        bool execute();

        bool trigger();

        bool timeout();
    private:
        bool running;
        bool active;
        os::Mutex execution_lock;
};

}}


#endif
