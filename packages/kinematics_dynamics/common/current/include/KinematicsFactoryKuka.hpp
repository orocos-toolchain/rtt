/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  KinematicsFactoryKuka.hpp 

                        KinematicsFactoryKuka.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef KINEMATICSFACTORYKUKA_HPP
#define KINEMATICSFACTORYKUKA_HPP

#include "KinematicsFactory.hpp"
#include "SerialZYYDWH.hpp"

namespace ORO_KinDyn
{

    /**
     * Factory for creating KUKA kinematics objects.
     */
    class KinematicsFactoryKuka
        : public KinematicsFactoryStub
    {
    public:
        /**
         * Constructor
         */
        KinematicsFactoryKuka()
            :KinematicsFactoryStub("Kuka361")
        {
        }

        virtual ~KinematicsFactoryKuka() {}

        /**
         * Factory method
         * 
         * @return The newly created KUKA kinematics object
         */
        virtual KinematicsInterface* create()
        {
            SerialZYYDWH* ki = new SerialZYYDWH();
            ki->geometrySet( 1.020,0.480,0.645,0.,0.,0.120 );
            ki->offsetSet(0.);
            ki->eccentricitySet(0.);
            return ki;
        }
    
    };


}



#endif
