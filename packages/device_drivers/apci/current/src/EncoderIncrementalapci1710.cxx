/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:23:37 CEST 2002  EncoderIncrementalACPI1710.cpp 

                       EncoderIncrementalACPI1710.cpp -  description
                          -------------------
   begin                : Mon June 10 2002
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


/* KG
   Adapted as a specific implementation of EncoderIncremental<>
*/

#include <device_drivers/EncoderIncrementalapci1710.hpp>

// include lib first, if not present, include KM headers. (InKernel)
#if defined (OROINT_DEVICE_DRIVERS_APCI1710_INC) && defined(OROPKG_OS_LXRT)
#include "apci_lxrt.h"
#elif defined (OROBLD_DEVICE_DRIVERS_APCI1710_INC_KM)
extern "C"
{
#define new _new
#define class _class
#define LINUX_PCI_H
#include "apci1710/apci1710.h"
#include "apci1710/incr.h"
#undef class
#undef new
}
#endif


namespace ORO_DeviceDriver
{
  EncoderIncrementalACPI1710::EncoderIncrementalACPI1710( unsigned int encNr, 
							  const std::string& name , 
							  UnitType Scale, 
							  UnitType Offset)
    : EncoderIncremental<UnitType>(name,scale,offset), 
#ifdef OROPKG_CORELIB_EVENTS
      posEvent( 0 ), 
#endif
      encoderNr(encNr)
  {
    init();
  }

  EncoderIncrementalACPI1710::EncoderIncrementalACPI1710( unsigned int encNr,
							  UnitType Scale, 
							  UnitType Offset)
    : EncoderIncremental<UnitType>(scale,offset), 
#ifdef OROPKG_CORELIB_EVENTS
      posEvent( 0 ), 
#endif
      encoderNr(encNr)
  {
    init();
  }

  void EncoderIncrementalACPI1710::init()
  {
#ifdef OROINT_DEVICE_DRIVERS_APCI1710_INC
    rtos_printf( "Creating EncoderIncrementalACPI1710\n" );
    dev = apci1710_get_device();
    int res;
    module = apci1710_incr_create_module();
    res = apci1710_incr_init( dev, encoderNr, 
                              ORONUM_DEVICE_DRIVERS_APCI1710_INC_COUNTER,
                              ORONUM_DEVICE_DRIVERS_APCI1710_INC_MODE, 
                              ORONUM_DEVICE_DRIVERS_APCI1710_INC_HYSTERESIS, 
                              0, 0, module );
    if (res != 0) rtos_printf("ERROR! apci1710 returned %d. see incr.h !\n",res);
#endif
  }

  EncoderIncrementalACPI1710::~EncoderIncrementalACPI1710()
  {
#ifdef OROINT_DEVICE_DRIVERS_APCI1710_INC
    apci1710_incr_cleanup_module(module);
#endif
  }

#ifdef OROPKG_CORELIB_EVENTS
  void EncoderIncrementalACPI1710::handleEvent( EventInterface *e )
  {
    unsigned int tmppos=0;
#ifdef OROINT_DEVICE_DRIVERS_APCI1710_INC
    apci1710_incr_latch_counter( module, 0 );
    apci1710_incr_read_latch_register_value( module, 0, &tmppos );
#endif

    if ( posEvent )
      if ( posEvent->conditionMet( tmppos ) )
	posEvent->fire();
  }

  void EncoderIncrementalACPI1710::setEvent( EventPositionInterface* event )
  {
    posEvent = event; // only one "virtual index switch" for now
  }
#endif

  void EncoderIncrementalACPI1710::reset(int value)
  {
    if (value != 0)
      rtos_printf("Warning : reset not properly implemented in EncoderIncrementalACPI1710\n");
#ifdef OROINT_DEVICE_DRIVERS_APCI1710_INC
    // Set postion = 0
    //apci1710_incr_write_32bit_counter_value(&module,0x0);
    apci1710_incr_clear_counter_value( module );
#endif
  }

  int EncoderIncrementalACPI1710::readCounter( int& pos)
  {
#ifdef OROINT_DEVICE_DRIVERS_APCI1710_INC
    unsigned int tmppos=0;
    apci1710_incr_latch_counter( module, 0 );
    apci1710_incr_read_latch_register_value( module, 0, &tmppos );
    pos = tmppos;
    return 0;
#else
    pos = 0; 
    return -1;
#endif
        
  }

}
