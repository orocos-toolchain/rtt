/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  CANDeviceRegistrator.hpp 

                        CANDeviceRegistrator.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef CANDEVICE_HPP
#define CANDEVICE_HPP

#include "CANDeviceInterface.hpp"
#include "CANBusInterface.hpp"

namespace CAN
{
    
	/**
	 * A helper class registering and deregistering a CANDevice from the bus.
	 */
	class CANDeviceRegistrator
    {
		/**
		 * Register a CANDeviceInterface to a bus.
		 *
		 * @param bus The bus to be registered to.
		 * @param dev The device to be registered to the bus.
		 */
        CANDeviceRegistrator(CANDeviceInterface* dev, CANBusInterface* bus) {}
            
        /**
         * Removes the CANDeviceInterface from the CANBus.
         */
        virtual ~CANDevice() {}
        
        protected:
        
        CANBusInterface* myBus;
        CANDeviceInterface* myDev;
	};
}


#endif


