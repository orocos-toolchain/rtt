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
    HeartBeatGenerator::Seconds _delta_time = HeartBeatGenerator::Instance()->secondsSince(_previous_time);
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
    _delta_time = HeartBeatGenerator::Instance()->secondsSince(_previous_time);
    _position += _velocity*(double)_delta_time;
  }
  
  // set new velocity and start time
  _previous_time = ORO_CoreLib::HeartBeatGenerator::Instance()->ticksGet();
  _velocity = velocity;
}










SimulationAxis::SimulationAxis(double initial, double min, double max):
  _enable(false),
  _velocity(0),
  _encoder(initial, min, max)
{}



SimulationAxis::~SimulationAxis()
{}



bool
SimulationAxis::enable()
{
  _enable = true;
  return true;
}


bool 
SimulationAxis::disable()
{
  // stop moving
  _encoder.setDrive(0);

  // disable
  _enable = false;

  return true;
}



bool 
SimulationAxis::drive( double v )
{
  // drive was not enabled
  if (!_enable)
    return false;

  // tell encoders we are moving
  else{
    
    if ( (v < -_max_drive_value) || (v > _max_drive_value) )
    {
      std::cerr << "(SimulationAxis)  Maximum drive value exceeded. Axis.disable()" << std::endl;
        this->disable();
        return false;
    }
    
    _encoder.setDrive(v);

    return true;
  }
}



HomingInterface* 
SimulationAxis::getHomingInterface()
{
  // we don't have a homing interface yet :-)
  return NULL;
}



const SensorInterface<double>* 
SimulationAxis::getSensor(const std::string& name) const
{
  if (name == "Position")
    return &_encoder;
  else
    return NULL;
}



