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

#include <device_interface/EncoderInterface.hpp>
#include <device_interface/SensorInterface.hpp>

#include <string>
#include <map>
#include <vector>

namespace ORO_DeviceDriver
{
    class HomePositionDetector;
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
    class Axis
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
         * @post Drive, Encoder and DigitalInput objects are deleted.
         */
        virtual ~Axis();

        /**
         * @brief Stop the Axis (electronically), breaks disabled,
         * drive enabled and set to zero.
         */
        void stop();

        /**
         * @brief Lock the Axis (mechanically), breaks enabled, drive
         * disabled.
         */
        void lock();

        /**
         * @brief Return true if the drive is enabled, and breaks are off.
         */
        bool isEnabled() const;

        /**
         * @brief Drive a certain 'physical unit' (eg velocity, torque,...).
         */
        void drive( double v );

        /**
         * @brief Add a break to the Axis. It is aggregated.
         */
        void breakSet( DigitalOutput* brk );

        /**
         * @brief Get the break of the Axis,
         * returns the inverse drive status (drive on -> break off) if none present.
         */
        DigitalOutput* breakGet() const;

        /**
         * @brief Set the Homing Switch of the Axis, it is aggregated.
         */
        void homeswitchSet( DigitalInput* swtch );

        /**
         * @brief Retrieve the homing switch of this Axis, returns a
         * HomePositionDetector based DigitalInput if none present,
         * but a "Position" Sensor is present. Returns a null if not
         * applicable.
         */
        const DigitalInput* homeswitchGet() const;

        /**
         */
        /** 
         * @brief Add a sensor to the Axis (position, velocity, torque,...). The sensor
         * is aggregated.
         * 
         * @param name The name of the sensor.
         * @param _sens The sensor
         * @param lowlim The LowLimit switch, disabling negative drive commands if \a lowlim->isOn()
         * @param highlim The HighLimit switch, disabling positive drive commands if \a highlim->isOn()
         */
        void sensorSet(const std::string& name, SensorInterface<double>* _sens, DigitalInput* lowlim, DigitalInput* highlim );

        /**
         * @brief Retrieve a sensor from the Axis.
         */
        SensorInterface<double>* sensorGet(const std::string& name) const;

        std::vector<std::string> sensorList() const;

        /**
         * @brief Returns the drive used
         */
        AnalogDrive* driveGet() const;

    private:
        struct SensorInfo
        {
            SensorInfo( SensorInterface<double>* _sens, DigitalInput* lowlim, DigitalInput* highlim )
                : sensor(_sens), low(lowlim), high(highlim)
            {}
            SensorInterface<double>* sensor;
            DigitalInput* low;
            DigitalInput* high;
        };

        /**
         * Our actuator (motor)
         */
        AnalogDrive* act;

        DigitalInput* homeswitch;

        DigitalOutput* breakswitch;

        HomePositionDetector* hpd;

        typedef std::map< std::string,SensorInfo > SensList;

        /**
         * All sensors (position, torque, ... )
         */
        SensList sens;
    };

}


#endif

