/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:22:14 CEST 2002  Axis.hpp 

                       Axis.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
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

#ifndef DEVICEDRIVERS_AXIS_HPP
#define DEVICEDRIVERS_AXIS_HPP

#include <device_interface/AxisInterface.hpp>
#include <corelib/Event.hpp>
#include <string>
#include <map>
#include <vector>

namespace ORO_DeviceDriver
{
    class AnalogDrive;
    class DigitalInput;
    class DigitalOutput;

    using namespace ORO_DeviceInterface;

    /**
     * @brief Axis is an example of how you can easily implement
     * a generic axis using the Device Interfaces.
     *
     * Calibration of
     * the sensors must be done outside of this object, since sensor
     * calibration is to specific per sensor.
     * @see CalibrationInterface
     *
     * The EndLimit switches limit the drive() command. Each Sensor
     * added must impose a software limit on drive().
     */
    class Axis : public AxisInterface
    {
    public:

        /** 
         * @brief Create an Axis with an AnalogDrive.
         * 
         * @param adrive The Analog drive of this axis.
         */
        Axis( AnalogDrive* adrive );

        /**
         * @brief Disable the Drive and destruct the Axis object.
         * @post Drive, Sensors and DigitalInput objects are deleted.
         */
        virtual ~Axis();

        virtual bool stop();
        virtual bool lock();
        virtual bool unlock();
        virtual bool drive( double v );
        virtual bool isLocked() const;
        virtual bool isStopped() const;
        virtual bool isDriven() const;

        virtual void limitDrive( double max );
        virtual void setLimitDriveEvent(ORO_CoreLib::Event<void(void)>& maximumDrive);

        /**
         * @brief Add a brake to the Axis. It is aggregated.
         */
        void setBrake( DigitalOutput* brk );

        /**
         * @brief Get the brake of the Axis,
         * returns the inverse drive status (drive on -> brake off) if none present.
         */
        DigitalOutput* getBrake() const;

        /**
         * @brief Sets the drive used
         */
        void setDrive( AnalogDrive* a);

        /**
         * @brief Returns the drive used
         */
        AnalogDrive* getDrive() const;

        virtual const SensorInterface<double>* getSensor(const std::string& name) const;

        /** 
         * @brief Add a sensor to the Axis (position, velocity, torque,...). The sensor
         * is aggregated.
         * 
         * @param name The name of the sensor.
         * @param _sens The sensor
         * @param lowlim The LowLimit switch, disabling negative drive commands if \a lowlim->isOn()

         * @param highlim The HighLimit switch, disabling positive drive commands if \a highlim->isOn()
         */
        void setSensor(const std::string& name, SensorInterface<double>* _sens);

        virtual std::vector<std::string> sensorList() const;


    private:
        /**
         * Our actuator (motor)
         */
        AnalogDrive* act;

        DigitalOutput* brakeswitch;

        typedef std::map< std::string,SensorInterface<double>* > SensList;

        /**
         * All sensors (position, torque, ... )
         */
        SensList sens;

        bool _is_locked, _is_stopped, _is_driven;

        /**
         * Maximum drive value and event
         */
        double _max_drive;
        ORO_CoreLib::Event<void(void)>* _max_drive_event;
    };

}


#endif

