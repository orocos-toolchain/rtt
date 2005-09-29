
#include "comedi/ComediSubDeviceDOut.hpp"
#include "os/fosi.h"

#include <pkgconf/system.h>

#include "comedi_internal.h"

namespace ORO_DeviceDriver
{
  using namespace ORO_CoreLib;
  using namespace ORO_DeviceInterface;

    ComediSubDeviceDOut::ComediSubDeviceDOut( ComediDevice* cd, const std::string& name, unsigned int subdevice)
      : DigitalOutInterface( name ),
	myCard( cd ), subDevice( subdevice ), channels(0), error(0)
    {
      init();
    }

    ComediSubDeviceDOut::ComediSubDeviceDOut( ComediDevice* cd, unsigned int subdevice )
      : myCard( cd ), subDevice( subdevice ), channels(0), error(0)
    {
      init();
    }

    void ComediSubDeviceDOut::init()
    {
      if ( ( myCard->getSubDeviceType( subDevice ) != COMEDI_SUBD_DO ) &&
	   ( myCard->getSubDeviceType( subDevice ) != COMEDI_SUBD_DIO) )
	{
	  error = -1;
	  rtos_printf( "Comedi Digital Out : comedi_get_subdevice_type failed\n" );
	  rtos_printf( "Type = %d \n", myCard->getSubDeviceType( subDevice ));
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
	comedi_dio_write( myCard->getDevice(),subDevice,bit,1);
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
