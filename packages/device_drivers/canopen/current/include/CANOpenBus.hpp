#ifndef CANOPENBUS_HPP
#define CANOPENBUS_HPP

#include "CANBusInterface.hpp"
#include "CANControllerInterface.hpp"

namespace CAN
{
    using std::vector;
    
	/**
	 * A CAN Open Bus in its simplest (but effective) form, making use of
     * nodeId() and other CANOpen functionalities to optimise and interpret
     * the data flow.
	 */
	class CANOpenBus
        :public CANBusInterface
    {
        public:
        /**
         * Create a CANBus instance with no devices attached to it.
         * A controller device needs to be added to put on and receive 
         * messages from the bus.
         *
         * @param _controller The Controller of the bus.
         */
        CANOpenBus( CANControllerInterface* _controller)
            : controller( _controller )
        {
            listeners.reserve(MAX_DEVICES);
        }

        void setController(CANControllerInterface* contr)
        {
            controller = contr;
        }
        
        virtual bool addDevice(CANDeviceInterface* dev)
        {
            if ( listeners.size() < MAX_DEVICES)
                listeners.push_back(dev);
            else
                return false;
            return true;
        }

        virtual void removeDevice(CANDeviceInterface* dev)
        {
            vector<CANDeviceInterface*>::iterator itl;
            itl = find(listeners.begin(), listeners.end(), dev);
            if ( itl != listeners.end() )
                listeners.erase(itl);
        }

        virtual void write(const CANMessage *msg)
        {
            if ( controller == 0 )
                return;
            if (msg->origin->nodeId() !=  controller->nodeId() )
                controller->process(msg);
            else
                for (vector<CANDeviceInterface*>::iterator itl = listeners.begin(); itl != listeners.end(); ++itl)
                    (*itl)->process(msg); // eventually check the node id (in CANOpenBus)
        }

        static const unsigned int MAX_DEVICES = 127;
    protected:
        vector<CANDeviceInterface*> listeners;

        CANControllerInterface* controller;
	};
			
}


#endif


