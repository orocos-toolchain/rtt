/***************************************************************************
  tag: Peter Soetens  Sat May 21 20:15:51 CEST 2005  TemplateDigitalIn.hpp 

                        TemplateDigitalIn.hpp -  description
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


