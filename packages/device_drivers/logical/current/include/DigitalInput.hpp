/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  DigitalInput.hpp 

                        DigitalInput.hpp -  description
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
 
#ifndef DIGITALINPUT_HPP
#define DIGITALINPUT_HPP

#include <device_interface/DigitalInInterface.hpp>

namespace ORO_DeviceDriver
{

    using namespace ORO_DeviceInterface;

    /**
     * A class representing an on/off switch, derived
     * from a digital input.
     */
    class DigitalInput
    {
        public:
            /**
             * Create a switch object to read the state of a switch which
             * can be on or off.
             *
             * @param dig_in The digital input device to use to read the status.
             * @param bit_nr The bit number to use on the device.
             */
            DigitalInput( DigitalInInterface* dig_in, unsigned int bit_nr, bool _invert = false )
                :board(dig_in), bitnumber(bit_nr) , invert(_invert)
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
                return invert != board->isOn(bitnumber);
            }

        private:
        DigitalInInterface *board;
        int bitnumber;
        bool invert;
    };
};

#endif // DIGITALINPUT_HPP
