/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  BeckhoffDIn.hpp 

                        BeckhoffDIn.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
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
            : coupler(_coupler), bit_status(0), totalInputs(0)
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


