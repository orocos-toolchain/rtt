/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:22:14 CEST 2002  Axis.hpp 

                          HomingInterface.hpp -  description
                          -------------------
   begin                : Thu October 21 2004
   copyright            : (C) 2004 Johan Rutgeerts
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

#ifndef _HOMING_INTERFACE_HPP
#define _HOMING_INTERFACE_HPP

namespace ORO_DeviceInterface
{

class HomingInterface
{
public:

    /**
     * A HomingInterface provides all necessary data to perform axis homing.
     *
     */
    HomingInterface();

    virtual ~HomingInterface();

    /**
     * Returns true if a 'home switch event' has occurred 
     * (e.g. a home switch pulsed or changed its state) since
     * the last reset.
     */
    virtual bool homingStatus() = 0;

    
    /**
     * Sets the homingstatus to false.
     */
    virtual void resetHomingStatus() = 0;

    
    /**
     * Returns the recommended drive value for coarse movement to
     * the home switch. If the axis is driven with this value,
     * a 'home switch event' is guaranteed to occur.
     */
    virtual double homingDriveValue() = 0;

};

}; //namespace ORO_DeviceInterface

#endif //_HOMING_INTERFACE_HPP

