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

#ifndef _SIM_AXIS_HPP
#define _SIM_AXIS_HPP

#include <device_interface/AxisInterface.hpp>
#include <corelib/HeartBeatGenerator.hpp>

namespace ORO_DeviceDriver
{

class SimulationEncoder: public ORO_DeviceInterface::SensorInterface<double>
{
public:
  SimulationEncoder(double initial=0, double min=-10, double max=10);
  virtual ~SimulationEncoder() {};
  
  virtual double readSensor() const;
  virtual int readSensor(double& data) const;
  virtual double maxMeasurement() const {return _max; };
  virtual double minMeasurement() const {return _min; };
  virtual double zeroMeasurement() const {return 0.0;};

  void setDrive(double velocity);
  
private:
  double _position, _velocity, _min, _max;
  ORO_CoreLib::HeartBeatGenerator::ticks _previous_time;
  ORO_CoreLib::HeartBeatGenerator::Seconds _delta_time;
  bool _first_drive;

}; // class



class SimulationAxis: public ORO_DeviceInterface::AxisInterface
{
public:
  SimulationAxis(double initial=0, double min=-10, double max=10);
  virtual ~SimulationAxis();

  virtual bool enable();
  virtual bool disable();

  virtual bool drive( double v );
  virtual ORO_DeviceInterface::HomingInterface* getHomingInterface();
  virtual const ORO_DeviceInterface::SensorInterface<double>* getSensor(const std::string& name) const;

  void   setMaxDriveValue( double v_max ) { _max_drive_value = v_max; }
private:
  bool        _enable;
  double      _velocity;
  double      _max_drive_value;
  SimulationEncoder  _encoder;
  
  
}; // class

 
} // namespace






#endif //_SIM_AXIS_HPP

