/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:58 CEST 2004  FakeDigitalDevice.hpp 

                        FakeDigitalDevice.hpp -  description
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
 
#ifndef FAKEDIGITALDEVICE_HPP
#define FAKEDIGITALDEVICE_HPP

#include "dev/DigitalInInterface.hpp"
#include "dev/DigitalOutInterface.hpp"
#include <vector>

namespace RTT
{
    /**
     * A Fake (Simulated) Digital Input/Output Device which replicates the inputs
     * on its outputs.
     */
    class FakeDigitalDevice
        : public DigitalInInterface,
          public DigitalOutInterface
    {
    public:
        std::vector<bool> mchannels;

        FakeDigitalDevice(unsigned int channels=32)
            : DigitalInInterface("FakeDigitalDevice"),
              DigitalOutInterface("FakeDigitalDevice"),
              mchannels(channels, false)
        {}
        
        virtual void switchOn( unsigned int n )
        {
            if ( n < mchannels.size() ) 
                mchannels[n] = true;
        }

        virtual void switchOff( unsigned int n )
        {
            if ( n < mchannels.size() ) 
                mchannels[n] = false;
        }

        virtual void setBit( unsigned int bit, bool value )
        {
            if ( bit < mchannels.size() ) 
                mchannels[bit] = value;
        }
                
        virtual void setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value)
        {
            if ( start_bit < mchannels.size() && stop_bit < mchannels.size() ) 
                for (unsigned int i = start_bit; i <= stop_bit; ++i)
                    mchannels[i] = value & ( 1<<( i - start_bit ) );
        }

        virtual bool checkBit(unsigned int n) const
        {
            if ( n < mchannels.size() ) 
                return mchannels[n];
            return false;
        }


        virtual unsigned int checkSequence( unsigned int start_bit, unsigned int stop_bit ) const
        {
            unsigned int result = 0;
            if ( start_bit < mchannels.size() && stop_bit < mchannels.size() ) 
                for (unsigned int i = start_bit; i <= stop_bit; ++i)
                    result += (mchannels[i] & 1)<<i;
            return result;
        }
            
        virtual unsigned int nbOfOutputs() const
        {
            return mchannels.size();
        }

        virtual unsigned int nbOfInputs() const
        {
            return mchannels.size();
        }

        virtual bool isOn( unsigned int bit = 0) const 
        {
            if ( bit < mchannels.size() ) 
                return mchannels[bit];
            return false;
        }

        virtual bool isOff( unsigned int bit = 0) const
        {
            if ( bit < mchannels.size() ) 
                return !mchannels[bit];
            return true;
        }
            
        virtual bool readBit( unsigned int bit = 0) const
        {
            if ( bit < mchannels.size() ) 
                return mchannels[bit];
            return false;
        }

        virtual unsigned int readSequence(unsigned int start_bit, unsigned int stop_bit) const
        {
            if ( start_bit < mchannels.size() && stop_bit < mchannels.size() ) 
                return checkSequence(start_bit, stop_bit);
            return 0;
        }
            
    };


}


#endif
