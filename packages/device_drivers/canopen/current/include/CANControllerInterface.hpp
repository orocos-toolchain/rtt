/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  CANControllerInterface.hpp 

                        CANControllerInterface.hpp -  description
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
        virtual ~CANControllerInterface() {}
	};
			
}


#endif

