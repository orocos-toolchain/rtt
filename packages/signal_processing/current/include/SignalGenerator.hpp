#ifndef ORO_SIGNAL_PROC_SIGNALGENERATORINTERFACE_HPP
#define ORO_SIGNAL_PROC_SIGNALGENERATORINTERFACE_HPP


namespace ORO_SignalProcessing
{

    /**
     * Generates a signal.
     */
    class SignalGeneratorInterface
    {
    public:
        /**
         * Return the value of the signal on time t.
         */
        virtual double result( double t ) = 0;

        /**
         * Reset the generator.
         */
        virtual void reset() = 0
    };
}

#endif
