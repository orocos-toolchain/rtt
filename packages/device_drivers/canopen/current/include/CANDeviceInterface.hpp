/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  CANDeviceInterface.hpp 

                        CANDeviceInterface.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
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
         * @invariant Only this may be removed from the CANBusInterface during process.
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
        /**
         * A CANDevice is in the PowerOff status 
         * after construction.
         */
        CANDeviceInterface()
            : status( PowerOff ) {}
        virtual ~CANDeviceInterface() {}

        /**
         * Returns the node ID of this device.
         */
        virtual unsigned int nodeId() const = 0;

        /**
         * The CANOpen State Diagram of a Device.
         */
        enum NodeStatus { PowerOff, Initialisation, PreOperational, Stopped, Operational };

        /**
         * The state of this device.
         */
        NodeStatus status;
	};
			
}


#endif


