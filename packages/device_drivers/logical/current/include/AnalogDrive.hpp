/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:43 CEST 2002  AnalogDrive.hpp 

                        AnalogDrive.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
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
/* Klaas Gadeyne, Mon August 11 2003
   - Adapted to new AnalogOutInterface API
*/ 

#ifndef DEVICE_DRIVERS_ANALOGDRIVE_HPP
#define DEVICE_DRIVERS_ANALOGDRIVE_HPP

#include <device_interface/AnalogOutInterface.hpp>
#include <device_interface/DigitalOutInterface.hpp>
#include "AnalogOutput.hpp"
#include "DigitalOutput.hpp"

namespace ORO_DeviceDriver
{
    using namespace ORO_CoreLib;
    using namespace ORO_DeviceInterface;

    /**
     * @brief A AnalogDrive Object converts a physical unit (position, torque,...)
     * to an analog output. It can also be enabled.
     *
     * Uses an analog output which is connected to
     * a hardware drive with position/velocity/acceleration/torque control
     * and the enabling of the drive through a digital output.
     */
    class AnalogDrive
    {
        typedef double InputStruct;

    public:
        /**
         * Construct an Analog Drive object and aggregate
         * <an_out> and <dig_out>. The voltage sent to the AnalogOutput
         * is calculated as :
         * @verbatim
         * volt = (setpoint + offset)/scale
         * @endverbatim
         *
         * @param _scale The scale of unit per volt such that unit = volt * _scale
         * @param _offset The offset to be added to the unit such that new_unit = vel + offset
         */
        AnalogDrive( AnalogOutput<unsigned int>* an_out,
               DigitalOutput* dig_out, double _scale=1.0, double _offset=0.0 )
            : analogDevice( an_out ),
            enableDevice(dig_out), mySpeed(0.0),
            scale(_scale), offset( _offset ),
            lowvel( an_out->highest() ), highvel( an_out->lowest() )
        {
            driveSet(0);
            disableDrive();
        }

        ~AnalogDrive()
        {
            driveSet(0);
            disableDrive();

            delete analogDevice;
            delete enableDevice;
        }

        /**
         * Enable the drive.
         */
        bool enableDrive()
        {
            enableDevice->switchOn();
            return enableDevice->isOn();
        }

        /**
         * Disable the drive.
         */
        bool disableDrive()
        {
            enableDevice->switchOff();
            return !enableDevice->isOn();
        }

        DigitalOutput* enableGet()
        {
            return enableDevice;
        }

        /**
         * Limit the velocity of the drive.
         */
        void limit(double lower, double higher)
        {
            lowvel = lower;
            highvel = higher;
        }

        /**
         * Sets a new velocity.
         */
        int driveSet( double v )
        {
            mySpeed = v;

            // limit v;
            if ( mySpeed < lowvel )
                mySpeed = lowvel;
            else if ( mySpeed > highvel)
                mySpeed = highvel;

            analogDevice->value( (mySpeed+offset)/scale );

            return 0;
        }

        /**
         * Returns the current drive velocity.
         */
        double driveGet() const
        {
            return mySpeed;
        }

        /**
         * Returns the maximum drive velocity without limitations.
         */
        double maxDriveGet() const
        {
            return analogDevice->highest()*scale-offset;
        }

        /**
         * Returns the minimum drive velocity without limitations.
         */
        double minDriveGet() const
        {
            return analogDevice->lowest()*scale-offset;
        }

    protected:

        AnalogOutput<unsigned int>* analogDevice;
        DigitalOutput* enableDevice;

        double mySpeed;
        double scale, offset;
        double lowvel, highvel;
    };

}

#endif
