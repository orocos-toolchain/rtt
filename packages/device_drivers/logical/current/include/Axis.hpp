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

#include "AxisInterface.hpp"
#include <rtt/Event.hpp>
#include <string>
#include <map>
#include <vector>

namespace RTT
{
    class AnalogDrive;
    class DigitalInput;
    class DigitalOutput;

    

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

        /**
         * @brief Limit the range of the axis between lower and higher
        angle. The provided event will be fired when the axis exceeds
        this range limit.
         */
        virtual void limitDrive( double lower, double higher, const Event<void(std::string)>& ev);

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

        /**
         * @brief Returns the last drive value that has been send
         */
        double getDriveValue() const;

        virtual SensorInterface<double>* getSensor(const std::string& name) const;

        /** 
         * @brief Add a sensor to the Axis (position, velocity, torque,...). The sensor
         * is aggregated.
         * 
         * @param name The name of the sensor.
         * @param _sens The sensor
         */
        void setSensor(const std::string& name, SensorInterface<double>* _sens);

      virtual std::vector<std::string> sensorList() const;

      virtual DigitalInput* getSwitch(const std::string& name) const;
      
      virtual std::vector<std::string> switchList() const;
  
        /** 
         * @brief Add a digital input to the Axis (home switch, end of run,...). The \a _digin object
         * is aggregated.
         * 
         * @param name The name of the input.
         * @param _digin The DigitalInput.
         */
        void setSwitch(const std::string& name, DigitalInput* _digin);

      virtual SensorInterface<int>* getCounter(const std::string& name) const;

      virtual std::vector<std::string> counterList() const;
  
        /** 
         * @brief Add a counter to the Axis (position, turns,...).
         * The SensorInterface object is aggregated.
         * is aggregated.
         * 
         * @param name The name of the counter.
         * @param _sens The counter.
         */
        void setCounter(const std::string& name, SensorInterface<int>* _sens);

    private:
        double _drive_value;

        /**
         * Our actuator (motor)
         */
        AnalogDrive* act;

        DigitalOutput* brakeswitch;

        typedef std::map< std::string,SensorInterface<double>* > SensList;
        typedef std::map< std::string,SensorInterface<int>* > CountList;
        typedef std::map< std::string,DigitalInput* > SwitchList;

        /**
         * All sensors (position, torque, ... )
         */
        SensList sens;
        CountList count;
        SwitchList swtch;

        bool _is_locked, _is_stopped, _is_driven;

    };

}


#endif

