/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  DigitalOutput.hpp 

                        DigitalOutput.hpp -  description
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
#ifndef DIGITAL_OUTPUT_HPP
#define DIGITAL_OUTPUT_HPP
 
 
#include <device_interface/DigitalOutInterface.hpp>

namespace ORO_DeviceDriver
{

    using namespace ORO_DeviceInterface;

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
             * @param relay_nr The bit number to use on \a dig_out
	     * @param _invert Set to true if \a isOn() must return inverted signal
             */
            DigitalOutput( DigitalOutInterface* dig_out, unsigned int relay_nr, bool _invert=false )
                : board( dig_out ), relaynumber( relay_nr ), invert(_invert)
            {}
      /**
       * Create a virtual (software) relay
       * 
       */
      DigitalOutput ( )
                : board( 0 ), relaynumber( 0 ), invert(false)
      {}
      
      

            /**
             * Destruct a DigitalOutpuputt
             */
            ~DigitalOutput()
            {}

            /**
             * Set the bit high of the digital output.
             */
            void switchOn()
            {
	      if(board)
                board->setBit( relaynumber, !invert );
	      else
		invert=true;
	    }

            /**
             * Set the bit low of the digital output.
             */
            void switchOff()
            {
	      if(board)
                board->setBit( relaynumber, invert );
	      else
		invert=false;
	    }

            /**
             * Check if the output is on (high).
             *
             * @return true if the bit is high.
             */
            bool isOn() const
            {
	      if(board)
                return invert != board->checkBit( relaynumber );
	      else
		return invert;
	    }

        private:
        DigitalOutInterface *board;
        unsigned int relaynumber;
        bool invert;
    };
};

#endif
