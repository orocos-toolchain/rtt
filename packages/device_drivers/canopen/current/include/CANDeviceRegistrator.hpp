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


