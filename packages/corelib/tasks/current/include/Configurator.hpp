/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  Configurator.hpp 

                        Configurator.hpp -  description
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
 

#ifndef CONFIGURATOR_HPP
#define CONFIGURATOR_HPP

#include "ConfigurationInterface.hpp"
#include "TaskNonRealTime.hpp"
#include "TaskPreemptible.hpp"
#include "TaskNonPreemptible.hpp"
#include "TimeService.hpp"
#include "Time.hpp"

namespace ORO_CoreLib
{
    /**
     * @brief A task which configures components.
     *
     * This is a step-wise configurator which instructs
     * another class to do a configuration 'step'. After
     * the specified period, it instructs to do the next
     * step and so on until an error occured or the configuration
     * is finished.
     *
     * @param TaskBase Template : The Task formalism to use.
     */
    template< class TaskBase >
    class Configurator 
        : protected TaskBase
    {
    public:
        typedef TaskBase Base;
        
        /**
         * Creates a Configurator with configuration
         * period of <period>.
         *
         * @param period The period between each configuration step.
         */
        Configurator( double period )
            :Base( period ), target(0), status (false)
        {
        }

        /**
         * Configures <_target>. Returns when the configuration
         * finished or terminated in error. So this is a blocking call,
         * but the Configurator itself is a non blocking task.
         *
         * @return true if the configuration finished, false otherwise.
         */
        bool configure( ConfigurationInterface* _target, Seconds _timeout = 0 )
        {
            target = _target;
            TimeService::ticks timestamp = TimeService::Instance()->getTicks();
            this->start();

            while ( this->isRunning() && ( _timeout == 0 ||
                                           TimeService::Instance()->secondsSince(timestamp) < _timeout ) )
                {
                    struct timespec tt;
                    tt.tv_nsec=100*1000*1000; // 0.1 Hz
                    tt.tv_sec =0;
                    nanosleep(&tt, 0);
                }

            this->stop();

            return status;
        }

        /**
         * Returns the current configuration target.
         */
        ConfigurationInterface* configTarget() const
        {
            return target;
        }

        bool initialize()
        {
            target->configInit();
            return true;
        }

        void step()
        {
            if ( !target->configStep() || target->isFinished() )
                this->stop();
        }

        void finalize()
        {
            status = target->isFinished();
            target->configCleanup();
        }

    protected:
        ConfigurationInterface* target;

        /**
         * Flag to save status of the target.
         */
        bool status;
    };

    /**
     * A NonPreemptible Configurator.
     */
    typedef Configurator<TaskNonPreemptible> NonPreemptibleConfigurator;
    /**
     * A Preemptible Configurator.
     */
    typedef Configurator<TaskPreemptible> PreemptibleConfigurator;
    /**
     * A NonRealTime Configurator.
     */
    typedef Configurator<TaskNonRealTime> NonRealTimeConfigurator;
}


#endif
