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
#include <corelib/MultiVector.hpp>
#include "Configuration.hpp"

namespace ORO_KinDyn
{
    using ORO_CoreLib::Double6D;

    class Configuration;
    class Singularity;

    /**
     * This is an interface for stateless kinematics calculations. All required 
     * data must be given as method parameters.
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
             * Calculates the forward Jacobian with given joint positions
             *
             * @param q
             *        The joint positions for which the Jacobian has to be calculated
             * @param J
             *        The resulting Jacobian for jointpositions \a q
             * @param s
             *        The detected singularities of q
             * @return true if the the result could be calculated, false otherwise
             */
            virtual bool jacobianForward( const Double6D& q, double J[ 6 ][ 6 ], Singularity& s ) const = 0;

            /**
             * Calculates the inverse Jacobian
             *
             * @param q
             *        The joint positions for which the inverse Jacobian has to be calculated
             * @param J
             *        The resulting inverse Jacobian for jointpositions \a q
             * @param s
             *        The detected singularities of q
             * @return true if the the result could be calculated, false otherwise
             */
            virtual bool jacobianInverse( const Double6D& q, double J[ 6 ][ 6 ], Singularity& s ) const = 0;

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
            virtual bool positionForward( const Double6D& q, ORO_Geometry::Frame& mp_base, Singularity& s ) const = 0;

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
            virtual bool positionInverse( const ORO_Geometry::Frame& mp_base, const Configuration conf, Double6D& q, Singularity& s ) const = 0;

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
            virtual bool velocityInverse( const ORO_Geometry::Frame& pos_base, const ORO_Geometry::Twist& vel_base, const Configuration conf, Double6D& q, Double6D& qdot, Singularity& s ) const = 0;

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
            virtual bool velocityInverse( const Double6D& q, const ORO_Geometry::Twist& vel_base, Double6D& qdot, Singularity& s ) const = 0;
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
            virtual bool velocityForward( const Double6D& q, const Double6D& qdot, ORO_Geometry::Frame& pos_base, ORO_Geometry::Twist& vel_base, Singularity& s ) const = 0;

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
        virtual void configurationGet(const Double6D& q, Configuration& c) const = 0;

    };

}

#endif
