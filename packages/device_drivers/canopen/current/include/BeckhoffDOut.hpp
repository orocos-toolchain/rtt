#ifndef BECKHOFFDOUT_HPP
#define BECKHOFFDOUT_HPP

#include <device_interface/DigitalOutInterface.hpp>
#include "BeckhoffCANCoupler.hpp"

namespace Beckhoff
{
    using namespace ORO_CoreLib;
    using ORO_DeviceInterface::DigitalOutInterface;

	/**
     * A class representing an Beckhoff Digital Out Terminal.
     * TODO : set all bits to zero in init
	 */
	class BeckhoffDOut
        : public DigitalOutInterface
    {
        public:
        /**
         */
        BeckhoffDOut()
            : bit_status(), totalBits(0)
        {
        }

        void addTerminal( unsigned int nr, unsigned int bits)
        {
            totalBits += bits;
        }

        void switchOn( unsigned int bit )
        {
            bit_status |= (0x1 << bit);
        }

        void switchOff( unsigned int bit )
        {
            bit_status &= !(0x1 << bit);
        }

        virtual void setBit( unsigned int bit, bool value ) 
        {
            if (value)
                switchOn(bit);
            else
                switchOff(bit);
        }

        virtual void setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value)
        {
            for (unsigned int i=start_bit; i <= stop_bit; ++i)
                setBit(i, (value >> (i-start_bit) ) & 0x1 );
        }

        virtual bool checkBit(unsigned int n) const
        {
            return (bit_status >> n) & 0x1;
        }

        virtual unsigned int checkSequence( unsigned int start_bit, unsigned int stop_bit ) const
        {
            return (bit_status >> start_bit) & ( (0x1 << (stop_bit - start_bit + 1)) - 1);
        }
                                                
        unsigned int nbOfOutputs() const
        {
            return 32;
        }

        
    protected:

        unsigned int bit_status;

        unsigned int totalBits;
	};
			
}


#endif


