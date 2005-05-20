#ifndef TEMPLATEDIGITALIN_HPP
#define TEMPLATEDIGITALIN_HPP

#include <device_interface/DigitalInInterface.hpp>

namespace ORO_DeviceDriver
{
	/**
     * @brief A utility class to create a DigitalInInterface
     * from 0 to 32 bits.
     *
     * The result of the methods is stored in a protected variable
     * bit_status, which can be used by a derived class.
     * @see DigitalInInterface
	 */
	class TemplateDigitalIn
    {
        public:
        /**
         * The default constructor initialises the bit_status to zero.
         */
        TemplateDigitalIn()
            : bit_status(0)
        {
        }
            
        bool isOn( unsigned int bit ) const
        { return bit_status >> bit & 0x1; }

        bool isOff( unsigned int bit ) const
        { return !isOn(bit); }

        bool readBit( unsigned int bit ) const
        { return isOn(bit); }

        unsigned int readSequence(unsigned int start_bit, unsigned int stop_bit) const
        { 
            unsigned int result = bit_status >> start_bit;
            return result & ( 1 << (stop_bit-start_bit+1) ) - 1;
        }

    protected:
        /**
         * The status of all bits, with bit \a n being
         * the bit selected by (bit_status >> n) & 0x1.
         */
        unsigned int bit_status;
	};
			
}


#endif


