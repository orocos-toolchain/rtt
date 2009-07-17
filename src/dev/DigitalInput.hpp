/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  DigitalInput.hpp

                        DigitalInput.hpp -  description
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

#ifndef DIGITALINPUT_HPP
#define DIGITALINPUT_HPP

#include "DigitalInInterface.hpp"

namespace RTT
{
    /**
     * A class representing a switch which can be on or off.
     * This class can be used in combination with a DigitalInInterface
     * or as a 'virtual' switch in which case the on/off state is stored
     * in an external boolean variable supplied to the constructor.
     * @see also DigitalOutput
     * @ingroup DeviceInterface
     */
    class RTT_API DigitalInput
    {
    public:
        /**
         * Create an object to read the state of a switch which
         * can be on or off.
         *
         * @param dig_in The digital input device to use to read the status.
         * @param bit_nr The bit number to use on the device.
         * @param invert Set to true to return the inverted bit in isOn().
         */
        DigitalInput( DigitalInInterface* dig_in, unsigned int bit_nr, bool invert = false )
            :board(dig_in), bitnumber(bit_nr) , minvert(invert), mvalue(invert)
        {
        }

        /**
         * Create an object to read the state of a boolean value which
         * can be on or off.
         *
         * @param value A reference to a boolean representing a digital input
         * and which is used to read the status.
         * @param invert Set to true to return the inverted \a value in isOn().
         */
        DigitalInput( const bool& value, bool invert = false )
            :board(0), bitnumber(0), minvert(invert), mvalue(value)
        {
        }

        /**
         * Destruct the DigitalInput.
         */
        ~DigitalInput() {};

        /**
         * Status.
         *
         * @return true if the input is high.
         */
        bool isOn() const
        {
            // Q: who knew that != is logical xor ?
            return board ? minvert != board->isOn(bitnumber) : minvert != mvalue;
        }

    private:
        DigitalInInterface *board;
        int bitnumber;
        bool minvert;
        const bool& mvalue;
    };
}

#endif // DIGITALINPUT_HPP
