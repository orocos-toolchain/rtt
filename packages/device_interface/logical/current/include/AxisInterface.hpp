/***************************************************************************

                       AxisInterface.hpp -  description
                          -------------------
   begin                : Thu October 21 2004
   copyright            : (C) 2002 Johan Rutgeerts
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

#ifndef _AXISINTERFACE_HPP
#define _AXISINTERFACE_HPP

#include "HomingInterface.hpp"
#include <device_interface/SensorInterface.hpp>
#include <string>

namespace ORO_DeviceInterface
{

class AxisInterface
{
public:

    AxisInterface() {};

    virtual ~AxisInterface() {};

    virtual bool enable() = 0;
    virtual bool disable() = 0;

    virtual bool drive( double v ) = 0;

    virtual HomingInterface* getHomingInterface() = 0;

    virtual const ORO_DeviceInterface::SensorInterface<double>* getSensor(const std::string& name) const = 0;
};


}; // namespace ORO_DeviceInterface

#endif //_AXISINTERFACE_HPP

