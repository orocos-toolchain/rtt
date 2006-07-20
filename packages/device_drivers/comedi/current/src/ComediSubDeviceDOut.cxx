/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  ComediSubDeviceDOut.cxx 

                        ComediSubDeviceDOut.cxx -  description
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
 
 

#include "rtt/dev/ComediSubDeviceDOut.hpp"
#include "rtt/os/fosi.h"

#include <pkgconf/system.h>

#include "comedi_internal.h"

namespace RTT
{
  
  

    ComediSubDeviceDOut::ComediSubDeviceDOut( ComediDevice* cd, const std::string& name, unsigned int subdevice)
      : DigitalOutInterface( name ),
	myCard( cd ), subDevice( subdevice ), channels(0)
    {
      init();
    }

    ComediSubDeviceDOut::ComediSubDeviceDOut( ComediDevice* cd, unsigned int subdevice )
      : myCard( cd ), subDevice( subdevice ), channels(0)
    {
      init();
    }

    void ComediSubDeviceDOut::init()
    {
      if ( ( myCard->getSubDeviceType( subDevice ) != COMEDI_SUBD_DO ) &&
	   ( myCard->getSubDeviceType( subDevice ) != COMEDI_SUBD_DIO) )
	{
	  rtos_printf( "Comedi Digital Out : comedi_get_subdevice_type failed\n" );
	  rtos_printf( "Type = %d \n", myCard->getSubDeviceType( subDevice ));
	  // channels remains '0'.
	  return;
	}
      rtos_printf("Setting all dio on subdevice %d to output\n",subDevice);

      channels = comedi_get_n_channels(myCard->getDevice(), subDevice);

      for (unsigned int i=0; i<channels; ++i)
	comedi_dio_config(myCard->getDevice(), subDevice, i, COMEDI_OUTPUT);

    }

    void ComediSubDeviceDOut::switchOn( unsigned int bit)
    {
      if (bit < channels)
	comedi_dio_write( myCard->getDevice(),subDevice,bit,1);
    }

    void ComediSubDeviceDOut::switchOff( unsigned int bit)
    {
      if (bit < channels)
	comedi_dio_write( myCard->getDevice(),subDevice,bit,0);
    }

    void ComediSubDeviceDOut::setBit( unsigned int bit, bool value)
    {
      if (bit < channels)
	{
	  if (value == true)
	    comedi_dio_write( myCard->getDevice(),subDevice,bit,1);
	  else
	    comedi_dio_write( myCard->getDevice(),subDevice,bit,0);
	}
    }


    void ComediSubDeviceDOut::setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value)
    {
      if ((start_bit > stop_bit) || (stop_bit >= channels))
	{
	  rtos_printf( "Comedi Digital Out : start_bit should be less than stop_bit) and stopbit can be bigger than the number of channels\n" );
	  return;
	}
      else
	{
	  unsigned int write_mask = 0;
	  // FIXME: Can somebody check this cumbersome line please?
	  for (unsigned int i = start_bit; i <= stop_bit ; i++) 
	    {
	      write_mask = write_mask | (0x1 << i);
	    }
	  // Shift Value startbits to the left
	  value = value << start_bit;
	  comedi_dio_bitfield(myCard->getDevice(), subDevice, write_mask, &value);
	}
      // FIXME: why is this needed ??? does bitfield change DIO mode ?
      // Make sure every bit is back to output
      for (unsigned int i=0; i<channels; ++i)
	comedi_dio_config(myCard->getDevice(), subDevice, i, COMEDI_OUTPUT);
    }

    bool ComediSubDeviceDOut::checkBit( unsigned int bit) const
    {
      unsigned int value = 0;
      // At this moment, only supported for DIO subdevices (not DO)
      if ( myCard->getSubDeviceType( subDevice ) != COMEDI_SUBD_DIO)
	{
	  //rtos_printf( "Comedi Digital Out : CheckBit not implemented for DO (only DIO), returning true\n" );
	  //rtos_printf( "Type = %d \n", myCard->getSubDeviceType( subDevice ));
	  return true;
	}
      // Read all channels
      comedi_dio_bitfield(myCard->getDevice(), subDevice, 0x0, &value);
      // Filter data from these channels
      unsigned int write_mask = (0x1 << bit);
      // Erase other bits that we read
      value = value & write_mask ;
      // Shift value bits to the right
      value = value >> bit;
      // FIXME: Is this needed ??
      // Make sure every bit is back to output
      for (unsigned int i=0; i<channels; ++i)
	comedi_dio_config(myCard->getDevice(), subDevice, i, COMEDI_OUTPUT);
      return value;
    }

    unsigned int ComediSubDeviceDOut::checkSequence (unsigned int start_bit, unsigned int stop_bit) const
    {
      rtos_printf( "Comedi Digital Out : CheckSequence not implemented, returning 0x0\n" );
      return 0;
    }

    unsigned int ComediSubDeviceDOut::nbOfOutputs() const
    {
      return channels;
    }


}
