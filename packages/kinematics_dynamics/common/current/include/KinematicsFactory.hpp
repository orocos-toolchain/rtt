/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  KinematicsFactory.hpp 

                        KinematicsFactory.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 

#ifndef KINEMATICSFACTORY_HPP
#define KINEMATICSFACTORY_HPP

#include <corelib/Logger.hpp>
#include <math.h>
#include <iostream>
#include "KinematicsInterface.hpp"
#include "SerialZYYDWH.hpp"
#include "SerialmZXXmZXmZ.hpp"



namespace ORO_KinDyn
{
  /**
   * The KinematicsFactory can be asked to create the library for
   * a kind of robot
   */
    class KinematicsFactory
    {
    public:
      /**
       * Create a KinematicsInterface instance for a given robot.
       *
       * @param name The name of the robot.
       * @return A KinematicInterface instance or zero if the robot is not found.
       */
        static
        KinematicsInterface* create(const std::string& name)
      {
	if (name == "kuka361"){
	  SerialZYYDWH* ki = new SerialZYYDWH();
	  ki->geometrySet( 1.020,0.480,0.645,0.,0.,0.120 );
	  ki->offsetSet(0.);
	  ki->eccentricitySet(0.);
	  return ki;
	}
	else if (name == "kuka160"){
	  SerialmZXXmZXmZ* ki = new SerialmZXXmZXmZ();
          ki->geometrySet( 0.900,0.970,1.080,0.,0.,0.180 );
	  ki->offsetSet(0.);
	  ki->eccentricitySet(0.);
	  return ki;
	}
	else{
	  ORO_CoreLib::Logger::log() << ORO_CoreLib::Logger::Error << "(KinematicsFactory) No robot kinematics available for " << name << ORO_CoreLib::Logger::endl;
	  return NULL;
	}
	  
      }
    private:
    };
}

#endif
