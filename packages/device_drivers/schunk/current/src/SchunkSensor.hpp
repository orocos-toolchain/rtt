/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:43 CEST 2002  SchunkSensor.hpp 

                        SchunkSensor.hpp -  description
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
 
/* koen.muylkens@esat.kuleuven.ac.be */

#ifndef SCHUNKSENSOR_HPP
#define SCHUNKSENSOR_HPP

#include <os/fosi.h>
#include <os/cpp.hpp>

#include <corelib/EventInterfaces.hpp>
#include <corelib/Event.hpp>
#include <corelib/ListenerRegistration.hpp>
#include <corelib/TaskPreemptible.hpp>

#include <fdi/ForceSensor6DInterface.hpp>

#ifdef __KERNEL__
typedef void comedi_t;
#else

struct comedi_t_struct;

typedef struct comedi_t_struct comedi_t;
#endif



namespace ORO_DeviceDriver
{

    using namespace ORO_CoreLib;

    // * SchunkSensor is the ForceSensor6DInterface for Analog Communication with
    // the SchunkSensor via Comedi.
    // * use SchunkSensorSerial for Serial Communication with the SchunkSensor
    // * constructor has 3 argument: a device number, a subdevice number and a polltime
    // * polltime is in seconds

    class SchunkSensor 
        : public ForceSensor6DInterface, 
          public TaskPreemptible
    {
        public:
            SchunkSensor( unsigned int devmin, unsigned int subd, double polltime );
            virtual ~SchunkSensor();

            //Reads all six channels of the Sensor
            virtual int readSensor();

            // get last force , read by readSensor()
            virtual double getLastX() const;
            virtual double getLastY() const;
            virtual double getLastZ() const;
            virtual void getForces(double&, double&, double& ) const;
            virtual double getLastXTorque() const;
            virtual double getLastYTorque() const;
            virtual double getLastZTorque() const;
            virtual void getTorques(double&, double&, double& ) const;

            // Read Sensor and do range checking
            virtual void step( ) ;

            //Set maxima for RangeChecking
            virtual void setMaximumX( double max );
            virtual void setMaximumY( double max );
            virtual void setMaximumZ( double max );
            virtual void setMaximumTorqueX( double max );
            virtual void setMaximumTorqueY( double max );
            virtual void setMaximumTorqueZ( double max );

            EventInterface* getSchunkSensorOutOfRange();
        protected:
            //returns the maximum value of the channels
            unsigned int getMaxData() const;

            //reads 1 channel on the comedi subdevice
            int readChannel( unsigned int chan, unsigned int range, unsigned int aref, unsigned int* value );

            //read 1 channel and returns the value in Newtons
            double getChannelValue( unsigned int chan ) const;

            double counts2newton( unsigned int counts, int chan ) const;

            //event fired when sensorvalues exceeds maxima's
            Event SchunkSensorOutOfRange;

        //private:
            unsigned int devminor, subdevice, maxdata;
            comedi_t* it;

            // last forces/torques  read by readSensor
            double lastX, lastY, lastZ, lastXtor, lastYtor, lastZtor;

            //maxima used by rangecheck
            double maxx, maxy, maxz;
            double maxTorqx, maxTorqy, maxTorqz;

            //constants for volts to newton conversion
            static const double factor[ 6 ];

    };

};

#endif
