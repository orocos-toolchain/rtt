/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  HardwareAxis.cxx 

                        HardwareAxis.cxx -  description
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
 
 
#include <device_drivers/HardwareAxis.hpp>
#include <limits>

namespace ORO_DeviceDriver
{


HardwareAxis::HardwareAxis( AnalogDrive* ADrive)
    : _max_drive_value(std::numeric_limits<double>::max()), _drive(ADrive), _brake(NULL), _homing(NULL), _enabled(false)
{
    _drive->driveSet(0);
    _stopsignals.resize(0);
    _stopsignal_active = false;
}


bool HardwareAxis::enable()
{
    _drive->driveSet(0);
    _drive->enableDrive();
    if (_brake) _brake->switchOff();
    _stopsignal_active = false;
    _enabled = true;

    return true;
}



bool HardwareAxis::disable()
{
    _enabled = false;
    _drive->driveSet(0);
    if (_brake) _brake->switchOn();
    _drive->disableDrive();

    return true;
}



bool HardwareAxis::drive(double v)
{
    if( _enabled )
    {
        for (unsigned int i = 0; i < _stopsignals.size(); i++)
        {
            _stopsignal_active = _stopsignal_active || _stopsignals[i]->isOn();
        }
        
        if (_stopsignal_active) 
        {
            this->disable();
            return false;
        }
            
        if ( (v < -_max_drive_value) || (v > _max_drive_value) )
        {
            this->disable();
            return false;
        }
        else
        {
            _drive->driveSet( v );
            return true;
        }
    }

    return false;
}
    


bool HardwareAxis::addSensor( const std::string& name, SensorInterface<double>* sensor )
{
    if (_sensormap.count(name) != 0) return false;

    _sensormap.insert( make_pair(name, sensor) );
    return true;
}



const SensorInterface<double>* HardwareAxis::getSensor( const std::string& name ) const
{
    if (_sensormap.count(name) == 0)  return NULL;

    return _sensormap.find(name)->second;
}


}; // namespace 
