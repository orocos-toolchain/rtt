/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  ForceSensor6DInterface.hpp 

                        ForceSensor6DInterface.hpp -  description
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
 
 
#ifndef FORCESENSOR6DINTERFACE_HPP
#define FROCESENSOR6DINTERFACE_HPP


namespace ORO_DeviceInterface
{
    /**
     * An interface to the SchunkSensor.
     * Analog via SchunkSensor: via Comedi
     * Serial via SchunkSensorSerial: via rt_com
     * reads and stores 3 forces: x,y,z and 3 torques: x,y,z 
    */

    class ForceSensor6DInterface
    {

        public:
            //reads and stores all six forces/torques
            virtual int readSensor() = 0;
            //get last x,y,z force read by readSensor()
            virtual double getLastX() const = 0;
            virtual double getLastY() const = 0;
            virtual double getLastZ() const = 0;
            virtual void getForces(double&, double&, double& ) const = 0;
            //get last x,y,z torqe read by readSensor()
            virtual double getLastXTorque() const = 0;
            virtual double getLastYTorque() const = 0;
            virtual double getLastZTorque() const = 0;
            virtual void getTorques(double&, double&, double& ) const = 0;
            virtual ~ForceSensor6DInterface()
            {}
    };

}

#endif 
