#ifndef CANDEVICEINTERFACE_HPP
#define CANDEVICEINTERFACE_HPP

namespace CAN
{
    class CANMessage;

    /**
     * A CANListenerInterface instance receives any message posted on the bus.
     */
    struct CANListenerInterface
    {
        virtual ~CANListenerInterface() {}

		/**
		 * This method instructs the CANDeviceInterface instance to process a
         * certain CANMessage. You are not the owner of <msg>.
		 *
		 * @param msg The message to be processed by this instance.
         * @invar Only this may be removed from the CANBusInterface during process.
		 */
        virtual void process(const CANMessage* msg) = 0;
    };
    
	/**
	 * An interface describing a generic CANOpenDevice. 
     * This can be the controller or a normal slave.
	 */
	struct CANDeviceInterface 
        : public CANListenerInterface
    {
        virtual ~CANDeviceInterface() {}

        /**
         * Returns the node ID of this device.
         */
        virtual unsigned int nodeId() const = 0;
	};
			
}


#endif


