/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:22:14 CEST 2002  Axis.hpp 

                       Axis.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

   ***************************************************************************
   *                                                                         *
   *   This program is free software; you can redistribute it and/or modify  *
   *   it under the terms of the GNU General Public License as published by  *
   *   the Free Software Foundation; either version 2 of the License, or     *
   *   (at your option) any later version.                                   *
   *                                                                         *
   ***************************************************************************/


#ifndef DEVICEDRIVERS_AXIS_HPP
#define DEVICEDRIVERS_AXIS_HPP

#include <device_interface/EncoderInterface.hpp>
#include <device_interface/SensorInterface.hpp>
#include "DigitalInput.hpp"
#include "AnalogDrive.hpp"

#include <math.h>

namespace ORO_DeviceDriver
{
    class Axis_PositionSensor;


    using namespace ORO_DeviceInterface;

    /**
     * @brief Axis is an example of how you can easily implement
     * a generic axis using the Device Interfaces.
     *
     * @todo PositionSensor for this axis, so that the constructor looks like:
     *       Axis( AnalogDrive* drive );
     */
    class Axis
    {
    public:

        /**
         * Constructs an Axis object and initializes the Drive,Encoder and
         * Homing switch. 
         * 
         * @post the drive is disabled
         * @post The Drive, EncoderInterface and DigitalInput are aggregated by the Axis.
         */
        Axis( AnalogDrive* a,  EncoderInterface* e, double _unit_to_inc,  DigitalInput* s);

        /**
         * Disable the Drive and destruct the Axis object.
         * @post Drive, Encoder and DigitalInput objects are deleted.
         */
        virtual ~Axis();

        /**
         * Resets the Axis.
         * @post The actuator is fully stopped.
         */
        void reset();

        void enable();

        void disable();

        bool isEnabled();

        void turnSet( int t );

        void drive( double v );

        double positionGet();

        void sensorSet(const std::string& name, SensorInterface<double>* _sens);
        SensorInterface<double>* sensorGet(const std::string& name);

        void positionSet( double newpos );
        
        void positionLimits(double min, double max);

        /**
         * Calibrate the Axis with a given calibration position
         * and direction of the counter.
         *
         * @param cal_pos The postion of the last motor-zero before the switch.
         * @param direction The positive or negative counting of the position
         *        with respect to the positive movement.
         * @return The change in position caused by this calibration.
         */
        double calibrate( double cal_pos, double direction);

        /**
         * Returns the drive used
         */
        AnalogDrive* driveGet();

        /**
         * Returns the encoder used
         */
        EncoderInterface* encoderGet();

        /**
         * Returns the homing switch used
         */
        DigitalInput* switchGet();

    private:
        /**
         * Our actuator (motor)
         */
        AnalogDrive* act;
        /**
         * Our encoder
         */
        EncoderInterface* encoder;

        /**
         * Our homing switch
         */
        DigitalInput* swt;

        double unit_to_inc;
        double posOffset;

        Axis_PositionSensor* pos_sens;

        bool status;

        std::map<std::string,SensorInterface<double>* > sens;
    };

}


#endif

