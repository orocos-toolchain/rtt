 
#include <fdi/DigitalOutInterface.hpp>

namespace CBDeviceDriver
{

    using namespace ORO_CoreLib;

    /**
     * A DigitalOut represents any on/off output. Examples are
     * brakes, valves, simple grippers etc.
     */
    class DigitalOutput
    {
        public:
            /**
             * Create a new Relay acting on a digital output device.
             * 
             * @param dig_out The digital output device to use
             * @param relay_nr The bit number to use on <dig_out>
             */
            DigitalOutput( DigitalOutInterface* dig_out, unsigned int relay_nr )
                    : board( dig_out ), relaynumber( relay_nr )
            {}

            /**
             * Destruct a DigitalOutpuputt
             */
            virtual ~DigitalOutput()
            {}

            /**
             * Set the bit high of the digital output.
             */
            void switchOn()
            {
                board->switchOn( relaynumber );
            }

            /**
             * Set the bit low of the digital output.
             */
            void switchOff()
            {
                board->switchOff( relaynumber );
            }

            /**
             * Check if the output is on (high).
             *
             * @return true if the bit is high.
             */
            bool isOn()
            {
                return board->checkBit( relaynumber );
            }

        private:
            DigitalOutInterface *board;
            unsigned int relaynumber;
    };
};

