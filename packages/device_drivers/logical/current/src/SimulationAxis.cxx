/***************************************************************************
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

#include <device_drivers/SimulationAxis.hpp>
#include <iostream>

using namespace ORO_DeviceDriver;
using namespace ORO_DeviceInterface;
using namespace ORO_CoreLib;


SimulationEncoder::SimulationEncoder(double initial, double min, double max):
    _position(initial),
    _min(min),
    _max(max),
    _first_drive(true)
{}
  
double 
SimulationEncoder::readSensor() const
{
  if (_first_drive)
    return _position;

  else{
    // get new position, using time and velocity
    TimeService::Seconds _delta_time = TimeService::Instance()->secondsSince(_previous_time);
    return _position + (_velocity*(double)_delta_time);
  }
}

int
SimulationEncoder::readSensor(double& data) const
{
  data = readSensor();
  return 0;
}


void 
SimulationEncoder::setDrive(double velocity) 
{
  // adjust position, using previous velocity
  if (_first_drive)
    _first_drive = false;

  else{
    _delta_time = TimeService::Instance()->secondsSince(_previous_time);
    _position += _velocity*(double)_delta_time;
  }
  
  // set new velocity and start time
  _previous_time = ORO_CoreLib::TimeService::Instance()->getTicks();
  _velocity = velocity;
}











SimulationAxis::SimulationAxis(double initial, double min, double max):
  _drive_value(0),
  _enable(false),
  _velocity(0),
  _max_drive_value(std::numeric_limits<double>::max()),
  _encoder( new SimulationEncoder( initial, min, max) ),
  _velSensor( new SimulationVelocitySensor( this, _max_drive_value ) ),
  _is_locked(true),
  _is_stopped(false),
  _is_driven(false)
{}



SimulationAxis::~SimulationAxis()
{
  delete _encoder;
  delete _velSensor;
}



bool 
SimulationAxis::drive( double vel )
{
  if (_is_stopped || _is_driven){
    if ( (vel < -_max_drive_value) || (vel > _max_drive_value) ){
      std::cerr << "(SimulationAxis)  Maximum drive value exceeded. Axis.disable()" << std::endl;
      stop();
      lock();
      return false;
    }
    else{
      _encoder->setDrive(vel);
      _drive_value = vel;
      _is_stopped = false;
      _is_driven  = true;
      return true;
    }
  }
  else
    return false;
}

bool
SimulationAxis::stop()
{
  if (_is_driven){
    _encoder->setDrive(0);
    _drive_value = 0;
    _is_driven  = false;
    _is_stopped = true;
    return true;
  }
  else if (_is_stopped)
    return true;
  else
    return false;
}
  
bool
SimulationAxis::lock()
{
  if (_is_stopped){
    _is_locked  = true;
    _is_stopped = false;
    return true;
  }
  else if (_is_locked)
    return true;
  else
    return false;
}
      
bool
SimulationAxis::unlock()
{
  if (_is_locked){
    _is_locked  = false;
    _is_stopped = true;
    return true;
  }
  else if (_is_stopped)
    return true;
  else
    return false;
}

bool
SimulationAxis::isLocked() const
{
  return _is_locked;
}

bool
SimulationAxis::isStopped() const
{
  return _is_stopped;
}

bool
SimulationAxis::isDriven() const
{
  return _is_driven;
}


SensorInterface<double>* 
SimulationAxis::getSensor(const std::string& name) const
{
  if (name == "Position")
      return _encoder;
  if (name == "Velocity")
      return _velSensor;
  
  return NULL;
}


std::vector<std::string> 
SimulationAxis::sensorList() const
{
  std::vector<std::string> result;
  result.push_back("Position");
  return result;
}

double SimulationAxis::getDriveValue() const
{
  return _drive_value;
}

