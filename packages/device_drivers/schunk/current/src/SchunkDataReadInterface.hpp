/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:22:44 CEST 2002  SchunkDataReadInterface.hpp 

                        SchunkDataReadInterface.hpp -  description
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
 

#ifndef SCHUNKDATAREADINTERFACE_HPP
#define SCHUNKDATAREADINTERFACE_HPP

#include <os/cpp.hpp>

namespace ORO_DeviceDriver
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

