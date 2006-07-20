/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  ComediSubDeviceAOut.hpp 

                        ComediSubDeviceAOut.hpp -  description
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
   stuff.
*/

#ifndef COMEDISUBDEVICEAOUT_HPP
#define COMEDISUBDEVICEAOUT_HPP

#include "AnalogOutInterface.hpp"
#include "ComediDevice.hpp"

namespace RTT
{

  /**
   * This logical device represents one subdevice of a Comedi device.
   */
  class ComediSubDeviceAOut
    : public AnalogOutInterface<unsigned int>
  {

  public:
    /**
     * Create a new ComediSubDevice with a given ComediDevice, subdevice number and
     * a name for this sub device
     *
     * @param cao The ComediDevice to use for output
     * @param name The name of this instance
     */
    ComediSubDeviceAOut( ComediDevice* cao, const std::string& name, 
			 unsigned int subdevice=1 );

    ComediSubDeviceAOut( ComediDevice* cao, unsigned int subdevice=1 );

    ~ComediSubDeviceAOut();

    void init();

    virtual void rangeSet(unsigned int chan, unsigned int range=0);

    virtual void arefSet(unsigned int chan, unsigned int aref=AnalogOutInterface<unsigned int>::Ground);

    virtual void write( unsigned int chan, unsigned int value );

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
     * The subdevice number of this instance in \a myDrive
     */
    unsigned int _subDevice;
    unsigned int * _sd_range;
    unsigned int * _aref;
    unsigned int channels;
  };

};

#endif
