#ifndef DIGITALINPUT_HPP
#define DIGITALINPUT_HPP

#include <fdi/DigitalInInterface.hpp>

namespace CBDeviceDriver
{

    using namespace ORO_CoreLib;

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
            DigitalInput( DigitalInInterface* dig_in, unsigned int bit_nr )
                :board(dig_in), bitnumber(bit_nr) 
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
                return board->isOn(bitnumber);
            }

        private:
            DigitalInInterface *board;
            int bitnumber;
    };
};

#endif // DIGITALINPUT_HPP
