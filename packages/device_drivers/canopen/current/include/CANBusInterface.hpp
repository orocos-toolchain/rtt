#ifndef CANBUSINTERFACE_HPP
#define CANBUSINTERFACE_HPP

namespace CAN
{
    class CANDeviceInterface;
    class CANMessage;
    class CANControllerInterface;
    
	/**
	 * An interface describing a CANBus.
	 */
	struct CANBusInterface 
    {
        virtual ~CANBusInterface() {}

        /**
         * Specify the controller for this bus.
         */
        virtual void setController( CANControllerInterface* c) = 0;

		/**
		 * Register a CANDeviceInterface instance to the bus.
		 *
		 * @param dev The device to be added to the bus.
         * @return True if the operation succeeded, false otherwise.
		 * @post      The device will receive all messages on the bus.
		 */
        virtual bool addDevice(CANDeviceInterface* dev) = 0;

        /**
         * Remove a CANDeviceInterface from the bus.
         *
         * @param dev The device to be removed from the bus.
         * @post      The device will no longer receive messages from the bus.
         * @invar     During a process(), the device may only remove itself.
         */
        virtual void removeDevice(CANDeviceInterface* dev) = 0;

		/**
		 * Register a CANListenerInterface instance to the bus.
		 *
		 * @param dev The listener to be added to the bus.
         * @return True if the operation succeeded, false otherwise.
		 * @post      The listener will receive all messages on the bus.
		 */
        virtual bool addListener(CANListenerInterface* dev) = 0;

        /**
         * Remove a CANListenerInterface from the bus.
         *
         * @param dev The listener to be removed from the bus.
         * @post      The listener will no longer receive messages from the bus.
         * @invar     During a process(), the listener may only remove itself.
         */
        virtual void removeListener(CANListenerInterface* dev) = 0;

        /**
         * Write a CANMessage to the bus. CANDevices can use this to put data
         * on the CANBus. When this call returns, you are still the owner of
         * <msg>.
         *
         * @param msg A CANMessage which will be put on the bus.
         */
        virtual void write(const CANMessage *msg) = 0;
	};
			
}


#endif

