/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  SchunkDataReadInterface.hpp 

                        SchunkDataReadInterface.hpp -  description
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
 
 

#ifndef SCHUNKDATAREADINTERFACE_HPP
#define SCHUNKDATAREADINTERFACE_HPP

#include <os/cpp.hpp>

namespace CBDeviceDriver
{

    class SchunkDataReadInterface
    {

        public:
            virtual bool getOverloaded() = 0;
            virtual int getXForce() = 0;
            virtual int getYForce() = 0;
            virtual int getZForce() = 0;
            virtual int getXTorque() = 0;
            virtual int getYTorque() = 0;
            virtual int getZTorque() = 0;
            virtual ~SchunkDataReadInterface()
            {}

            ;
    };

};

#endif // SCHUNKDATAREADINTERFACE_HPP

