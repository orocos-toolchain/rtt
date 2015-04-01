/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  SlaveActivity.hpp

                        SlaveActivity.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

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


#ifndef ORO_SLAVE_ACTIVITY_HPP
#define ORO_SLAVE_ACTIVITY_HPP

#include "../base/ActivityInterface.hpp"
#include "../base/RunnableInterface.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{ namespace extras {


    /**
     * @brief An base::ActivityInterface implementation which executes
     * 'step' upon the invocation of 'execute()', which is called by
     * another Activity ('master').  The SlaveActivity can only be started if the
     * master is active or if no master is present. If a master is used,
     * this activity takes periodicity over from the master. If no
     * master is present, use one of the alternative constructors to make
     * a periodic slave or a non periodic slave.
     *
     * Any activity object can be a master of a SlaveActivity,
     * including itself.  A master needs to keep track of its slave
     * itself. There is no standard mechanism in RTT in which masters
     * execute slaves. You need to code this yourself in your master
     * activity by calling execute() on each Slave.
     *
     * The only thing a slave does/can do is
     * * be started and stopped independently.
     * * notifying its master that it was triggered (if present)
     * * copying the periodic/thread properties of its master (if present)
     *
     * \section ExecReact Reactions to execute():
     *
     * In the non periodic case, base::RunnableInterface::loop() is called, in the
     * periodic case, base::RunnableInterface::step() is called. In case the
     * base::RunnableInterface did not implement loop(), step() is invoked by default.
     * If no base::RunnableInterface is given, said functions are called upon
     * SlaveActivity itself.
     *
     * \section TrigReact Reactions to trigger():
     *
     * trigger() is ignored and returns false when no master is present.
     *
     * When there is a master: In the non periodic case, trigger() is
     * called upon the master (causing it to execute), in the periodic
     * case, it is ignored (you can not trigger periodic activities).
     * @ingroup CoreLibActivities
     */
    class RTT_API SlaveActivity
        :public base::ActivityInterface
    {
    public:
        /**
         * Create an activity which is the slave of \a master.
         * @param master The activity which will execute this activity.
         * @param run Run this instance.
         */
        SlaveActivity( base::ActivityInterface* master, base::RunnableInterface* run = 0 );

        /**
         * Create an activity which is periodic. If \a period == 0.0, this constructor
         * is equivalent to the one below for non periodic activities.
         * @param period The periodicity at which you will \a trigger() this activity.
         * @param run Run this instance.
         */
        SlaveActivity( double period, base::RunnableInterface* run = 0 );

        /**
         * Create an activity for which \a trigger() will not be periodically called.
         * The period of this activity will be 0.0.
         * @param run Run this instance.
         */
        SlaveActivity( base::RunnableInterface* run = 0 );

        /**
         * Cleanup and notify the base::RunnableInterface that we are gone.
         */
        ~SlaveActivity();

        Seconds getPeriod() const;

        bool setPeriod(Seconds s);

        unsigned getCpuAffinity() const;

        bool setCpuAffinity(unsigned cpu);

        os::ThreadInterface* thread();

        base::ActivityInterface *getMaster() const;

        virtual bool initialize();
        virtual void step();
        virtual void work(base::RunnableInterface::WorkReason reason);
        virtual void loop();
        virtual bool breakLoop();
        virtual void finalize();

        /**
         * Start this slave. Only allowed if the master activity is already running.
         * @retval false if !master->isRunning() || this->isRunning() || initialize() returns false
         * @retval true othwerise.
         */
        bool start();

        bool stop();

        bool isRunning() const;

        bool isPeriodic() const;

        bool isActive() const;

        bool execute();

        bool trigger();

        bool timeout();
    private:
        base::ActivityInterface* mmaster;
        double mperiod;
        bool running;
        bool active;
};

}}


#endif
