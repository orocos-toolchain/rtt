/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ComediSubDeviceDIn.cxx 

                        ComediSubDeviceDIn.cxx -  description
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
 
 

#include "rtt/dev/ComediSubDeviceDIn.hpp"
#include <rtt/os/fosi.h>
#include "comedi_internal.h"

namespace RTT
{

  
  

    ComediSubDeviceDIn::ComediSubDeviceDIn( ComediDevice* cd, const std::string& name, unsigned int subdevice)
      : DigitalInInterface( name ),
	myCard( cd ), _subDevice( subdevice ) 
    {
      init();
    }

    ComediSubDeviceDIn::ComediSubDeviceDIn( ComediDevice* cd, unsigned int subdevice )
      : myCard( cd ), _subDevice( subdevice )
    {
      init();
    }

    void ComediSubDeviceDIn::init()
    {
      if ( ( myCard->getSubDeviceType( _subDevice ) != COMEDI_SUBD_DI ) &&
	   ( myCard->getSubDeviceType( _subDevice ) != COMEDI_SUBD_DIO) )
	{
	  error = -1;
	  rtos_printf( "Comedi Digital In : comedi_get_subdevice_type failed\n" );
	}
      rtos_printf("Setting all dio on subdevice %d to input\n",_subDevice);
      unsigned int num_chan = this->nbOfInputs();
      for (unsigned int i=0; i<num_chan; ++i)
	comedi_dio_config(myCard->getDevice(), _subDevice, i, COMEDI_INPUT);
    }

  bool ComediSubDeviceDIn::isOn( unsigned int bit /*= 0*/) const
    {
      unsigned int tmp;
      comedi_dio_read( myCard->getDevice(),_subDevice,bit, &tmp );
      return (tmp == 1);
    }

  bool ComediSubDeviceDIn::isOff( unsigned int bit /*= 0*/) const
    {
      return !isOn(bit);
    }

  bool ComediSubDeviceDIn::readBit( unsigned int bit /*= 0*/) const
    {
      return isOn(bit);
    }

    unsigned int ComediSubDeviceDIn::nbOfInputs() const
    {
      return comedi_get_n_channels(myCard->getDevice(), _subDevice);
    }

    unsigned int ComediSubDeviceDIn::readSequence(unsigned int start_bit, unsigned int stop_bit) const
    {
      unsigned int value = 0;
      if ((start_bit > stop_bit) || (stop_bit >= this->nbOfInputs()))
	{
	  rtos_printf( "Comedi Digital In : You Moron, trying to trick me?  start_bit should be less than stop_bit)\n" );
	}
      else
	{
	  // Read all channels
	  comedi_dio_bitfield(myCard->getDevice(), _subDevice, 0x0, &value);
	  // Filter data from these channels
	  unsigned int write_mask = 0;
	  // Can somebody check this cumbersome line please?
	  for (unsigned int i = start_bit; i <= stop_bit ; i++) 
	    {
	      write_mask = write_mask | (0x1 << i);
	    }
	  // Erase other bits that we read
	  value = value & write_mask ;
	  // Shift value "start_bit" bits to the right
	  value = value >> start_bit;
	  // Everything is in read mode, so we don't have to call
	  // dio_config here (as in ComediSubDeviceOut.hpp)
	}
      
      return value;
      
    }

}
