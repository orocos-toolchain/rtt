
#ifndef CONFIGURATOR_HPP
#define CONFIGURATOR_HPP

#include "ConfigurationInterface.hpp"
#include "TaskNonRealtime.hpp"
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
            :Base( period ), target(0)
        {
        }

        /**
         * Configures <_target>. Returns when the configuration
         * finished or terminated in error. So this is a blocking call,
         * but the Configurator itself is a non blocking task.
         *
         * @return true if the configuration finished, false otherwise.
         */
        bool configure( ConfigurationInterface* _target )
        {
            target = _target;
            this->start();

            while ( this->isRunning() )
                {
                    struct timespec tt = { 0, 1000*1000*10 };
                    nanosleep(&tt, 0);
                }
                    
            return target->isFinished();
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
            target->configCleanup();
        }

    protected:
        ConfigurationInterface* target;
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
