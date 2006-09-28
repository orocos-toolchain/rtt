/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  CANControllerInterface.hpp 

                        CANControllerInterface.hpp -  description
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
 
#ifndef CANCONTROLLERINTERFACE_HPP
#define CANCONTROLLERINTERFACE_HPP

#include "CANDeviceInterface.hpp"

namespace RTT
{namespace CAN
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
			
}}


#endif

