/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  DigitalOutput.hpp

                        DigitalOutput.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
#ifndef DIGITAL_OUTPUT_HPP
#define DIGITAL_OUTPUT_HPP


#include "DigitalOutInterface.hpp"

namespace RTT
{ namespace dev {

    /**
     * A DigitalOut represents any on/off output. Examples are
     * brakes, valves, simple grippers etc.
     * This class can be used in combination with a DigitalOutInterface
     * or as a 'virtual' switch in which case the on/off state is stored
     * in this object.
     * @see also DigitalInput
     * @ingroup DeviceInterface
     */
    class RTT_API DigitalOutput
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
         * @param initial_state The initial on/off state of the DigitalOutput.
         */
        DigitalOutput ( bool initial_state = false )
            : board( 0 ), relaynumber( 0 ), invert(initial_state)
        {}


        /**
         * Destruct a DigitalOutput
         */
        ~DigitalOutput()
        {}

        /**
         * Set the bit to the on or off state.
         * @param on_off Set to \a true to turn the bit on.
         */
        void setBit( bool on_off )
        {
            if(board)
                board->setBit( relaynumber, on_off != invert );
            else
                invert=on_off;
        }

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
}}

#endif
