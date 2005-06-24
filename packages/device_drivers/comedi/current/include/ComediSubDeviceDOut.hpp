// $Id: ComediSubDeviceDOut.hpp,v 1.4 2003/08/14 09:17:13 kgadeyne Exp $
// Copyright (C) 2003 Klaas Gadeyne <klaas.gadeyne@mech.kuleuven.ac.be>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  

/* Based completely on ComediSubDeviceDin.hpp,  basically I didn't do
   anything but M-x query-replace [Enter] in [Enter] out :-)
   And changed the api to that of DigitalOutInterface offcourse.
   And the implementation: M-x query-replace [Enter] read [Enter] write :-)
   And some other things...
   And ...
*/

#ifndef COMEDISUBDEVICEDOUT_HPP
#define COMEDISUBDEVICEDOUT_HPP

#include <device_interface/DigitalOutInterface.hpp>
#include "ComediDevice.hpp"

namespace ORO_DeviceDriver
{
  using namespace ORO_CoreLib;
  using namespace ORO_DeviceInterface;

  /**
   * This logical device represents one subdevice of a Comedi device.
   */

  class ComediSubDeviceDOut
    : public DigitalOutInterface
  {

  public:
    /**
     * Create a new ComediSubDeviceDOut with a given ComediDevice, subdevice number and
     * a name for this sub device
     *
     * @param cd The ComediDevice to use for output
     * @param subdevice The subdevice number for this comedi device
     * @param name The name of this instance
     */
    ComediSubDeviceDOut( ComediDevice* cd, const std::string& name, unsigned int subdevice)
      : DigitalOutInterface( name ),
	myCard( cd ), subDevice( subdevice ) 
    {
      init();
    }

    ComediSubDeviceDOut( ComediDevice* cd, unsigned int subdevice )
      : myCard( cd ), subDevice( subdevice )
    {
      init();
    }

    void init()
    {
      if ( ( myCard->getSubDeviceType( subDevice ) != COMEDI_SUBD_DO ) &&
	   ( myCard->getSubDeviceType( subDevice ) != COMEDI_SUBD_DIO) )
	{
	  error = -1;
	  rtos_printf( "Comedi Digital Out : comedi_get_subdevice_type failed\n" );
	  rtos_printf( "Type = %d \n", myCard->getSubDeviceType( subDevice ));
	}
      rtos_printf("Setting all dio on subdevice %d to output\n",subDevice);
      unsigned int num_chan = this->nbOfOutputs();
      for (unsigned int i=0; i<num_chan; ++i)
	comedi_dio_config(myCard->getDevice(), subDevice, i, COMEDI_OUTPUT);
    }

    virtual void switchOn( unsigned int bit)
    {
      this->setBit(bit,true);
    }

    virtual void switchOff( unsigned int bit)
    {
      this->setBit(bit,false);
    }

    virtual void setBit( unsigned int bit, bool value)
    {
      if (bit >= this->nbOfOutputs())
	{
	  error = -1;
	  rtos_printf( "Comedi Digital Out : No such channel\n" );
	}
      else
	{
	  if (value == true)
	    comedi_dio_write( myCard->getDevice(),subDevice,bit,1);
	  else comedi_dio_write( myCard->getDevice(),subDevice,bit,0);
	}
    }


    virtual void setSequence(unsigned int start_bit, unsigned int stop_bit, unsigned int value)
    {
      if ((start_bit > stop_bit) || (stop_bit >= this->nbOfOutputs()))
	{
	  rtos_printf( "Comedi Digital Out : You Moron, trying to trick me?  start_bit should be less than stop_bit) and stopbit can be bigger than the number of channels\n" );
	}
      else
	{
	  unsigned int write_mask = 0;
	  // Can somebody check this cumbersome line please?
	  for (unsigned int i = start_bit; i <= stop_bit ; i++) 
	    {
	      write_mask = write_mask | (0x1 << i);
	    }
	  // Shift Value startbits to the left
	  value = value << start_bit;
	  comedi_dio_bitfield(myCard->getDevice(), subDevice, write_mask, &value);
	}
      // Make sure every bit is back to output
      unsigned int num_chan = this->nbOfOutputs();
      for (unsigned int i=0; i<num_chan; ++i)
	comedi_dio_config(myCard->getDevice(), subDevice, i, COMEDI_OUTPUT);
    }

    virtual bool checkBit( unsigned int bit) const
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
      // Make sure every bit is back to output
      unsigned int num_chan = this->nbOfOutputs();
      for (unsigned int i=0; i<num_chan; ++i)
	comedi_dio_config(myCard->getDevice(), subDevice, i, COMEDI_OUTPUT);
      return value;
    }

    virtual unsigned int checkSequence (unsigned int start_bit, unsigned int stop_bit) const
    {
      rtos_printf( "Comedi Digital Out : CheckSequence not implemented, returning 0x0\n" );
      return 0;
    }

    virtual unsigned int nbOfOutputs() const
    {
      return comedi_get_n_channels(myCard->getDevice(), subDevice);
    }

  protected:
    /**
     * The output device to write to
     */
    ComediDevice* myCard;

    /**
     * The subdevice number of this instance in \a myCard
     */
    unsigned int subDevice;
    int error;
  };

};

#endif
