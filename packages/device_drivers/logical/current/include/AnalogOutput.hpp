/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:58 CEST 2004  AnalogOutput.hpp 

                        AnalogOutput.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
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
 
#ifndef ANALOGOUTPUT_HPP
#define ANALOGOUTPUT_HPP

#include <device_interface/AnalogOutInterface.hpp>

namespace ORO_DeviceDriver
{

    using namespace ORO_DeviceInterface;

    /**
     * A class representing an analog output channel.
     *
     * @param OutputType The type (int, unsigned int, double,...)
     * in which data is sent to the board.
     */
    template< class OutputType >
    class AnalogOutput
    {
    public:
        /**
         * Create an analog output object to read the state of a channel.
         *
         * @param ana_out     The analog output device to use to read the status.
         * @param channel_nr The channel number to use on the device.
         */
        AnalogOutput( AnalogOutInterface<OutputType>* ana_out, unsigned int channel_nr )
            :board(ana_out), channel(channel_nr)
        {
        }

        /**
         * Destruct the AnalogOutput.
         */
        ~AnalogOutput() {};

        /**
         * Write the value of this channel.
         */
        void value(double v)
        {
            if ( v < board->lowest(channel ) )
                d_cache = board->lowest( channel );
            else if ( v > board->highest( channel ) )
                d_cache = board->highest( channel ) ;
            else
                d_cache = v;
            i_cache = board->binaryLowest() + OutputType( ( d_cache - board->lowest(channel) ) * board->resolution(channel) );
            board->write(channel, i_cache);
        }

        /**
         * Write the raw value of this channel.
         */
        void rawValue(OutputType i)
        {
            if ( i < board->binaryLowest(channel ) )
                i_cache = board->binaryLowest( channel );
            else if ( i > board->binaryHighest( channel ) )
                i_cache = board->binaryHighest( channel ) ;
            else
                i_cache = i;
            board->write(channel, i);
        }

        /**
         * Read the value of this channel.
         */
        double value()
        {
             return d_cache;
        }

        /**
         * Read the raw value of this channel.
         */
        OutputType rawValue()
        {
            return i_cache;
        }

        /**
         * Return the highest output.
         */
        double highest()
        {
            return board->highest(channel);
        }

        /**
         * Return the lowest output.
         */
        double lowest()
        {
            return board->lowest(channel);
        }

    private:
        AnalogOutInterface<OutputType> *board;
        int channel;
        double d_cache;
        OutputType i_cache;
    };
}

#endif // ANALOGOUTPUT_HPP
