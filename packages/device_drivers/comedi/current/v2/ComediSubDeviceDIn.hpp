/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  ComediSubDeviceDIn.hpp 

                        ComediSubDeviceDIn.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
// $Id: ComediSubDeviceDIn.hpp,v 1.6 2003/08/14 09:17:13 kgadeyne Exp $
#ifndef COMEDISUBDEVICEDIN_HPP
#define COMEDISUBDEVICEDIN_HPP

#include <device_interface/DigitalInInterface.hpp>
#include "ComediDevice.hpp"

namespace ORO_DeviceDriver
{

  using namespace ORO_CoreLib;
  using namespace ORO_DeviceInterface;
  /**
   * This logical device represents one subdevice of a Comedi device.
   */

  class ComediSubDeviceDIn
    : public DigitalInInterface
  {

  public:
    /**
     * Create a new ComediSubDeviceDIn with a given ComediDevice, subdevice number and
     * a name for this sub device
     *
     * @param cd The ComediDevice to use for output
     * @param subdevice The subdevice number for this comedi device
     * @param name The name of this instance
     */
    ComediSubDeviceDIn( ComediDevice* cd, const std::string& name, unsigned int subdevice)
      : DigitalInInterface( name ),
	myCard( cd ), subDevice( subdevice ) 
    {
      init();
    }

    ComediSubDeviceDIn( ComediDevice* cd, unsigned int subdevice )
      : myCard( cd ), subDevice( subdevice )
    {
      init();
    }

    void init()
    {
      if ( ( myCard->getSubDeviceType( subDevice ) != COMEDI_SUBD_DI ) &&
	   ( myCard->getSubDeviceType( subDevice ) != COMEDI_SUBD_DIO) )
	{
	  error = -1;
	  rtos_printf( "Comedi Digital In : comedi_get_subdevice_type failed\n" );
	}
      rtos_printf("Setting all dio on subdevice %d to input\n",subDevice);
      unsigned int num_chan = this->nbOfInputs();
      for (unsigned int i=0; i<num_chan; ++i)
	comedi_dio_config(myCard->getDevice(), subDevice, i, COMEDI_INPUT);
    }

    virtual bool isOn( unsigned int bit = 0) const
    {
      unsigned int tmp;
      comedi_dio_read( myCard->getDevice(),subDevice,bit, &tmp );
      return (tmp == 1);
    }

    virtual bool isOff( unsigned int bit = 0) const
    {
      return !isOn(bit);
    }

    virtual bool readBit( unsigned int bit = 0) const
    {
      return isOn(bit);
    }

    virtual unsigned int readSequence(unsigned int start_bit, unsigned int stop_bit) const
    {
      unsigned int value = 0;
      if (start_bit > stop_bit) || (stop_bit >= this->nbOfOutputs())
	{
	  rtos_printf( "Comedi Digital In : You Moron, trying to trick me?  start_bit should be less than stop_bit)\n" );
	}
      else
	{
	  // Read all channels
	  comedi_dio_bitfield(myCard->getDevice(), subDevice, 0x0, &value);
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
	  return value;
        }

      virtual unsigned int nbOfInputs() const
        {
	  return comedi_get_n_channels(myCard->getDevice(), subDevice);
        }
    protected:
      /**
       * The output device to write to
       */
      ComediDevice* myCard;

      /**
       * The subdevice number of this instance in <myCard>
       */
      unsigned int subDevice;
      int error;
    };

  };

#endif
