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

#include "ComediEncoderIncremental.hpp"

namespace ORO_DeviceDriver
{
  ComediEncoderIncremental::ComediEncoderIncremental(ComediDevice * cd, 
						     unsigned int subd, 
						     unsigned int encNr, 
						     const std::string& name, 
						     UnitType Scale, 
						     UnitType Offset)
    : ORO_DeviceInterface::EncoderIncremental<UnitType>(name, Scale, Offset),
      _myCard(cd), _subDevice(subd), _channel(encNr)
  {
    init();
    rtos_printf("Comedi Counter configured as encoder now\n");
  }

  ComediEncoderIncremental::ComediEncoderIncremental(ComediDevice * cd, 
						     unsigned int subd, 
						     unsigned int encNr,
						     UnitType Scale, 
						     UnitType Offset)
    :  ORO_DeviceInterface::EncoderIncremental<UnitType>(Scale, Offset),
       _myCard(cd), _subDevice(subd), _channel(encNr)
  {
    init();
    rtos_printf( "Comedi Counter configured as encoder now\n");
  }

  void ComediEncoderIncremental::init()
  {
    rtos_printf( "Creating ComediEncoderIncremental\n" );
    // Check if subd is counter...
    if ( _myCard->getSubDeviceType( _subDevice ) != COMEDI_SUBD_COUNTER )
      {
	rtos_printf( "Comedi Counter : subdev is not a counter\n" );
	rtos_printf( "Type = %d \n", _myCard->getSubDeviceType( _subDevice ));
      }
    // Check how many counters this subdevice actually has
    unsigned int nchan = comedi_get_n_channels(_myCard->getDevice(),_subDevice);
    if ( nchan <= _channel )
      {
      	rtos_printf( "Comedi Counter : Only %d channels on this counter subdevice \n", nchan);
      }
    /* Configure the counter subdevice
       Configure the GPCT for use as an encoder 
    */
#define ENCODER_CONFIG_DATA 4
    typedef unsigned int Data;
    
    comedi_insn insn;
    Data config_data[ENCODER_CONFIG_DATA]; // Configuration data

    insn.insn=INSN_CONFIG;
    insn.n=1; // Irrelevant for config
    config_data[0] = GPCT_QUADRATURE_ENCODER;
    // Should become an option
    config_data[1] = GPCT_X4;
    config_data[2] = GPCT_IndexPhaseHighHigh;
    config_data[3] = GPCT_CONTINU_COUNTING_WHEN_INDEX_ARRIVES;

    insn.data=config_data;
    insn.subdev=_subDevice;
    insn.chanspec=CR_PACK(_channel,0,0);
    // rtos_printf("just before insn->insn = 0x%x\n",insn.insn);
    int ret=comedi_do_insn(_myCard->getDevice(),&insn);
    if(ret<0)
      {
	rtos_printf("Comedi Counter : Instruction to configure counter -> encoder failed\n");
      }
  }
  
  ComediEncoderIncremental::~ComediEncoderIncremental(){}

  void ComediEncoderIncremental::reset(int value)
  {
    // Confusing terminology: reset should resetted (ie. to zero)
    if (value != 0)
      {
	rtos_printf("Warning : reset is not true reset \n");
	rtos_printf("WARNING : can only write UNSIGNED int\n");
      }
    comedi_data_write(_myCard->getDevice(), _subDevice, 
		      _channel, 0, 0, (lsampl_t) value);
  }

  int ComediEncoderIncremental::readCounter( int& pos)
  {
    typedef unsigned int Data;

    int ret=comedi_data_read(_myCard->getDevice(),_subDevice,_channel,0,0,(unsigned int *)&pos);
    if(ret<0)
      {
	rtos_printf("Comedi Counter : reading encoder failed\n");
      }

    // Other possibility for reading the data (with instruction)
    /*    
    comedi_insn insn;
    Data readdata; // local data
    insn.insn=INSN_READ;
    insn.n=1; // Irrelevant for config
    insn.data=&readdata;
    insn.subdev=_subDevice;
    insn.chanspec=CR_PACK(_channel,0,0);
    rtos_printf("just before insn->insn = 0x%x\n",insn.insn);
    int ret=comedi_do_insn(_myCard->getDevice(),&insn);
    if(ret<0)
      {
	rtos_printf("Comedi Counter : Reading counter -> encoder failed\n");
      }
    pos = readdata;
    */
    return ret;
  } 

}
