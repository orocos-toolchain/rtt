#ifndef CANOPENBUS_HPP
#define CANOPENBUS_HPP

#include "CANBusInterface.hpp"
#include "CANControllerInterface.hpp"
#include "CANMessage.hpp"

#include <list>
#include <algorithm>
#include <os/rtstreams.hpp>

namespace CAN
{
    using std::list;
    using std::find;
    
	/**
	 * A CAN Open Bus in its simplest (but effective) form, making use of
     * nodeId() and other CANOpen functionalities to optimise and interpret
     * the data flow.
	 */
	class CANOpenBus
        :public CANBusInterface
    {
        CANMessage syncMsg;

        public:
        /**
         * Create a CANBus instance with no devices attached to it.
         * A controller device needs to be added to put on and receive 
         * messages from the bus.
         *
         * @param _controller The Controller of the bus.
         */
        CANOpenBus()
            : controller( 0 )
        {
            syncMsg.setStdId( 0x80 );
        }

        /**
         * Write a SYNC message to the bus.
         */

        void sync()
        {
            this->write( &syncMsg );
        }

        void setController(CANControllerInterface* contr)
        {
            controller = contr;
        }
        
        virtual bool addDevice(CANDeviceInterface* dev)
        {
            if ( devices.size() < MAX_DEVICES)
                devices.push_back(dev);
            else
                return false;
            return true;
        }

        virtual bool addListener(CANListenerInterface* dev)
        {
            if ( listeners.size() < MAX_DEVICES)
                listeners.push_back(dev);
            else
                return false;
            return true;
        }

        virtual void removeDevice(CANDeviceInterface* dev)
        {
            list<CANDeviceInterface*>::iterator itl;
            itl = find(devices.begin(), devices.end(), dev);
            if ( itl != devices.end() )
                devices.erase(itl);
        }

        virtual void removeListener(CANListenerInterface* dev)
        {
            list<CANListenerInterface*>::iterator itl;
            itl = find(listeners.begin(), listeners.end(), dev);
            if ( itl != listeners.end() )
                listeners.erase(itl);
        }

        virtual void write(const CANMessage *msg)
        {
            //rt_std::cout <<"Writing...";
            // All listeners are notified.
            list<CANListenerInterface*>::iterator itl = listeners.begin();
            while ( itl != listeners.end() )
                {
                    list<CANListenerInterface*>::iterator next = (++itl)--;
                    (*itl)->process(msg); // eventually check the node id (in CANOpenBus)
                    itl = next;
                }

            if ( controller == 0 )
                return;
            if (msg->origin == 0 || msg->origin->nodeId() !=  controller->nodeId() )
                {
                    //rt_std::cout <<"to controller !\n";
                    controller->process(msg);
                }
            else
                {
                    //rt_std::cout <<"to devicelist !\n";
                    list<CANDeviceInterface*>::iterator itd = devices.begin();
                    while ( itd != devices.end() )
                        {
                            list<CANDeviceInterface*>::iterator _next = (++itd)--;
                            (*itd)->process(msg); // eventually check the node id (in CANOpenBus)
                            itd = _next;          // XXX What if itd just removed _next (or everything)?
                                                  // can be 'solved' if only self-removal is allowed.
                        }
                }
        }

        static const unsigned int MAX_DEVICES = 127;
    protected:
        list<CANDeviceInterface*> devices;
        list<CANListenerInterface*> listeners;

        CANControllerInterface* controller;
	};
			
}


#endif


