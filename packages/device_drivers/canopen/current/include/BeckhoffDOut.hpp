/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  BeckhoffDOut.hpp 

                        BeckhoffDOut.hpp -  description
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
 
#ifndef BECKHOFFDOUT_HPP
#define BECKHOFFDOUT_HPP

#include "CAN.hpp"
#include <rtt/dev/DigitalOutInterface.hpp>

namespace CAN
{
	/**
     * @brief A class representing an Beckhoff Digital Out Terminal.
	 */
	class BeckhoffDOut
        : public DigitalOutInterface
    {
        public:
        /**
         */
        BeckhoffDOut()
            : bit_status(0), totalBits(0)
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
            bit_status &= ~(1 << bit);
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
            return totalBits;
        }

        
    protected:

        unsigned int bit_status;

        unsigned int totalBits;
	};
			
}


#endif


