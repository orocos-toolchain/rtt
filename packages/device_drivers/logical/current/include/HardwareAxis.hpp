/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  HardwareAxis.hpp 

                        HardwareAxis.hpp -  description
                           -------------------
    begin                : Tue December 21 2004
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
 
 
#ifndef HARDWARE_AXIS_HPP
#define HARDWARE_AXIS_HPP


#include <device_interface/AxisInterface.hpp>
#include <device_drivers/AnalogDrive.hpp>
#include <device_drivers/DigitalInput.hpp>
#include <device_drivers/DigitalOutput.hpp>
#include <vector>


namespace ORO_DeviceDriver
{

    
class HardwareAxis : public AxisInterface
{
public:
    HardwareAxis( AnalogDrive* ADrive );
    virtual ~HardwareAxis() {};

    bool   enable();
    bool   disable();

    bool   drive( double v );
    void   setMaxDriveValue( double v_max ) { _max_drive_value = v_max; }

    
    void               setHomingInterface( HomingInterface* HI) { _homing = HI; }
    HomingInterface*   getHomingInterface() { return _homing; }

    
    bool                                                  addSensor( const std::string& name, ORO_DeviceInterface::SensorInterface<double>* sensor );
    const ORO_DeviceInterface::SensorInterface<double>*   getSensor( const std::string& name ) const;

    
    void   setBrake( DigitalOutput* DO ) { _brake = DO; _brake->switchOn(); }

    void   addStopSignal( DigitalInput* DI ) { _stopsignals.push_back( DI ); }



private:
    double _max_drive_value;
    AnalogDrive* _drive;
    DigitalOutput* _brake;
    HomingInterface* _homing;
    bool _enabled;
    std::map< std::string, ORO_DeviceInterface::SensorInterface<double>* > _sensormap;
    std::vector< DigitalInput* > _stopsignals;
    bool _stopsignal_active;
};

}; // namespace

#endif  //HARDWARE_AXIS_HPP
