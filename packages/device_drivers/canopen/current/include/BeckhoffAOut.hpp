/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  BeckhoffAOut.hpp 

                        BeckhoffAOut.hpp -  description
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
 
#ifndef BECKHOFFAOUT_HPP
#define BECKHOFFAOUT_HPP

#include <device_interface/AnalogOutInterface.hpp>

namespace Beckhoff
{
    using ORO_DeviceInterface::AnalogOutInterface;
    /**
     * A BeckHoff terminal for 4 analog outputs.
     */
    class BeckhoffAOut
        : public AnalogOutInterface<unsigned int>
    {
    public:
        BeckhoffAOut()
        {
            channels[0] = 0;
            channels[1] = 0;
            channels[2] = 0;
            channels[3] = 0;
        }

        virtual void rangeSet( unsigned int chan, unsigned int range)
        {}

	    virtual void arefSet(unsigned int chan,  unsigned int aref)
        {}

        virtual void write( unsigned int chan, unsigned int value )
        {
            if (chan <0 || chan >3 ) return;
            channels[chan] = value;
        }    

        virtual unsigned int binaryRange() const { return 0xFFFF; }

        virtual unsigned int binaryZero()  const { return 0x0; }

        virtual unsigned int binaryLowest() const  { return 0x8001;}

        virtual unsigned int binaryHighest() const { return 0x7FFF; }

        virtual double lowest(unsigned int chan) const { return -10.0; }

        virtual double highest(unsigned int chan) const { return 10.0; }

        // in bits per MU
        virtual double resolution(unsigned int chan) const 
        { return binaryRange() / 20.; }
             
        /**
         * Returns the value of a channel <chan>.
         */
        unsigned int value(unsigned int chan)
        {
            if (chan <0 || chan >3 ) return ~0;
            return channels[chan];
        }

        virtual unsigned int nbOfChannels() const { return 4; }
                    
        void addTerminal(unsigned int nr, unsigned int channels) 
        {}
    protected:
        unsigned int channels[4];
    };






}


#endif
