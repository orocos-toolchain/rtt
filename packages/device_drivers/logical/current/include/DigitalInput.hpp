#ifndef DIGITALINPUT_HPP
#define DIGITALINPUT_HPP

#include <device_interface/DigitalInInterface.hpp>

namespace CBDeviceDriver
{

    using namespace ORO_DeviceInterface;

    /**
     * A class representing an on/off switch, derived
     * from a digital input.
     */
    class DigitalInput
    {
        public:
            /**
             * Create a switch object to read the state of a switch which
             * can be on or off.
             *
             * @param dig_in The digital input device to use to read the status.
             * @param bit_nr The bit number to use on the device.
             */
            DigitalInput( DigitalInInterface* dig_in, unsigned int bit_nr, bool _invert )
                :board(dig_in), bitnumber(bit_nr) , invert(_invert)
            {
            }

            /**
             * Destruct the DigitalInput.
             */
            virtual ~DigitalInput() {};

            /**
             * Status.
             *
             * @return true if the input is high.
             */
            bool isOn()
            {
                return invert ^ board->isOn(bitnumber);
            }

        private:
        DigitalInInterface *board;
        int bitnumber;
        bool invert;
    };
};

#endif // DIGITALINPUT_HPP
