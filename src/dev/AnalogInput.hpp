/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:58 CEST 2004  AnalogInput.hpp 

                        AnalogInput.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
#ifndef ANALOGINPUT_HPP
#define ANALOGINPUT_HPP

#include "AnalogInInterface.hpp"

namespace RTT
{
    /**
     * A class representing an analog input channel.
     *
     * @ingroup DeviceInterface
     */
    class AnalogInput
    {
    public:
        /**
         * Create an analog input object to read the state of a channel.
         *
         * @param ana_in     The analog input device to use to read the status.
         * @param channel_nr The channel number to use on the device.
         */
        AnalogInput( AnalogInInterface* ana_in, unsigned int channel_nr )
            :board(ana_in), channel(channel_nr)
        {
        }

        /**
         * Destruct the AnalogInput.
         */
        ~AnalogInput() {};

        /**
         * Read the value of this channel.
         */
        double value() const
        {
            double r(0.0);
            board->read(channel, r);
            return r;
        }

        /**
         * Read the raw value of this channel.
         */
        int rawValue() const
        {
            unsigned int r(0);
            board->rawRead(channel, r);
            return r;
        }

    private:
        AnalogInInterface *board;
        int channel;
    };
}

#endif // ANALOGINPUT_HPP
