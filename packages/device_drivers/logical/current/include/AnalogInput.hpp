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
         * this->value() = this->rawValue()*scale + offset;
         *
         * @param ana_in     The analog input device to use to read the status.
         * @param channel_nr The channel number to use on the device.
         * @param offset  The offset to be added to the converted value of the raw input.
         * @param scale   Conversion factor for the raw input value.
         */
        AnalogInput( AnalogInInterface<InputType>* ana_in, unsigned int channel_nr, double _offset=0.0, double _scale=1.0)
            :board(ana_in), channel(channel_nr),
             offset(_offset), scale(_scale)
        {
        }

        /**
         * Destruct the AnalogInput.
         */
        virtual ~AnalogInput() {};

        /**
         * Read the value of this channel.
         */
        double value()
        {
            InputType d;
            board->read(channel, d);
            return d*scale + offset;
        }

        /**
         * Read the raw value of this channel.
         */
        InputType rawValue()
        {
            InputType i;
            board->read(channel, i);
            return i;
        }

    private:
        AnalogInInterface<InputType> *board;
        int channel;
        double offset, scale;
    };
};

#endif // ANALOGINPUT_HPP
