/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:43 CEST 2002  Drive.hpp 

                        Drive.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
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
 
/* Klaas Gadeyne, Mon August 11 2003
   - Adapted to new AnalogOutInterface API
*/ 

#ifndef DRIVE_HPP
#define DRIVE_HPP

#include <device_interface/AnalogOutInterface.hpp>
#include <device_interface/DigitalOutInterface.hpp>
//#include <device_interface/ActuatorInterface.hpp>
//#include <iostream>

namespace ORO_DeviceDriver

{
    using namespace ORO_CoreLib;
    using namespace ORO_DeviceInterface;
    using namespace std;

    /**
     * Uses a channel of a certain analog output which is connected to
     * a hardware drive with velocity control, controlling an actuator
     * and the enabling/braking of the drive.
     */
    class Drive // VelocityDrive
    //: public ActuatorInterface<double>
    {
        typedef double InputStruct;

        public:
            /**
             * Construct an Analog Drive object. It sets its setpoint on channel
             * <chan> on device <an_out>. Enabling and disabling of the drive
             * happens on <dig_out> and bit <bit>.
             */
            Drive( AnalogOutInterface<unsigned int>* an_out, unsigned int chan, 
                   DigitalOutInterface* dig_out, unsigned int bit )
                : mySubDevice( an_out ), myChannel( chan ), 
                enableDevice(dig_out), enableBit(bit),offset( 0 )
            {}

            virtual ~Drive()
            {}

            /**
             * Enable the drive.
             */
            virtual bool enableDrive()
            {
                enableDevice->switchOn(enableBit);
                return enableDevice->checkBit(enableBit);
            }

            /**
             * Disable the drive.
             */
            virtual bool disableDrive()
            {
                enableDevice->switchOff(enableBit);
                return !enableDevice->checkBit(enableBit);
            }

            virtual int driveSet( double v )
            {
                // res is in bits/[Volt|Ampere|...]
                InputStruct resolution = mySubDevice->resolution(myChannel);
                mySpeed = v;

                if ( v+offset < minDriveGet() )
                    mySpeed = minDriveGet();

                if ( v+offset > maxDriveGet() )
                    mySpeed = maxDriveGet();

                unsigned int res = mySubDevice->binaryLowest() + unsigned ( ( mySpeed - minDriveGet() + offset ) * resolution );

                //rtos_printf("res: %d, copy %d \n",res,(int)(copy*1000) );
                //cout <<"Offset:"<<offset<<" "<<( mySpeed - minDriveGet() + offset ) * resolution << endl;
                //cout.setf ( ios_base::hex, ios_base::basefield );  // set hex as the basefield
                //cout.setf ( ios_base::showbase );                  // activate showbase
                //cout <<"RES: "<< res << "," << (res & mySubDevice->binaryRange()) <<endl;
                mySubDevice->write( myChannel, res & mySubDevice->binaryRange() );

                return 0;
            }

            virtual InputStruct driveGet() const
            {
                return mySpeed;
            }

            virtual InputStruct maxDriveGet() const
            {
                return mySubDevice->highest(myChannel);
            }

            virtual InputStruct minDriveGet() const
            {
                return mySubDevice->lowest(myChannel);
            }

            virtual InputStruct zeroGet() const
            {
                return 0.0; // XXX get this from the card ?
            }

            virtual void stop()
            {
                driveSet( zeroGet() );
            }

            virtual void offsetSet( const InputStruct& o )
            {
                offset = o;
            }

        protected:

        AnalogOutInterface<unsigned int>* mySubDevice;
        unsigned int myChannel;

        DigitalOutInterface* enableDevice;
        unsigned int enableBit;

        InputStruct offset;
        InputStruct mySpeed;
    };

}

#endif
