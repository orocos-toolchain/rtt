#ifndef BECKHOFFDIN_HPP
#define BECKHOFFDIN_HPP

#include <device_interface/DigitalInInterface.hpp>
#include "CANDeviceInterface.hpp"
#include "CANMessage.hpp"

namespace Beckhoff
{
    using namespace ORO_CoreLib;
    using namespace CAN;
    using ORO_DeviceInterface::DigitalInInterface;

	/**
     * A class representing an Beckhoff Digital Input Terminal.
     * TODO : set all bits to zero in init
	 */
	class BeckhoffDIn
        : public DigitalInInterface
    {
        public:
        /**
         */
        BeckhoffDIn(CANDeviceInterface* _coupler)
            : coupler(_coupler), bit_status(), totalInputs(0)
        {
        }
        
        void addTerminal(unsigned int nr, unsigned int bits)
        {
            totalInputs += bits;
        }
        
        void update(const CANMessage* cm)
        {
            bit_status = 0;
            for (unsigned int i=0; i < cm->getDLC(); ++i)
                bit_status += cm->getData(i) << 8*i;
        }
            
        virtual bool isOn( unsigned int bit = 0) const
        { return bit_status >> bit & 0x1; }

        virtual bool isOff( unsigned int bit = 0) const
        { return !isOn(bit); }

        virtual bool readBit( unsigned int bit = 0) const
        { return isOn(bit); }

        virtual unsigned int readSequence(unsigned int start_bit, unsigned int stop_bit) const
        { 
            unsigned int result = bit_status >> start_bit;
            return result & ( 1 << (stop_bit-start_bit+1) ) - 1;
        }

        virtual unsigned int nbOfInputs() const
        { return totalInputs; }
        
    protected:
        CANDeviceInterface* coupler;
        unsigned int bit_status;
        unsigned int totalInputs;

	};
			
}


#endif


