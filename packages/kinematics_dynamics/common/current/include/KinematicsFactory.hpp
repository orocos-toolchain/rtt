
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
            KinematicsFactoryStub* ki = KinematicsFactoryStub::nameserver.getObjectByName(name);
            if (ki) 
                return ki->create();
            return 0;
        }
    private:
    };
}

#endif
