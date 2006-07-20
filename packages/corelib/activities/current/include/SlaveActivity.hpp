/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:56 CEST 2006  SlaveActivity.hpp 

                        SlaveActivity.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
     * 'step' upon the invocation of 'trigger()', which is called by
     * another Activity.  The SlaveActivity can only be started if the
     * master is active or if no master is present. If a master is used, 
     * this activity takes periodicity over from the master. If no
     * master is present, use one of the alternative constructors to make
     * a periodic slave or a non periodic slave.
     *
     * \section TrigReact Reactions to trigger():
     * In the non periodic case, RunnableInterface::loop() is called, in the
     * periodic case, RunnableInterface::step() is called. In case the
     * RunnableInterface did not implement loop(), step() is invoked by default.
     * If no RunnableInterface is given, said functions are called upon
     * SlaveActivity itself.
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
         * Create an activity which is periodic.
         * @param period The periodicity at which you will \a trigger() this activity.
         * @param run Run this instance.
         */
        SlaveActivity( double period, RunnableInterface* run = 0 );

        /**
         * Create an activity for which \a trigger() will not be periodically called.
         * @param run Run this instance.
         */
        SlaveActivity( RunnableInterface* run = 0 );

        /**
         * Cleanup and notify the RunnableInterface that we are gone.
         */
        ~SlaveActivity();

        Seconds getPeriod() const;

        EventProcessor* getEventProcessor() const;

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

        bool run(RunnableInterface* _r);

        bool isPeriodic() const;

        bool isActive() const;

        /**
         * Call this function to 'trigger' the execution
         * of this Activity.
         * @return true when this->isActive(), false otherwise.
         */
        bool trigger();
    private:
        ActivityInterface* mmaster;
        double mperiod;
        RunnableInterface* runner;
        bool running;
        bool active;
};

}


#endif
