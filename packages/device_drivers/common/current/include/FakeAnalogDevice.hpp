/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  FakeAnalogDevice.hpp 

                        FakeAnalogDevice.hpp -  description
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
 
#ifndef FAKEANALOGDEVICE_HPP
#define FAKEANALOGDEVICE_HPP

#include "AnalogInInterface.hpp"
#include "AnalogOutInterface.hpp"

namespace RTT
{
    /**
     * A test class which replaces a real device driver.
     * It reproduces on the output what it gets on the input.
     */
    struct FakeAnalogDevice :
        public AnalogInInterface<unsigned int>,
        public AnalogOutInterface<unsigned int>
    {
        unsigned int nbofchans;
        unsigned int* mchannels;
        unsigned int mbin_range;
        double mlowest, mhighest;

        FakeAnalogDevice(unsigned int channels=32, unsigned int bin_range=4096, double lowest = -5.0, double highest = +5.0)
            : AnalogInInterface<unsigned int>("FakeAnalogDevice"),
              AnalogOutInterface<unsigned int>("FakeAnalogDevice"),
              nbofchans(channels),
              mchannels( new unsigned int[channels] ),
              mbin_range( bin_range),
              mlowest( lowest),
              mhighest( highest)
        {}

        ~FakeAnalogDevice() {
            delete[] mchannels;
        }

        virtual void rangeSet(unsigned int /*chan*/, 
                              unsigned int /*range*/) {}

        virtual void arefSet(unsigned int /*chan*/, 
                             unsigned int /*aref*/) {}

        virtual unsigned int nbOfChannels() const {
            return nbofchans;
        }

        virtual void read( unsigned int chan, unsigned int& value ) const 
        {
            if (chan < nbofchans)
                value = mchannels[chan];
        }

        virtual void write( unsigned int chan, unsigned int value ) {
            if (chan < nbofchans)
                mchannels[chan] = value;
        }

        virtual unsigned int binaryRange() const
        {
            return mbin_range;
        }

        virtual unsigned int binaryLowest() const 
        {
            return 0;
        }

        virtual unsigned int binaryHighest() const
        {
            return mbin_range;
        }

        virtual double lowest(unsigned int /*chan*/) const
        {
            return mlowest;
        }

        virtual double highest(unsigned int /*chan*/) const
        {
            return mhighest;
        }

        virtual double resolution(unsigned int /*chan*/) const
        {
            return mbin_range/(mhighest-mlowest);
        }

    };    

}

#endif
