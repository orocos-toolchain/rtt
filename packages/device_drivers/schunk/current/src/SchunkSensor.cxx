/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:33 CEST 2004  SchunkSensor.cxx 

                        SchunkSensor.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
#include "corelib/SchunkSensor.hpp"

extern "C"
{
#define NULL 0
#define new _new
#define class _class
#define LINUX_PCI_H
#include <linux/comedi.h>

    //only include for kernel builds
#ifndef USE_GNULINUX
#include <linux/comedilib.h>
#else
#include <comedilib.h>
#endif


#undef class
#undef new
}


namespace ORO_DeviceDriver
{

    SchunkSensor::SchunkSensor( unsigned int devmin, unsigned int subd, double pollperiod ) 
        : TaskPreemptible(pollperiod),
            SchunkSensorOutOfRange( Event::SYNSYN, "SchunkSensorOutOfRange" ),
            devminor( devmin ), subdevice( subd ), maxx( 0 ), maxy( 0 ), maxz( 0 ), maxTorqx( 0 ), maxTorqy( 0 ), maxTorqz( 0 )
    {
        char devstring[ 20 ];
        sprintf( devstring, "/dev/comedi%i", devminor );
        //open the comedi device on minor devminor
        it = comedi_open( devstring );
        rtos_printf( "comedi device opened : %s\n", devstring );

        if ( it )
        {
            //check if subdevice is really an analog input device

            if ( comedi_get_subdevice_type( it, subdevice ) != COMEDI_SUBD_AI )
            {
                rtos_printf( "not a analog input device\n" );
            }

            //set maxdata = the maximum value of a channel
            maxdata = comedi_get_maxdata( it, subdevice, 0 );

            rtos_printf( "reading sensor\n" );

            readSensor();

            rtos_printf( "sensor read\n" );
            
            start();

        }

        else
        {
            // error message and abort loading
            rtos_printf( "could not open comedi device\n" );
        }
    }

    SchunkSensor::~SchunkSensor()
    {
        stop();
    }

    // constants for counts to newton conversion
    // first 3 numbers: forces
    // second 3 numbers: torques
    // only correct when comedi range is set to 5V.
    const double SchunkSensor::factor[ 6 ] =
        {
            4, 4, 4, 66.7, 66.7, 66.7
        };

    int SchunkSensor::readSensor()
    {
        lastX = - getChannelValue( 0 );
        lastY = - getChannelValue( 1 );
        lastZ = - getChannelValue( 2 );
        lastXtor = getChannelValue( 3 );
        lastYtor = getChannelValue( 4 );
        lastZtor = getChannelValue( 5 );
        return 0;
    }

    // get last force , read by readSensor()
    double SchunkSensor::getLastX() const
    {
        return lastX;
    }

    double SchunkSensor::getLastY() const
    {
        return lastY;
    }

    double SchunkSensor::getLastZ() const
    {
        return lastZ;
    }

    void SchunkSensor::getForces(double& x, double& y,double& z) const
    {
        x = lastX; y=lastY; z=lastZ;
    }

    double SchunkSensor::getLastXTorque() const
    {
        return lastXtor;
    }

    double SchunkSensor::getLastYTorque() const
    {
        return lastYtor;
    }

    double SchunkSensor::getLastZTorque() const
    {
        return lastZtor;
    }

    void SchunkSensor::getTorques(double& x, double& y,double& z) const
    {
        x = lastXtor; y=lastYtor; z=lastZtor;
    }
    
    EventInterface* SchunkSensor::getSchunkSensorOutOfRange()
    {
        return &SchunkSensorOutOfRange;
    }

    //returns the maximum value of the channels
    unsigned int SchunkSensor::getMaxData() const
    {
        return maxdata;
    }

    //reads 1 channel on the comedi subdevice
    int SchunkSensor::readChannel( unsigned int chan, unsigned int range, unsigned int aref, unsigned int* value )
    {
        return comedi_data_read( it, subdevice, chan, range, aref, value );
    }

    //read 1 channel and returns the value in Newtons
    double SchunkSensor::getChannelValue( unsigned int chan ) const
    {
        unsigned int value = 0;
        //schunksensorrange is 5V, hence, range=1 instead of 0
        comedi_data_read( it, subdevice, chan, 1, AREF_GROUND, &value );
        return counts2newton( value, chan );
    }

    double SchunkSensor::counts2newton( unsigned int counts, int chan ) const
    {
        return ( double( counts ) - 2047. ) / factor[ chan ];
    }

    /*
     * Read Sensor and do range checking
     */
    void SchunkSensor::step()
    {
        readSensor();

        if ( ( fabs( getLastX() ) > maxx ) || ( fabs( getLastY() ) > maxy ) || ( fabs( getLastZ() ) > maxz ) || ( fabs( getLastXTorque() ) > maxTorqx ) || ( fabs( getLastYTorque() ) > maxTorqy ) || ( fabs( getLastZTorque() ) > maxTorqz ) )
            SchunkSensorOutOfRange.fire();
    }

    void SchunkSensor::setMaximumX( double max )
    {
        maxx = max;
    }

    void SchunkSensor::setMaximumY( double max )
    {
        maxy = max;
    }

    void SchunkSensor::setMaximumZ( double max )
    {
        maxz = max;
    }

    void SchunkSensor::setMaximumTorqueX( double max )
    {
        maxTorqx = max;
    }

    void SchunkSensor::setMaximumTorqueY( double max )
    {
        maxTorqy = max;
    }

    void SchunkSensor::setMaximumTorqueZ( double max )
    {
        maxTorqz = max;
    }
};
