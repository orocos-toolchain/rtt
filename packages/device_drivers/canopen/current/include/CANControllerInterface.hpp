#ifndef CANCONTROLLERINTERFACE_HPP
#define CANCONTROLLERINTERFACE_HPP

#include "CANDeviceInterface.hpp"

namespace CAN
{
	/**
	 * An interface describing a generic CANController.
     * The controller will put all messages to be processed on the physical CAN bus and
     * put physical CAN messages on the software bus.
	 */
	struct CANControllerInterface 
        : public CANDeviceInterface
    {
        virtual ~CANDeviceInterface() {}
	};
			
}


#endif

