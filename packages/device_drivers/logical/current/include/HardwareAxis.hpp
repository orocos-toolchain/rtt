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
    virtual ~HardwareAxis();

    bool   enable();
    bool   disable();

    bool   drive( double v );
    void   setMaxDriveValue( double v_max ) { _max_drive_value = v_max; }

    
    void               setHomingInterface( HomingInterface* HI) { _homing = HI; }
    HomingInterface*   getHomingInterface() { return _homing; }

    
    bool                                                  addSensor( const std::string& name, ORO_DeviceInterface::SensorInterface<double>* sensor );
    const ORO_DeviceInterface::SensorInterface<double>*   getSensor( const std::string& name ) const;

    
    void   setBreak( DigitalOutput* DO ) { _break = DO; _break->switchOn(); }

    void   addStopSignal( DigitalInput* DI ) { _stopsignals.push_back( DI ); }



private:
    double _max_drive_value;
    AnalogDrive* _drive;
    DigitalOutput* _break;
    HomingInterface* _homing;
    bool _enabled;
    std::map< std::string, ORO_DeviceInterface::SensorInterface<double>* > _sensormap;
    std::vector< DigitalInput* > _stopsignals;
    bool _stopsignal_active;
};

}; // namespace

#endif  //HARDWARE_AXIS_HPP
