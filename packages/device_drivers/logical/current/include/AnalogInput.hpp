/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:58 CEST 2004  AnalogInput.hpp 

                        AnalogInput.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ANALOGINPUT_HPP
#define ANALOGINPUT_HPP

#include <device_interface/AnalogInInterface.hpp>

namespace ORO_DeviceDriver
{

    using namespace ORO_DeviceInterface;

    /**
     * A class representing an analog input channel.
     *
     * @param InputType The type (int, unsigned int, double,...)
     * in which data is sent to the board.
     */
    template< class InputType >
    class AnalogInput
    {
    public:
        /**
         * Create an analog input object to read the state of a channel.
         *
         * @param ana_in     The analog input device to use to read the status.
         * @param channel_nr The channel number to use on the device.
         */
        AnalogInput( AnalogInInterface<InputType>* ana_in, unsigned int channel_nr )
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
            InputType r;
            board->read(channel, r);
            return ( r - board->binaryLowest() ) / board->resolution(channel) + board->lowest(channel);
        }

        /**
         * Read the raw value of this channel.
         */
        InputType rawValue() const
        {
            InputType r;
            board->read(channel, r);
            return r;
        }

    private:
        AnalogInInterface<InputType> *board;
        int channel;
    };
}

#endif // ANALOGINPUT_HPP
