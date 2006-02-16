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
#include "Serial321.hpp"
#include <boost/shared_ptr.hpp>
#include <map>


namespace ORO_KinDyn
{
    /**
     * The KinematicsFactory can be asked to create KinematicsInterface for
     * a kind of robot/device.
     * Currently, only two robot types are supported: "Kuka361" and
     * "Kuka160". You need to change this class in order to add your
     * Robot type \b or you can use the KinematicsComponent::setKinematics()
     * function to load your own algorithm into the framework.
     *
     * See also the \a SaveDevice() and \a LoadDevice() functions
     * to load a new robot type from file. Currently, only the ZXXZXZ and ZYYDWH
     * type robots are known.
     *
     * @see KinematicsComponent::setKinematics()
     * @see KinematicProcess::getKinematics()
     * @see KinematicProcess::setAlgorithm()
     *
     */
    class KinematicsFactory
    {
    protected:
        typedef boost::shared_ptr<KinematicsFactory> KinematicsFactoryPtr;

        /**
         * You may inherit from this class and assign the subclass
         * as the instance of the factory. If no instance is defined,
         * a default one is created.
         */
        static KinematicsFactoryPtr instance;

        typedef std::map<std::string, boost::shared_ptr<Serial321> > Serial321Devices;

        /**
         * This map only stores devices of the 'Serial321' kind.
         */
        Serial321Devices serial321Devices;

        /**
         * The default factory only contains the Kuka361 and Kuka160.
         * Your subclass (or future versions) may provide more architectures.
         */
        KinematicsFactory();

        /**
         * @name Internal Serial 3-2-1 Functions
         * These functions manage kinematics creation for Serial robots.
         */
        KinematicsInterface* createSerial321Device(const std::string& name);

        bool removeSerial321Device( std::string name );

        bool addSerial321Device( std::string name, std::string kind, 
                                 double l1, double l2, double l3, double l4, double l5, double l6,
                                 double offset = 0.0, double eccentricity = 0.0);

        bool addSerial321Device( std::string name, Serial321* device);

        bool saveSerial321Device(std::string name, std::string filename);

        bool loadSerial321Device(std::string filename);
        /** @} */

        /**
         * @name Internal Selection Functions
         * These functions select the correct implementation. Override
         * them in your subclass to add different kinematic architectures.
         * These functions are called by the static variants.
         */
        virtual KinematicsInterface* create(const std::string& name);

        virtual bool saveDevice(std::string name, std::string filename);

        virtual bool loadDevice(std::string filename);

        virtual bool hasDevice( std::string name) const;
        /** @} */
    public:
        virtual ~KinematicsFactory() {}

        /**
         * This method sets a new instance of the KinematicsFactory to be
         * used by the methods below.
         */
        static void setInstance(KinematicsFactory* new_fact) { 
            instance.reset( new_fact );
        }

        /**
         * This method checks if \a instance is defined and returns it
         * or a default one.
         */
        static KinematicsFactoryPtr getInstance() { 
            if ( !instance)
                instance.reset( new KinematicsFactory() );
            return instance;
        }

        /**
         * @name Serial 3-2-1 Functions
         * Add a new kind 6DOF serial device to or remove an existing one from the database.
         * @{
         */

        /**
         * Add a device/robot to this factory. All units are in meter.
         * @param name The production name/serial number of the robot.
         * @param kind The mechanical configuration of the robot. Currently ZYYDWH and ZXXZXZ are known.
         * @param l1 until l6: length of the joints (some may be zero).
         * @param offset shoulder offset
         * @param eccentricity arm eccentricity
         * @retval true if the device could be added
         * @retval false if \a name was already used or \a kind is unknown.
         */
        static bool AddSerial321Device( std::string name, std::string kind, 
                                        double l1, double l2, double l3, double l4, double l5, double l6,
                                        double offset = 0.0, double eccentricity = 0.0) {
            using ORO_CoreLib::Logger;
            Logger::In in("KinematicsFactory::AddSerial321Device");
            bool result= getInstance()->addSerial321Device( name, kind, l1,l2,l3,l4,l5,l6, offset, eccentricity);
            if ( !result)
                Logger::log() << Logger::Error << "Could not add robot kinematics for type " << name << Logger::endl;
            return result;
        }

        /**
         * Add a device/robot to this factory. All units are in meter.
         * @param name The production name/serial number of the robot.
         * @param device A Serial321 robot architecture object.
         * Ownership of the \a device is taken by this factory.
         * @retval true if the device could be added
         * @retval false if \a name was already used or \a kind is unknown.
         */
        static bool AddSerial321Device( std::string name, Serial321* device) {
            using ORO_CoreLib::Logger;
            ORO_CoreLib::Logger::In in("KinematicsFactory::AddSerial321Device");
            bool result= getInstance()->addSerial321Device( name, device);
            if ( !result)
                Logger::log() << Logger::Error << "Could not add robot kinematics for type " << name << Logger::endl;
            return result;
        }

        static bool RemoveSerial321Device( std::string name ) {
            using ORO_CoreLib::Logger;
            ORO_CoreLib::Logger::In in("KinematicsFactory::RemoveSerial321Device");
            bool result= getInstance()->removeSerial321Device(name);
            if ( !result)
                Logger::log() << Logger::Error << "Could not remove robot kinematics for type " << name << Logger::endl;
            return result;
        }
        /**
         * @}
         */
                
        /**
         * @name Factory Functions
         * These functions operate on any robot/device type
         * known to this factory.
         * @{
         */

        /**
         * Create a KinematicsInterface instance for a given device/robot.
         *
         * @param name The production name/serial number of the device.
         * @return A KinematicInterface instance or zero if the device is not found.
         */
        static KinematicsInterface* Create(const std::string& name) {
            using ORO_CoreLib::Logger;
            Logger::In in("KinematicsFactory::Create");
            if (!HasDevice(name) )
                Logger::log() << Logger::Error << "Unknown device: " << name << Logger::endl;
            KinematicsInterface* result = getInstance()->create(name);
            if ( !result)
                Logger::log() << Logger::Error << "No robot kinematics available for " << name << Logger::endl;
            return result;
        }

        /**
         * Returns true if such a device can be created.
         */
        static bool HasDevice(std::string name) {
            return getInstance()->hasDevice(name);
        }

        /**
         * Save an added device's parameters to an XML file.
         */
        static bool SaveDevice(std::string name, std::string filename) {
            using ORO_CoreLib::Logger;
            Logger::In in("KinematicsFactory::SaveDevice");
            if (!HasDevice(name) )
                Logger::log() << Logger::Error << "Unknown device: " << name << Logger::endl;
            bool result= getInstance()->saveDevice(name, filename);
            if ( !result)
                Logger::log() << Logger::Error << "Could not save properties of " << name << Logger::endl;
            return result;
        }

        /**
         * Load a new device from an XML file.
         * Warning: this may override an older definition but already
         * created KinematicInterface objects are not altered !
         */
        static bool LoadDevice(std::string filename) {
            using ORO_CoreLib::Logger;
            Logger::In in("KinematicsFactory::LoadDevice");
            bool result= getInstance()->loadDevice(filename);
            if ( !result)
                Logger::log() << Logger::Error << "Could not load properties from " << filename << Logger::endl;
            return result;
        }
        //! @}
    };
}

#endif
