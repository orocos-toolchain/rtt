/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:21:19 CEST 2002  KinematicsInterface.hpp 

                        KinematicsInterface.hpp -  description
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
 

#ifndef KINEMATICSINTERFACE_HPP
#define KINEMATICSINTERFACE_HPP


#include <geometry/frames.h>
#include "KinematicsTypes.hpp"
#include "Configuration.hpp"

namespace ORO_KinDyn
{
    class Singularity;

    /**
     * This is an interface for stateless N-DOF (thus N axes robots)
     * kinematics calculations. All required data must be given as
     * method parameters. For joint data, the given argument must
     * contain the equal or more elements than the algorithm expects.
     * The same holds for the Jacobian calculations. The algorithms
     * will alter the sizes of the given parameters in return arguments
     * to indicate the number of written elements.
     *
     * Each KinematicsInterface instance works on right handed frames with all
     * joint angles positive in the positive rotation direction. 
     */
    class KinematicsInterface
    {

    public:
        virtual ~KinematicsInterface()
        {}

        /**
         * Returns the number of joints the kinematic
         * algorithms currently can handle.
         * This function can be used to see how much elements in
         * the Joint parameters are expected currently.
         */
        virtual int getNumberOfJoints() const = 0;

        /**
         * Returns the maximum number of joints the kinematic
         * algorithms can handle.
         * This function can be used to reserve enough elements in
         * the Joint parameters of the algorithms below.
         */
        virtual int maxNumberOfJoints() const = 0;

        /**
         * Calculates the forward Jacobian with given joint positions
         *
         * @param q
         *        The joint positions for which the Jacobian has to be calculated
         * @param jac
         *        The resulting Jacobian for jointpositions \a q
         * @param s
         *        The detected singularities of q
         * @return true if the the result could be calculated, false otherwise
         */
        virtual bool jacobianForward( const JointVelocities& q,
                                      Jacobian& jac, Singularity& s ) const = 0;

        /**
         * Calculates the (pseudo) inverse Jacobian
         *
         * @param q
         *        The joint positions for which the inverse Jacobian has to be calculated
         * @param jac
         *        The resulting inverse Jacobian for jointpositions \a q
         * @param s
         *        The detected singularities of q
         * @return true if the the result could be calculated, false otherwise
         */
        virtual bool jacobianInverse( const JointPositions& q, Jacobian& jac, Singularity& s ) const = 0;

        /**
         * Calculates the forward end effector position expressed in the
         * robots base frame from the current joint positions
         *
         * @param q
         *        The given joint positions
         * @param mp_base
         *        The resulting end effector frame expressed in the robot base frame
         * @param s
         *        The detected singularities of q
         * @return true if the the result could be calculated, false otherwise
         */
        virtual bool positionForward( const JointPositions& q, ORO_Geometry::Frame& mp_base, Singularity& s ) const = 0;

        /**
         * Calculates the joints positions from the current end effector
         * position expressed in the robots base frame
         *
         * @param mp_base
         *        The given end effector frame expressed in the robot base frame
	     * @param conf
	     *        The current configuration of the robot.
         * @param q
         *        The resulting joint positions
         * @param s
         *        The detected singularities of mp_base
         * @return true if the the result could be calculated, false otherwise
         */
        virtual bool positionInverse( const ORO_Geometry::Frame& mp_base, const Configuration conf, JointPositions& q, Singularity& s ) const = 0;

        /**
         * Calculates the end eff. velocity expressed in the robots
         * base frame from the joint velocities
         *
         * @param q
         *        The given joint positions
         * @param qdot
         *        The given joint velocities
         * @param pos_base
         *        The resulting end effector position with joint positions \a q,
         *        expressed in the robot base frame
         * @param vel_base
         *        The resulting end effector Twist with joint positions \a q and joint velocities \a qdot
         *        expressed in the robot base frame
         * @param s
         *        The detected singularities of q
         * @return true if the the result could be calculated, false otherwise
         */
        virtual bool velocityForward( const JointPositions& q, const JointVelocities& qdot, ORO_Geometry::Frame& pos_base, ORO_Geometry::Twist& vel_base, Singularity& s ) const = 0;

        /**
         * Calculates the joint velocities form the end eff. velocity
         * expressed in the robots base frame
         *
         * @param pos_base
         *        The given end effector position expressed in the robot base frame
         * @param vel_base
         *        The given end effector Twist expressed in the robot base frame
	     * @param conf
	     *        The current configuration of the robot.
         * @param q
         *        The resulting joint positions with end effector position \a pos_base
         * @param qdot
         *        The resulting joint velocities with end effector positions \a pos_base
         *        and end effector velocity \a vel_base
         * @param s
         *        The detected singularities of pos_base
         * @return true if the the result could be calculated, false otherwise
         */
        virtual bool velocityInverse( const ORO_Geometry::Frame& pos_base, Configuration conf, const ORO_Geometry::Twist& vel_base, JointPositions& q, JointVelocities& qdot, Singularity& s ) const = 0;

        /**
         * Calculate the joint velocities from a given end effector twist expressed in
         * the robot base frame and given the current joint positions.
         *
         * @param q
         *        The joint positions in which the inverse velocity must be calculated
         * @param vel_base
         *        The given end effector Twist expressed in the robot base frame
         * @param qdot
         *        The resulting joint velocities with joint positions \a q
         *        and end effector velocity \a vel_base
         * @param s
         *        The detected singularities of q
         * @return true if the the result could be calculated, false otherwise
         */
        virtual bool velocityInverse( const JointPositions& q, const ORO_Geometry::Twist& vel_base, JointVelocities& qdot, Singularity& s ) const = 0;

        /**
         * Calculates the joint velocities form the end eff. velocity
         * expressed in the robots base frame
         *
         * @param pos_base
         *        The given end effector position expressed in the robot base frame
         * @param vel_base
         *        The given end effector Twist expressed in the robot base frame
	     * @param conf
	     *        The current configuration of the robot.
         * @param q
         *        The resulting joint positions with end effector position \a pos_base
         * @param qdot
         *        The resulting joint velocities with end effector positions \a pos_base
         *        and end effector velocity \a vel_base
         * @param s
         *        The detected singularities of pos_base
         * @return true if the the result could be calculated, false otherwise
         * @deprecated by the above versions with more logical argument ordering.
         */
        bool velocityInverse( const ORO_Geometry::Frame& pos_base, const ORO_Geometry::Twist& vel_base, Configuration conf, JointPositions& q, JointVelocities& qdot, Singularity& s ) const {
            return this->velocityInverse(pos_base, conf, vel_base, q, qdot, s);
        }

        /**
         * We might add these later to the interface.
         virtual bool isSingular(const Frame& pos_base) const = 0;
         virtual bool isSingular(const Double6D& q) const = 0;
         virtual void singularityGet(const Frame& pos_base, Singularities& s) const = 0;
        */

        /**
         * Calculates the configuration \a c of joint positions \a q.
         *
         * @param q
         *        The joint positions of which the configuration must be known.
         * @param c
         *        The resulting configuration of q.
         */
        virtual void configurationGet(const JointPositions& q, Configuration& c) const = 0;

    };

}

#endif
