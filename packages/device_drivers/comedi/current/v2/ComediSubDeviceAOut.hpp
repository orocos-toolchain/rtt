/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  ComediSubDeviceAOut.hpp 

                        ComediSubDeviceAOut.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

    ***************************************************************************
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    *                                                                         *
    ***************************************************************************/
/* Klaas Gadeyne August 2003: implemented some non/badly implemented
   stuff.
*/

#ifndef COMEDISUBDEVICEAOUT_HPP
#define COMEDISUBDEVICEAOUT_HPP

#include <fdi/AnalogOutInterface.hpp>
#include "ComediDevice.hpp"

namespace CBDeviceDriver
{

  using namespace ORO_CoreLib;

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
			 unsigned int subdevice=1 )
      : AnalogOutInterface<unsigned int>( name ),
	myCard( cao ), _subDevice( subdevice )
    {
      init();
    }

    ComediSubDeviceAOut( ComediDevice* cao, unsigned int subdevice=1 )
      : myCard( cao ), _subDevice( subdevice )
    {
      init();
    }

    void init()
    {
      if ( myCard->getSubDeviceType( _subDevice ) != COMEDI_SUBD_AO )
	{
	  _error = -1;
	  rtos_printf( "comedi_get_subdevice_type failed\n" );
	}
      int num_chan = this->nbOfChannels();
      _sd_range = new unsigned int[num_chan];
      _aref = new unsigned int[num_chan];
      // Put default range and _aref into every channel
      for (int i = 0; i < num_chan ; i++)
	{
	  _sd_range[i] = 0;
	  _aref[i] = AREF_GROUND;
	}
    }

    virtual void rangeSet(unsigned int chan, unsigned int range=0)
    {
      if ( chan < this->nbOfChannels() )
	{
	  _sd_range[chan] = range;
	}
      else rtos_printf("Go away, channel does not exist\n");
    }

    virtual void arefSet(unsigned int chan, unsigned int aref=AREF_GROUND)
    {
      if ( chan < this->nbOfChannels() )
	{
	  _aref[chan] = aref;
	}
      else rtos_printf("Go away, channel does not exist\n");
    }

    virtual void write( unsigned int chan, unsigned int value )
    {
      if ( myCard->write( _subDevice, chan, _sd_range[chan], 
			  _aref[chan], value ) != -1);
      else rtos_printf("write on subdevAOut failed\n");
    }

    virtual unsigned int binaryRange() const
    {
      return myCard->getMaxData(_subDevice);
    }

    virtual unsigned int binaryLowest() const
    {
      return 0;
    }

    virtual unsigned int binaryHighest() const
    {
      return myCard->getMaxData(_subDevice);
    }

    virtual double lowest(unsigned int chan) const
    {
      /* Damned: kcomedilib does not know comedi_range structure but
	 uses as comedi_krange struct (probably not to enforce
	 floating point support in your RT threads?)
      */
#ifdef __KERNEL__
      // See file:/usr/src/comedilib/doc/html/x3563.html#REF-TYPE-COMEDI-KRANGE
      comedi_krange range;
      comedi_get_krange(myCard->getDevice(), _subDevice, chan, 
			_sd_range[chan], &range);
      return (double) range.min / 1000000;
#else
#ifdef USE_LXRT
#define __KERNEL__
      comedi_krange range;
      comedi_get_krange(myCard->getDevice(), _subDevice, chan, 
			_sd_range[chan], &range);
      return (double) range.min / 1000000;
#else // Userspace
      comedi_range * range_p;
      if ((range_p = comedi_get_range(myCard->getDevice(), 
				      _subDevice, chan, 
				      _sd_range[chan])) != 0)
	{
	  return range_p->min;
	}
      else
	{
	  rtos_printf("Error getting comedi_range struct\n");
	  return -1.0;
	}
#endif // Userspace
#endif // __KERNEL__
    }

    virtual double highest(unsigned int chan) const
    {
      /* Damned: kcomedilib does not know comedi_range structure but
	 uses as comedi_krange struct (probably not to enforce
	 floating point support in your RT threads?)
      */
#ifdef __KERNEL__
      // See file:/usr/src/comedilib/doc/html/x3563.html#REF-TYPE-COMEDI-KRANGE
      comedi_krange range;
      comedi_get_krange(myCard->getDevice(), _subDevice, chan, 
			_sd_range[chan], &range);
      return (double) range.max / 1000000;
#else
#ifdef USE_LXRT
#define __KERNEL__
      comedi_krange range;
      comedi_get_krange(myCard->getDevice(), _subDevice, chan, 
			_sd_range[chan], range);
      return (double) range.max / 1000000;
      }
#else // Userspace
      comedi_range * range_p;
      if ((range_p = comedi_get_range(myCard->getDevice(), 
				      _subDevice, chan, 
				      _sd_range[chan])) != 0)
	{
	  return range_p->max;
	}
      else
	{
	  rtos_printf("Error getting comedi_range struct\n");
	  return -1.0;
	}
#endif // Userspace
#endif // __KERNEL__
    }

    virtual double resolution(unsigned int chan) const
    {
      return binaryRange() / ( highest(chan) - lowest(chan) );
    }

    virtual unsigned int nbOfChannels() const
    {
      return comedi_get_n_channels(myCard->getDevice(), _subDevice);
    }

  protected:
    /**
     * The output device to write to
     */
    ComediDevice* myCard;

    /**
     * The subdevice number of this instance in <myDrive>
     */
    unsigned int _subDevice;
    unsigned int * _sd_range;
    unsigned int * _aref;
    int _error;
  };

};

#endif
