/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  ComediSubDeviceAIn.hpp 

                        ComediSubDeviceAIn.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
/* Klaas Gadeyne August 2003: implemented some non/badly implemented
   stuff.  Added comedilib.h include header
   Added range information.  
   Klaas Gadeyne November 2003: Not foolproof yet!  Should work with
   new build system for gnulinux and lxrt
*/ 

#ifndef COMEDISUBDEVICEAIN_HPP
#define COMEDISUBDEVICEAIN_HPP

#include "AnalogInInterface.hpp"
#include "ComediDevice.hpp"

namespace RTT
{

  /**
   * This logical device represents one subdevice of a Comedi device.
   */
  class ComediSubDeviceAIn
    : public AnalogInInterface<unsigned int>
  {

  public:
    /**
     * Create a new ComediSubDeviceAIn with a given ComediDevice, subdevice number and
     * a name for this sub device
     *
     * @param cd The ComediDevice to use for output
     * @param subdevice The subdevice number for this comedi device (usually 1)
     * @param name The name of this instance
     */
    ComediSubDeviceAIn( ComediDevice* cd, const std::string& name, unsigned int subdevice=0);

    ComediSubDeviceAIn( ComediDevice* cd, unsigned int subdevice=0 );

    ~ComediSubDeviceAIn();

    void init();

    virtual void rangeSet(unsigned int chan, unsigned int range=0);

    virtual void arefSet(unsigned int chan, unsigned int aref=AnalogInInterface<unsigned int>::Ground);

    virtual void read( unsigned int chan, unsigned int& value ) const;

    virtual unsigned int binaryRange() const;

    virtual unsigned int binaryLowest() const;

    virtual unsigned int binaryHighest() const;
        
    virtual double lowest(unsigned int chan) const;

    virtual double highest(unsigned int chan) const;

    virtual double resolution(unsigned int chan) const;

    virtual unsigned int nbOfChannels() const;

  protected:
    /**
     * The output device to write to
     */
    ComediDevice* myCard;

    /**
     * The subdevice number of this instance in \a myCard
     */
    unsigned int _subDevice;
    unsigned int * _sd_range;
    unsigned int * _aref;
    unsigned int channels;
  };

};

#endif
