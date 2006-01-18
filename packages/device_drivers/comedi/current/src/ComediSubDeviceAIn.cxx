/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  ComediSubDeviceAIn.cxx 

                        ComediSubDeviceAIn.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 

#include "comedi/ComediSubDeviceAIn.hpp"
#include "comedi_internal.h"
#include <os/fosi.h>

namespace ORO_DeviceDriver
{
  using namespace ORO_DeviceInterface;
  using namespace ORO_CoreLib;

  ComediSubDeviceAIn::ComediSubDeviceAIn( ComediDevice* cd, const std::string& name, unsigned int subdevice /*=0*/)
      : AnalogInInterface<unsigned int>( name ),
	myCard( cd ), _subDevice( subdevice ),
	_sd_range(0), _aref(0), channels(0)
    {
      init();
    }

  ComediSubDeviceAIn::ComediSubDeviceAIn( ComediDevice* cd, unsigned int subdevice /*=0*/ )
      : myCard( cd ), _subDevice( subdevice ),
	_sd_range(0), _aref(0), channels(0)
    {
      init();
    }

  ComediSubDeviceAIn::~ComediSubDeviceAIn()
  {
    delete[] _sd_range;
    delete[] _aref;
  }

    void ComediSubDeviceAIn::init()
    {
      if ( myCard->getSubDeviceType( _subDevice ) != COMEDI_SUBD_AI )
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

  void ComediSubDeviceAIn::rangeSet(unsigned int chan, unsigned int range /*=0*/)
    {
      if ( chan < channels )
	{
	  _sd_range[chan] = range;
	}
      else rtos_printf("Channel does not exist\n");
    }

  void ComediSubDeviceAIn::arefSet(unsigned int chan, unsigned int aref /*=AREF_GROUND*/)
    {
      if ( chan < channels )
	{
	  _aref[chan] = aref;
	}
      else rtos_printf("Channel does not exist\n");
    }

    void ComediSubDeviceAIn::read( unsigned int chan, unsigned int& value ) const
    {
      if ( myCard->read( _subDevice,chan, _sd_range[chan],
			 _aref[chan], value ) != -1);
      else rtos_printf("read on subdevAIn failed\n");
    }

    unsigned int ComediSubDeviceAIn::binaryRange() const
    {
      return myCard->getMaxData(_subDevice);
    }

    unsigned int ComediSubDeviceAIn::binaryLowest() const 
    {
      return 0;
    }

    unsigned int ComediSubDeviceAIn::binaryHighest() const
    {
      return myCard->getMaxData(_subDevice);
    }
        
    double ComediSubDeviceAIn::lowest(unsigned int chan) const
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

    double ComediSubDeviceAIn::highest(unsigned int chan) const
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

    double ComediSubDeviceAIn::resolution(unsigned int chan) const
    {
      return binaryRange() / ( highest(chan) - lowest(chan) );
    }

    unsigned int ComediSubDeviceAIn::nbOfChannels() const
    {
      return channels;
    }
}
