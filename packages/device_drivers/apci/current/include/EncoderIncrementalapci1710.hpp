/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:23:39 CEST 2002  EncoderIncrementalACPI1710.hpp 

                       EncoderIncrementalACPI1710.hpp -  description
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

#ifndef __ENCODERINCREMENTAL_ACPI1710_HPP
#define __ENCODERINCREMENTAL_ACPI1710_HPP



#include <pkgconf/system.h>
#include "apci1710/incr_defs.h"
#include <pkgconf/device_drivers_apci.h>

#include <device_interface/SensorInterface.hpp>
#include <device_interface/EncoderIncrementalInterface.hpp>

#include <device_interface/EncoderIncremental.hpp>

#ifdef OROPKG_CORELIB_EVENTS
#include <corelib/EventPositionInterface.hpp>
#include <corelib/EventInterfaces.hpp>
#endif

// /* Forward declarations */

// #ifndef OROPKG_OS_LXRT
// typedef struct apci1710_device_t apci1710_device;
// struct apci1710_module_t;
// #else
// typedef void apci1710_device;
// typedef void apci1710_module_t;
// #endif

namespace CBDeviceDriver
{
  using namespace ORO_CoreLib;
    using namespace ORO_DeviceInterface;
  typedef double UnitType;

  /**
   * A class for reading an incremental encoder using the APCI 1710
   * encoder card. 
   */
  class EncoderIncrementalACPI1710 :
    public EncoderIncremental<UnitType>
  {
  public:
    /**
     * Create a nameserved encoder.
     *
     * @param encNr The number of the encoder on the APCI 1710 card.
     * @param name  The name of the encoder.
     * @param Scale The scale factor for reading the position.
     * @param Offset The offset (in physical units) for reading the position.
     */
    EncoderIncrementalACPI1710( unsigned int encNr, const std::string& name, UnitType Scale=1.0, UnitType Offset=0.0 );

    /**
     * Create an encoder.
     *
     * @param encNr The number of the encoder on the APCI 1710 card.
     * @param Scale The scale factor for reading the position.
     * @param Offset The offset (in physical units) for reading the position.
     */
    EncoderIncrementalACPI1710( unsigned int encNr, UnitType Scale=1.0, UnitType Offset=0.0 );

    virtual ~EncoderIncrementalACPI1710();

    virtual int readCounter( int& p);

    virtual void reset( int value=0 );

#ifdef OROPKG_CORELIB_EVENTS
    virtual void handleEvent( EventInterface *e );

    void setEvent( EventPositionInterface* event );

  protected:
    EventPositionInterface* posEvent;
#endif
  protected:
    void init();

    unsigned int encoderNr;

#ifdef OROINT_DEVICE_DRIVERS_APCI1710
    OROIMP_DEVICE_DRIVERS_APCI1710_T* dev;
    OROIMP_DEVICE_DRIVERS_APCI1710_MODULE_T* module;
#endif
  };

};

#endif

