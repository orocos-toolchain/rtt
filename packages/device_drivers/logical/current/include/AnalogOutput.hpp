#ifndef ANALOGOUTPUT_HPP
#define ANALOGOUTPUT_HPP

#include <device_interface/AnalogOutInterface.hpp>

namespace ORO_DeviceDriver
{

    using namespace ORO_DeviceInterface;

    /**
     * A class representing an analog output channel.
     *
     * @param OutputType The type (int, unsigned int, double,...)
     * in which data is sent to the board.
     */
    template< class OutputType >
    class AnalogOutput
    {
    public:
        /**
         * Create an analog output object to read the state of a channel.
         *
         * this->value() = this->rawValue()*scale + offset;
         *
         * @param ana_out     The analog output device to use to read the status.
         * @param channel_nr The channel number to use on the device.
         * @param offset  The offset to be added to the converted value of the raw input.
         * @param scale   Conversion factor for the raw input value.
         */
        AnalogOutput( AnalogOutInterface<OutputType>* ana_out, unsigned int channel_nr, double _offset=0.0, double _scale=1.0)
            :board(ana_out), channel(channel_nr),
             offset(_offset), scale(_scale)
        {
        }

        /**
         * Destruct the AnalogOutput.
         */
        virtual ~AnalogOutput() {};

        /**
         * Write the value of this channel.
         */
        void value(double d)
        {
            d_cache = d;
            OutputType res = OutputType((d-offset)/scale);
            board->write(channel, res);
        }

        /**
         * Write the raw value of this channel.
         */
        void rawValue(OutputType i)
        {
            i_cache = i;
            board->write(channel, i);
        }

        /**
         * Read the value of this channel.
         */
        double value()
        {
            return d_cache;
        }

        /**
         * Read the raw value of this channel.
         */
        OutputType rawValue()
        {
            return i_cache;
        }

    private:
        AnalogOutInterface<OutputType> *board;
        int channel;
        double offset, scale;
        double d_cache;
        OutputType i_cache;
    };
};

#endif // ANALOGOUTPUT_HPP
