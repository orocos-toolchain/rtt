/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:21:19 CEST 2002  KinematicsComponent.hpp 

                        KinematicsComponent.hpp -  description
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
 
 
#ifndef KINEMATICSCOMPONENT_HPP
#define KINEMATICSCOMPONENT_HPP

#include "KinematicsInterface.hpp"
#include "Configuration.hpp"
#include "Singularity.hpp"
#include <corelib/MultiVector.hpp>

namespace ORO_KinDyn
{
    using ORO_CoreLib::Double6D;

    /**
     *  The KinematicsComponent is meant for following and calculating the current
     *  end effector position and direction the joints are moving in.
     *
     *  This Component uses a stateless KinematicsInterface instance purely for the calculations and keeps
     *  track of kinematic states between two calls. The state includes
     *  robot configuration, joint positions and singularities.
     *
     *  Consecutive calculations which turn out valid (return true) will always
     *  update the internal state of the component.
     */
    class KinematicsComponent
    {
    public:
        /**
         * Create a statefull KinematicsComponent.
         *
         * @param stateless
         *        The KinematicsInterface instance to use for the calculations.
         * @param _signs Change the signs if the engines are not right-positive-turning
         * @param _offsets Change the offsets if your zero is not 
         */
        KinematicsComponent( KinematicsInterface* stateless, 
                             Double6D _signs = Double6D(1.0), 
                             Double6D _offsets = Double6D(0) )
            :kine(stateless), jointDirection(1), deltaX(0.5), 
             signs(_signs), offsets(_offsets)
        {
        }
  
        /**
         * Destroy a Kinematics Component.
         */
        virtual ~KinematicsComponent()
        {
        }
        
        /**
         * Set a new KinematicsInterface to use for the calculations.
         * @param kin
         *        The KinematicsInterface instance to use for the calculations.
         * @param _signs Change the signs if the engines are not right-positive-turning
         * @param _offsets Change the offsets if your zero is not 
         */
        void setKinematics( KinematicsInterface *kin,
                            Double6D _signs = Double6D(1.0), 
                            Double6D _offsets = Double6D(0) );
 
        /**
         * Calculate the forward Jacobian.
         *
         * @param q
         *        The current joint positions
         * @param J
         *        The resulting Jacobian. Allocate the memory for this object yourself.
         * @return true if it could be calculated, false otherwise (check for singularities).
         * @post The internal joint state is updated if it returned true
         */
        virtual bool jacobianForward( const Double6D& q, double J[ 6 ][ 6 ] );
  
        /**
         * Calculate the inverse Jacobian.
         *
         * @param q
         *        The current joint positions
         * @param J
         *        The resulting inverse Jacobian. Allocate the memory for this object yourself.
         * @return true if it could be calculated, false otherwise (check for singularities).
         * @post The internal joint state is updated if it returned true
         */
        virtual bool jacobianInverse( const Double6D& q, double J[ 6 ][ 6 ] );

        /**
         * Calculate the current end frame given the joint positions.
         * 
         * @param q
         *        The current joint positions.
         * @param mp_base
         *        The resulting end frame expressed in the robot base frame.
         * @return true if it could be calculated, false otherwise (check for singularities).
         * @post The internal joint state is updated if it returned true
         */
        virtual bool positionForward( const Double6D& q, ORO_Geometry::Frame& mp_base );
  
        /**
         * Calculate the current joint positions given the end frame.
         * 
         * @param mp_base
         *        The current end frame expressed in the robot base frame.
         * @param q
         *        The resulting joint positions.
         * @return true if it could be calculated, false otherwise (check for singularities).
         * @post The internal joint state is updated if it returned true
         */
        virtual bool positionInverse( const ORO_Geometry::Frame& mp_base, Double6D& q );
  
        /**
         * Calculate the current joint positions and velocities given the end frame position and velocity
         * expressed in the robot base frame.
         * 
         * @param pos_base
         *        The current end frame position expressed in the robot base frame.
         * @param vel_base
         *        The current end frame velocity expressed in the robot base frame.
         * @param q
         *        The resulting joint positions.
         * @param qdot
         *        The resulting joint velocities.
         * @return true if it could be calculated, false otherwise (check for singularities).
         * @post The internal joint state is updated if it returned true
         */
        virtual bool velocityInverse( const ORO_Geometry::Frame& pos_base, const ORO_Geometry::Twist& vel_base, Double6D& q, Double6D& qdot );

        /**
         * Calculate the current joint velocities given the end frame velocity
         * expressed in the robot base frame and the joint positions.
         * 
         * @param q
         *        The current joint positions.
         * @param vel_base
         *        The current end frame velocity expressed in the robot base frame.
         * @param qdot
         *        The resulting joint velocities.
         * @return true if it could be calculated, false otherwise (check for singularities).
         * @post The internal joint state is updated if it returned true
         */
        virtual bool velocityInverse( const Double6D& q, const ORO_Geometry::Twist& vel_base, Double6D& qdot );

        /**
         * Calculate the current end frame position and velocity given the joint positions and velocities.
         * 
         * @param q
         *        The current joint positions.
         * @param qdot
         *        The current joint velocities.
         * @param pos_base
         *        The resulting end frame expressed in the robot base frame.
         * @param vel_base
         *        The resulting end frame velocity expressed in the robot base frame.
         * @return true if it could be calculated, false otherwise (check for singularities).
         * @post The internal joint state is updated if it returned true
         */
        virtual bool velocityForward( const Double6D& q, const Double6D& qdot, ORO_Geometry::Frame& pos_base, ORO_Geometry::Twist& vel_base );

        /**
         * Set the current joint positions for future estimations
         * The configuration represented by these joints will be used 
         * in inverse calculations
         */
        void stateSet(const Double6D& q_new);

        /**
         * Set the current joint positions according to a frame and a configuration.
         *
         * @param f
         *        The end frame.
         * @param c
         *        The configuration of the robot.
         */
        void stateSet(const ORO_Geometry::Frame& f, Configuration c);

        /**
         * Read the current, estimated joint positions.
         */
        void jointsGet(Double6D& q) const;
   
        /**
         * Change to a new configuration.
         *
         * @param conf
         *        The new configuration to be used in the inverse calculations.
         */
        void configurationSet(const Configuration& conf)
        {
            configState = conf;
        }

        /**
         * Retrieve the Configuration the robot is in.
         *
         * @return The current Configuration
         */
        Configuration& configurationGet()
        {
            return configState;
        }
  
    protected:
        KinematicsComponent() 
        {
        }

        /**
         * Adapts the theoretic joint values to be as close as
         * possible to the actual jointState
         */
        bool adaptJoints( Double6D& qtheoretic);

        /**
         * The stateless kinlib we use
         */
        KinematicsInterface* kine;
  
        /**
         * The current joint positions
         */
        Double6D jointState;

        /**
         * The windup of each joint
         */
        Double6D jointOffset;

        /**
         * The direction the joint is going in
         */
        Int6D jointDirection;

        /**
         * Contains the current configuration the robot has
         */
        Configuration configState;

        /**
         * Contains any singularity which the robot is currently in
         */
        Singularity sings;

        /**
         * The maximum distance between two consecutive joint angles.
         */
        double deltaX;

        Double6D signs;
        Double6D offsets;
    };
}

#endif
