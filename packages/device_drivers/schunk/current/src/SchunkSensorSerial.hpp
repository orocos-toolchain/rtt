/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  SchunkSensorSerial.hpp 

                        SchunkSensorSerial.hpp -  description
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
 
 
#ifndef SCHUNKSENSORSERIAL_HPP
#define SCHUNKSENSORSERIAL_HPP

#include <os/cpp.hpp>

#ifdef __KERNEL__
extern "C"
{
#include "schunk/serial_schunk.h"
}

#endif

namespace ORO_DeviceDriver
{
    // using namespace ORO_CoreLib;
    // SchunkSensorSerial is the SchunkSensorInterface for Serial Communication
    // use SchunkSensor for Analog Communication with Comedi

    class SchunkSensorSerial
    {

        public:
            SchunkSensorSerial()
            {}

            int readSensor()
            {
                return 0;
            }

            unsigned int getLastX()
            {
                return 0;
            }

            unsigned int getLastY()
            {

                return 0;
            }

            unsigned int getLastZ()
            {
                return 0;
            };

            unsigned int getLastXtorque()
            {
                return 0;
            }

            unsigned int getLastYtorque()
            {
                return 0;
            }

            unsigned int getLastZtorque()
            {
                return 0;
            }
    };
};

#endif
