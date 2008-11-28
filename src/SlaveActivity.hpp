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

#include "ActivityInterface.hpp"
#include "RunnableInterface.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{

    /**
     * @brief An ActivityInterface implementation which executes
     * 'step' upon the invocation of 'execute()', which is called by
     * another Activity.  The SlaveActivity can only be started if the
     * master is active or if no master is present. If a master is used,
     * this activity takes periodicity over from the master. If no
     * master is present, use one of the alternative constructors to make
     * a periodic slave or a non periodic slave.
     *
     * \section ExecReact Reactions to execute():
     * In the non periodic case, RunnableInterface::loop() is called, in the
     * periodic case, RunnableInterface::step() is called. In case the
     * RunnableInterface did not implement loop(), step() is invoked by default.
     * If no RunnableInterface is given, said functions are called upon
     * SlaveActivity itself.
     *
     * \section TrigReact Reactions to trigger():
     * In the non periodic
     * case, trigger() is called upon the master (causing it to
     * execute), in the periodic case, it is ignored (you can not
     * trigger periodic activities).
     * @ingroup CoreLibActivities
     */
    class SlaveActivity
        :public ActivityInterface
    {
    public:
        /**
         * Create an activity which is the slave of \a master.
         * @param master The activity which will execute this activity.
         * @param run Run this instance.
         */
        SlaveActivity( ActivityInterface* master, RunnableInterface* run = 0 );

        /**
         * Create an activity which is periodic. If \a period == 0.0, this constructor
         * is equivalent to the one below for non periodic activities.
         * @param period The periodicity at which you will \a trigger() this activity.
         * @param run Run this instance.
         */
        SlaveActivity( double period, RunnableInterface* run = 0 );

        /**
         * Create an activity for which \a trigger() will not be periodically called.
         * The period of this activity will be 0.0.
         * @param run Run this instance.
         */
        SlaveActivity( RunnableInterface* run = 0 );

        /**
         * Cleanup and notify the RunnableInterface that we are gone.
         */
        ~SlaveActivity();

        Seconds getPeriod() const;

        OS::ThreadInterface* thread();

        bool initialize();
        void step();
        void loop();
        bool breakLoop();
        void finalize();

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
    private:
        ActivityInterface* mmaster;
        double mperiod;
        bool running;
        bool active;
};

}


#endif
