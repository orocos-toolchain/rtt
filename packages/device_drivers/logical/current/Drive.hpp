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

#include <fdi/AnalogOutInterface.hpp>
#include <fdi/DigitalOutInterface.hpp>
#include <fdi/ActuatorInterface.hpp>

namespace CBDeviceDriver

{

    using namespace ORO_CoreLib;

    /**
     * Uses a channel of a certain analog output which is connected to
     * a hardware drive with velocity control, controlling an actuator
     * and the enabling/braking of the drive.
     */
    class Drive
        : public ActuatorInterface<double>
    {

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

            virtual int drive( const InputStruct& v )
            {
                // res is in bits/[Volt|Ampere|...]
                InputStruct resolution = mySubDevice->resolution(myChannel);
                mySpeed = v;

                if ( v+offset < minDriveGet() )
                    mySpeed = minDriveGet();

                if ( v+offset > maxDriveGet() )
                    mySpeed = maxDriveGet();

                unsigned int res = ( unsigned int ) ( ( mySpeed - minDriveGet() + offset ) * resolution );

                //rtos_printf("res: %d, copy %d \n",res,(int)(copy*1000) );
                mySubDevice->write( myChannel, res );

                return 0;
            }

            virtual InputStruct speedGet() const
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
                drive( zeroGet() );
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
