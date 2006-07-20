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

#include "comedi/ComediSubDeviceAOut.hpp"
#include "comedi/ComediDevice.hpp"
#include <rtt/rtt/os/fosi.h>
#include "comedi_internal.h"

namespace RTT
{

  
  

    ComediSubDeviceAOut::ComediSubDeviceAOut( ComediDevice* cao, const std::string& name, 
					      unsigned int subdevice /*=1*/ )
      : AnalogOutInterface<unsigned int>( name ),
	myCard( cao ), _subDevice( subdevice ),
	_sd_range(0), _aref(0), channels(0)
    {
      init();
    }

  ComediSubDeviceAOut::ComediSubDeviceAOut( ComediDevice* cao, unsigned int subdevice /*=1*/ )
      : myCard( cao ), _subDevice( subdevice ),
	_sd_range(0), _aref(0), channels(0)
    {
      init();
    }

  ComediSubDeviceAOut::~ComediSubDeviceAOut()
  {
    delete[] _sd_range;
    delete[] _aref;
  }

    void ComediSubDeviceAOut::init()
    {
      if ( myCard->getSubDeviceType( _subDevice ) != COMEDI_SUBD_AO )
	{
	  rtos_printf( "comedi_get_subdevice_type failed\n" );
	}

      channels = comedi_get_n_channels(myCard->getDevice(), _subDevice);

      _sd_range = new unsigned int[channels];
      _aref = new unsigned int[channels];
      // Put default range and _aref into every channel
      for (unsigned int i = 0; i < channels ; i++)
	{
	  _sd_range[i] = 0;
	  _aref[i] = AREF_GROUND;
	}
    }

  void ComediSubDeviceAOut::rangeSet(unsigned int chan, unsigned int range /*=0*/)
    {
      if ( chan < channels )
	{
	  _sd_range[chan] = range;
	}
      else rtos_printf("Channel does not exist\n");
    }

  void ComediSubDeviceAOut::arefSet(unsigned int chan, unsigned int aref /*=AREF_GROUND*/)
    {
      if ( chan < channels )
	{
	  _aref[chan] = aref;
	}
      else rtos_printf("Channel does not exist\n");
    }

    void ComediSubDeviceAOut::write( unsigned int chan, unsigned int value )
    {
      if ( myCard->write( _subDevice, chan, _sd_range[chan], 
			  _aref[chan], value ) != -1);
      else rtos_printf("write on subdevAOut failed\n");
    }

    unsigned int ComediSubDeviceAOut::binaryRange() const
    {
      return myCard->getMaxData(_subDevice);
    }

    unsigned int ComediSubDeviceAOut::binaryLowest() const
    {
      return 0;
    }

    unsigned int ComediSubDeviceAOut::binaryHighest() const
    {
      return myCard->getMaxData(_subDevice);
    }

    double ComediSubDeviceAOut::lowest(unsigned int chan) const
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
#ifdef OROPKG_OS_LXRT
//#define __KERNEL__
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

    double ComediSubDeviceAOut::highest(unsigned int chan) const
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
#ifdef OROPKG_OS_LXRT
//#define __KERNEL__
      comedi_krange range;
      comedi_get_krange(myCard->getDevice(), _subDevice, chan, 
			_sd_range[chan], &range);
      return (double) range.max / 1000000;
      
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

    double ComediSubDeviceAOut::resolution(unsigned int chan) const
    {
      return binaryRange() / ( highest(chan) - lowest(chan) );
    }

    unsigned int ComediSubDeviceAOut::nbOfChannels() const
    {
      return channels;
    }

};

