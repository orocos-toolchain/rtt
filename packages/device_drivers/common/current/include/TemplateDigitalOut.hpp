/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:51 CEST 2005  TemplateDigitalOut.hpp 

                        TemplateDigitalOut.hpp -  description
                           -------------------
    begin                : Sat May 21 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
 
#ifndef TEMPLATEDIGITALOUT_HPP
#define TEMPLATEDIGITALOUT_HPP

#include <rtt/os/CAS.hpp>

namespace RTT
{

	/**
     * @brief A thread-safe class to create a DigitalOutInterface
     * from 0 to 32 bits.
     *
     * The result of the methods is stored in a protected method
     * getBitStatus, which can be read by a derived class. The derived
     * class should also do bounds checking, if necessary and must modify
     * the bit status through the accessor methods such that thread-safeness
     * is guaranteed.
	 */
	class TemplateDigitalOut
    {
        /**
         * The status of all bits, with bit \a n being
         * the bit selected by (bit_status >> n) & 0x1.
         */
        volatile unsigned int bit_status;

    public:
        /**
         * The default constructor initialises the bit_status to zero.
         */
        TemplateDigitalOut()
            : bit_status(0)
        {
        }

        /**
         * The bit status is read-only.
         * @return The status of all bits, with bit \a n being
         * the bit selected by (bit_status >> n) & 0x1.
         */
        unsigned int getBitStatus() const { return bit_status; }

        void switchOn( unsigned int bit )
        {
            unsigned int orig;
            unsigned int bcopy;
            do {
                orig  = bit_status;
                bcopy = orig | (0x1 << bit);
            } while (!OS::CAS(&bit_status, orig, bcopy));
        }

        void switchOff( unsigned int bit )
        {
            unsigned int orig;
            unsigned int bcopy;
            do {
                orig = bit_status;
                bcopy = orig & ~(1 << bit);
            } while (!OS::CAS(&bit_status, orig, bcopy));
        }

        void setBit( unsigned int bit, bool value ) 
        {
            unsigned int orig;
            unsigned int bcopy;
            do {
                orig = bit_status;
                bcopy = orig & ~(!value << bit); // note the 'logical' not of value !
                bcopy = bcopy & (value << bit);
            } while (!OS::CAS(&bit_status, orig, bcopy));
        }

        void setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value)
        {
            unsigned int orig;
            unsigned int bcopy;
            unsigned int orcopy = value << start_bit;
            // XOR the bits in the zone with '1111' (selective negation)
            // zeros become one, then take the inverse, bitwise and with bcopy.
            unsigned int andcopy = ~(( ((( 1<< (stop_bit-start_bit+1))-1)) ^ value )<<start_bit);
            do {
                orig = bit_status;
                bcopy = orig | orcopy; // OR sets ones to 1
                bcopy = bcopy & andcopy; // AND sets zeros to 0
            } while (!OS::CAS(&bit_status, orig, bcopy));

//             for (unsigned int i=start_bit; i <= stop_bit; ++i)
//                 setBit(i, (value >> (i-start_bit) ) & 0x1 );
        }

        bool checkBit(unsigned int n) const
        {
            return (bit_status >> n) & 0x1;
        }

        unsigned int checkSequence( unsigned int start_bit, unsigned int stop_bit ) const
        {
            return (bit_status >> start_bit) & ( (0x1 << (stop_bit - start_bit + 1)) - 1);
        }
	};
			
}


#endif


