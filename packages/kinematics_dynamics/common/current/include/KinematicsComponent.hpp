/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:21:19 CEST 2002  KinematicsComponent.hpp 

                        KinematicsComponent.hpp -  description
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
 
#ifndef KINEMATICSCOMPONENT_HPP
#define KINEMATICSCOMPONENT_HPP

#include "KinematicsInterface.hpp"
#include "Configuration.hpp"
#include "Singularity.hpp"
#include <corelib/MultiVector.hpp>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace ORO_KinDyn
{
    /**
     * The KinematicsComponent is meant for following and calculating
     * the current end effector position and direction the joints are
     * moving in.
     *
     * This Component uses a stateless KinematicsInterface instance
     * purely for the calculations and keeps track of kinematic states
     * between two calls. The state includes robot configuration,
     * joint positions and singularities.
     *
     * Consecutive calculations which turn out valid (return true)
     * will always update the internal state of the component. This
     * class can track changes no larger than 0.5 radians, or about 30
     * degrees. You could raise this number to a bit below pi (or 180
     * degrees), but larger deviations than this default mostly denote
     * an error.
     *
     * @see KinematicsInterface
     */
    class KinematicsComponent
    {
    public:
        /**
         * Create a statefull KinematicsComponent.
         *
         * @param stateless
         *        The KinematicsInterface instance to use for the calculations.
         * @param _signs Change the signs if the joints are not right-positive-turning
         * @param _offsets Change the offsets if your zero differs from what the algorithm expects.
         */
        KinematicsComponent( KinematicsInterface* stateless, 
                             JointVelocities _signs = JointVelocities(), 
                             JointPositions  _offsets = JointPositions() );
  
        /**
         * Destroy a Kinematics Component.
         */
        ~KinematicsComponent();

        /**
         * Set a new KinematicsInterface to use for the calculations.
         * The last two arguments are optional but should preferably
         * set explict for all \a kin->getNumberOfJoints(). The defaults are 
         * 1.0 for the \a _signs and 0.0 for the \a _offsets.
         * @param kin
         *        The KinematicsInterface instance to use for the calculations.
         * @param _signs Change the signs if the engines are not right-positive-turning
         * @param _offsets Change the offsets if your zero is differs from what the algorithm expects.
         */
        void setKinematics( KinematicsInterface *kin,
                            const JointVelocities& _signs = JointVelocities(), 
                            const JointPositions&  _offsets = JointPositions() );

        /**
         * Change the delta radial movement this class can track inbetween
         * calls to its interface. Defaults to 0.5 radians.
         * @param dx In radians, strictly positive, keep below M_PI.
         * @return true if acceptable value, false otherwise.
         */
        bool setDelta(double dx) {
            if (deltaX >=M_PI || deltaX < 0.0) return false;
            deltaX = dx;
            return true;
        }

        /**
         * Get the delta radial movement this class can track inbetween
         * calls to its interface. Defaults to 0.5 radians.
         */
        double getDelta() const {
            return deltaX;
        }

        /**
         * Calculate the forward Jacobian and update current joint positions.
         *
         * @param q
         *        The current joint positions
         * @param jac
         *        The resulting Jacobian. Allocate the memory for this object yourself.
         * @return true if it could be calculated, false otherwise (check for singularities).
         * @post The internal joint state is updated if it returned true
         */
        bool jacobianForward( const JointPositions& q, Jacobian& jac );
  
        /**
         * Calculate the forward Jacobian from current joint positions.
         *
         * @param jac
         *        The resulting Jacobian. Allocate the memory for this object yourself.
         * @return true if it could be calculated, false otherwise (check for singularities).
         */
        bool jacobianForward( Jacobian& jac );
  
        /**
         * Calculate the (pseudo) inverse Jacobian and update joint positions.
         *
         * @param q
         *        The current joint positions
         * @param jac
         *        The resulting inverse Jacobian. Allocate the memory for this object yourself.
         * @return true if it could be calculated, false otherwise (check for singularities).
         * @post The internal joint state is updated if it returned true
         */
        bool jacobianInverse( const JointPositions& q, Jacobian& jac );

        /**
         * Calculate the (pseudo) inverse Jacobian at current joint positions.
         *
         * @param jac
         *        The resulting inverse Jacobian. Allocate the memory for this object yourself.
         * @return true if it could be calculated, false otherwise (check for singularities).
         */
        bool jacobianInverse( Jacobian& jac );

        /**
         * Calculate the current end frame given the joint positions.
         * 
         * @param q
         *        The current joint positions.
         * @param mp_base
         *        The resulting end frame expressed in the robot base frame.
         * @return true if it could be calculated, false otherwise (check for singularities).
         * @post The internal joint state is updated if it returned true
         * @see getFrame() for current end frame.
         */
        bool positionForward( const JointPositions& q, ORO_Geometry::Frame& mp_base );
  
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
        bool positionInverse( const ORO_Geometry::Frame& mp_base, JointPositions& q );
  
        /**
         * Calculate the current end frame position and velocity given
         * the joint positions and velocities, updates the internal joint status.
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
         * @see getVelocity() to get current velocity.
         */
        bool velocityForward( const JointPositions& q, const JointVelocities& qdot, ORO_Geometry::Frame& pos_base, ORO_Geometry::Twist& vel_base );

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
        bool velocityInverse( const ORO_Geometry::Frame& pos_base, const ORO_Geometry::Twist& vel_base, JointPositions& q, JointVelocities& qdot );

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
        bool velocityInverse( const JointPositions& q, const ORO_Geometry::Twist& vel_base, JointVelocities& qdot );

        /**
         * Set the current joint positions for future estimations
         * The configuration represented by these joints will be used 
         * in inverse calculations.
         * @deprecated by setPosition()
         */
        bool stateSet(const JointPositions& q_new);

        /**
         * Set the current joint positions according to a frame and a configuration.
         *
         * @param f
         *        The end frame.
         * @param c
         *        The configuration of the robot.
         * @deprecated by setPosition()
         */
        bool stateSet(const ORO_Geometry::Frame& f, Configuration c);

        /**
         * Set the current joint positions for future estimations
         * The configuration represented by these joints will be used 
         * in inverse calculations.
         * @return false if the number of joints is incorrect.
         */
        bool setPosition(const JointPositions& q_new);

        /**
         * Set the current joint positions according to a frame and a configuration.
         *
         * @param f
         *        The end frame.
         * @param c
         *        The configuration of the robot.
         * @return false if the robot is in a singularity or no kinematic algorithm set.
         */
        bool setPosition(const ORO_Geometry::Frame& f, Configuration c);

        /**
         * Read the current, estimated joint positions.
         */
        bool getPosition(JointPositions& q) const;

        /**
         * Set the current joint velocities for future estimations.
         * @param qdot the velocities of each joint. Must match
         * the number of joints.
         * @return false if the number of joints is incorrect.
         */
        bool setVelocity(const JointVelocities& qdot);

        /**
         * Set the current joint velocities to a twist at the current position.
         *
         * @param t
         *        The end frame's velocity at the current position.
         * @return false if the robot is in a singularity or no kinematic algorithm set.
         */
        bool setVelocity(const ORO_Geometry::Twist& t);

        /**
         * Read the current, estimated joint positions.
         * @return false if the number of joints is incorrect.
         */
        bool getVelocity(JointVelocities& qdot) const;

        /**
         * Read the current, estimated joint positions.
         * @return false if the number of joints is incorrect.
         * @deprecated by getPosition()
         */
        bool jointsGet(JointPositions& q) const;

        /**
         * Get the current end frame/mounting plate positions.
         * @return The identity frame if no kinematic algorithm present
         */
        ORO_Geometry::Frame getFrame() const;

        /**
         * Get the current velocity of the mounting plate.
         * @return The zero twist if no velocity information or kinematic algorithm present.
         */
        ORO_Geometry::Twist getTwist() const;

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
        bool adaptJoints( JointPositions& qtheoretic);

        /**
         * The stateless kinlib we use
         */
        KinematicsInterface* kine;
  
        /**
         * The current joint positions
         */
        JointPositions jointState;

        /**
         * Since the kine algorithms only go from -PI to +PI, we need to
         * adapt for winding.
         */
        JointPositions jointOffset;

        /**
         * The velocity of each joint.
         */
        JointVelocities jointVel;

        /**
         * The direction the joint is going in
         */
        JointVelocities jointDirection;

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

        /**
         * The number of joints.
         */
        int mjoints;
    };
}

#endif
