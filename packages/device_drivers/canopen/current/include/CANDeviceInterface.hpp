#ifndef CANDEVICEINTERFACE_HPP
#define CANDEVICEINTERFACE_HPP

namespace CAN
{
    class CANMessage;

    struct CANListenerInterface
    {
		/**
		 * This method instructs the CANDeviceInterface instance to process a
         * certain CANMessage. You are not the owner of <msg>.
		 *
		 * @param msg The message to be processed by this instance.
		 */
        virtual void process(const CANMessage* msg) = 0;
    };
    
	/**
	 * An interface describing a generic CANDevice. This can be the controller or
     * a normal slave.
	 */
	struct CANDeviceInterface 
        : public CANListenerInterface
    {
        /**
         * Returns the node ID of this device.
         */
        virtual unsigned int nodeId() const = 0;
	};
			
}


#endif


