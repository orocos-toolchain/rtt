#ifndef ANALOGINPUT_HPP
#define ANALOGINPUT_HPP

#include <device_interface/AnalogInInterface.hpp>

namespace ORO_DeviceDriver
{

    using namespace ORO_DeviceInterface;

    /**
     * A class representing an analog input channel.
     *
     * @param InputType The type (int, unsigned int, double,...)
     * in which data is sent to the board.
     */
    template< class InputType >
    class AnalogInput
    {
    public:
        /**
         * Create an analog input object to read the state of a channel.
         *
         * @param ana_in     The analog input device to use to read the status.
         * @param channel_nr The channel number to use on the device.
         */
        AnalogInput( AnalogInInterface<InputType>* ana_in, unsigned int channel_nr )
            :board(ana_in), channel(channel_nr)
        {
        }

        /**
         * Destruct the AnalogInput.
         */
        ~AnalogInput() {};

        /**
         * Read the value of this channel.
         */
        double value() const
        {
            InputType r;
            board->read(channel, r);
            return ( r - board->binaryLowest() ) / board->resolution(channel) + board->lowest(channel);
        }

        /**
         * Read the raw value of this channel.
         */
        InputType rawValue() const
        {
            InputType r;
            board->read(channel, r);
            return r;
        }

    private:
        AnalogInInterface<InputType> *board;
        int channel;
    };
}

#endif // ANALOGINPUT_HPP
