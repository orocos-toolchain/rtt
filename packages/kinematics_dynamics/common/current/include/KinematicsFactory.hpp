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

#include <math.h>

#include <corelib/NameServerRegistrator.hpp>
#include "KinematicsInterface.hpp"


namespace ORO_KinDyn
{
    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::NameServerRegistrator;

    /**
     * This factory-stub creates the kinematics library for one
     * specific robot. You need to implement the create method
     * for your kind of robot in a sub class.
     */
    class KinematicsFactoryStub
        :public NameServerRegistrator<KinematicsFactoryStub*>
    {
    public:
        /**
         * Create a factory-stub with a given name.
         */
        KinematicsFactoryStub(const std::string& name)
            :NameServerRegistrator<KinematicsFactoryStub*>(nameserver, name, this)
        {}

        KinematicsFactoryStub()
        {}
    
        virtual ~KinematicsFactoryStub()
        {}

        /**
         * Create an instance of the KinematicsInterface for kinematic
         * calculations for the robot
         *
         * @return An instance of a KinematicsInterface which you get ownership of,
         *         so you need to delete it afterwards.
         */
        virtual KinematicsInterface* create() = 0;


        static NameServer<KinematicsFactoryStub*> nameserver;
    };

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
            KinematicsFactoryStub* ki = KinematicsFactoryStub::nameserver.getObject(name);
            if (ki) 
                return ki->create();
            return 0;
        }
    private:
    };
}

#endif
