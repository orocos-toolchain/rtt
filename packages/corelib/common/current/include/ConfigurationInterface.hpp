#ifndef CONFIGURATIONINTERFACE_HPP
#define CONFIGURATIONINTERFACE_HPP

namespace ORO_CoreLib
{
    /**
     * @brief Generic run-time class configuration interface.
     *
     * An Interface for configuring an object through
     * an external object, iterating over the methods
     * of this interface.
     */
    class ConfigurationInterface
    {
        public:
        /**
         * Initialize and startup a configuration of
         * an object.
         */
        virtual void configInit() = 0;
            
        /**
         * A stepwise configuration of the object.
         * returns false if it fatally failed.
         */
        virtual bool configStep() = 0;

        /**
         * Returns if the configuration is finished.
         * The configuration is only finished if it was successful.
         */
        virtual bool isFinished() const = 0;

        /**
         * Perform some cleanup activities of the
         * configuration.
         */
        virtual void configCleanup() = 0;
    };




}

#endif
