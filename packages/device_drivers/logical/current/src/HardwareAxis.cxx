#include <device_drivers/HardwareAxis.hpp>
#include <limits>

namespace ORO_DeviceDriver
{


HardwareAxis::HardwareAxis( AnalogDrive* ADrive)
    : _max_drive_value(std::numeric_limits<double>::max()), _drive(ADrive), _break(NULL), _homing(NULL), _enabled(false)
{
    _drive->driveSet(0);
    _stopsignals.resize(0);
    _stopsignal_active = false;
}


bool HardwareAxis::enable()
{
    _drive->driveSet(0);
    _drive->enableDrive();
    if (_break) _break->switchOff();
    _stopsignal_active = false;
    _enabled = true;

    return true;
}



bool HardwareAxis::disable()
{
    _enabled = false;
    _drive->driveSet(0);
    if (_break) _break->switchOn();
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
