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

#include <device_interface/SensorInterface.hpp>
#include <string>
#include <vector>

namespace ORO_DeviceInterface
{

class AxisInterface
{
public:

  AxisInterface() {};
  virtual ~AxisInterface() {};

  /**
   * @brief Stop the Axis (electronically), brakes disabled,
   * drive enabled and set to zero.
   */
  virtual bool stop() = 0;

  /**
   * @brief Lock the Axis (mechanically), brakes enabled, drive
   * disabled.
   */
  virtual bool lock() = 0;

  /**
   * @brief Unlock the Axis (mechanically), brakes enabled, drive
   * enabled.
   */
  virtual bool unlock() = 0;

  /**
   * @brief Drive a certain 'physical unit' (eg velocity, torque,...).
   */
  virtual bool drive( double v ) = 0;

  /**
   * @brief Return true if the drive is disabled, and brakes are on.
   */
  virtual bool isLocked() const = 0;

  /**
   * @brief Return true if the drive is enabled but not moving, and brakes are off.
   */
  virtual bool isStopped() const = 0;

  /**
   * @brief Return true if the drive is enabled, and brakes are off.
   */
  virtual bool isDriven() const = 0;
  
  /**
   * @brief Retrieve a sensor from the Axis.
   */
  virtual const ORO_DeviceInterface::SensorInterface<double>* getSensor(const std::string& name) const = 0;

  /**
   * @brief Retrieve a list of the sensor from the Axis.
   */
  virtual std::vector<std::string> sensorList() const = 0;
  
};


}; // namespace ORO_DeviceInterface

#endif //_AXISINTERFACE_HPP

