/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  Configurator.hpp

                        Configurator.hpp -  description
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


#ifndef ORO_CONFIGURATOR_HPP
#define ORO_CONFIGURATOR_HPP

#include "ConfigurationInterface.hpp"
#include "PeriodicActivity.hpp"
#include "../os/TimeService.hpp"
#include "../Time.hpp"

namespace RTT
{
    /**
     * @brief A class which configures other objects in small steps
     * spread in time.
     *
     * This is a step-wise configurator which instructs
     * another class to do a configuration 'step'. After
     * the specified period, it instructs to do the next
     * step and so on until an error occured or the configuration
     * is finished.
     *
     * @see ConfigurationInterface
     */
    class RTT_API Configurator
        : protected PeriodicActivity
    {
    public:
        /**
         * Creates a Configurator with configuration
         * period of \a period.
         *
         * @param period The period between each configuration step.
         */
        Configurator( double period )
            :PeriodicActivity(RTT::OS::LowestPriority, period ), mtarget(0), status (false)
        {
        }

        /**
         * Configures \a target. Returns when the configuration
         * finished or terminated in error. So this is a blocking call,
         * but the Configurator itself is a non blocking task.
         *
         * @return true if the configuration finished, false otherwise.
         */
        bool configure( ConfigurationInterface* target, Seconds _timeout = 0 )
        {
            mtarget = target;
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
            return mtarget;
        }

        bool initialize()
        {
            mtarget->configInit();
            return true;
        }

        void step()
        {
            if ( !mtarget->configStep() || mtarget->isFinished() )
                this->stop();
        }

        void finalize()
        {
            status = mtarget->isFinished();
            mtarget->configCleanup();
        }

    protected:
        ConfigurationInterface* mtarget;

        /**
         * Flag to save status of the target.
         */
        bool status;
    };

}


#endif
