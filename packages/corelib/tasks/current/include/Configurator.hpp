
#ifndef CONFIGURATOR_HPP
#define CONFIGURATOR_HPP

#include "ConfigurationInterface.hpp"
#include "TaskNonRealTime.hpp"
#include "TaskPreemptible.hpp"
#include "TaskNonPreemptible.hpp"
#include "Time.hpp"

namespace ORO_CoreLib
{
    /**
     * Configures in a separate thread other components.
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
            HeartBeatGenerator::ticks timestamp = HeartBeatGenerator::Instance()->ticksGet();
            this->start();

            while ( this->isRunning() && ( _timeout == 0 ||
                                           HeartBeatGenerator::Instance()->secondsSince(timestamp) < _timeout ) )
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
