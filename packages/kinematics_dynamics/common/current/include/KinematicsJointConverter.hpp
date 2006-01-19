/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  KinematicsJointConverter.hpp 

                        KinematicsJointConverter.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#ifndef ORO_KINDYN_KINEMATICS_JOINT_CONVERTER_HPP
#define ORO_KINDYN_KINEMATICS_JOINT_CONVERTER_HPP

#include "KinematicsInterface.hpp"

namespace ORO_KinDyn
{
    /**
     * This class wraps a KinematicsInterface to another
     * Axis convention for both signs and origins on the
     * joint level.
     * @verbatim
     q_robot = Sign * q_kine + Offset
     qdot_robot = Sign * qdot_kine
     @verbatim
     * where \a q_kine and \a qdot_kine are the joint position
     * and joint velocities the kinematic algorithm expects and
     * q_robot and qdot_robot are the actual robot positions and
     * velocities.
     */
    class KinematicsJointConverter
        : public KinematicsInterface
    {
      /**
       * The stateless kinlib we use
       */
      KinematicsInterface* kine;
      
      /**
       * The windup of each joint
       */
      JointPositions jointOffset;
      
      /**
       * The direction the joint is going in
       */
      JointVelocities jointDirection;
      
      /**
       * The signs the joints deviate from the right-turning standard.
       */
      JointVelocities jointSign;
      
      /**
       * Temporary joint position storage
       */
      mutable JointPositions jointPos;
      
      /**
       * Temporary joint velocity storage
       */
      mutable JointVelocities jointVel;
      
      /**
       * Temporty jacobian storage
       */
      mutable Jacobian jacForw;
      mutable Jacobian jacInv;
      
      /**
       * The number of joints.
       */
      int mjoints;
      
      void toRobot(const JointPositions& from, JointPositions& to) const
      {
	for(int i=0; i < mjoints; ++i)
	  to[i] = jointSign[i]*from[i] + jointOffset[i];
      }
      
      void toKine(const JointPositions& from, JointPositions& to) const
      {
	for(int i=0; i < mjoints; ++i)
	  to[i] = jointSign[i]*(from[i] - jointOffset[i]);
      }
      void toRobotdot(const JointVelocities& from, JointVelocities& to) const
      {
	for(int i=0; i < mjoints; ++i)
	  to[i] = jointSign[i]*from[i];
      }
      
      void toKinedot(const JointVelocities& from, JointVelocities& to) const
      {
	for(int i=0; i < mjoints; ++i)
	  to[i] = jointSign[i]*(from[i]);
      }
      void toRobotdot(const JointPositions& from, JointPositions& to,const JointVelocities& vfrom, JointVelocities& vto) const
      {
	for(int i=0; i < mjoints; ++i) {
	  to[i] = jointSign[i]*from[i] - jointOffset[i];
	  vto[i] = jointSign[i]*vfrom[i];
	}
      }
      
      void toKinedot(const JointPositions& from, JointPositions& to,const JointVelocities& vfrom, JointVelocities& vto) const
      {
	for(int i=0; i < mjoints; ++i) {
	  to[i] = jointSign[i]*(from[i] - jointOffset[i]);
	  vto[i] = jointSign[i]*(vfrom[i]);
	}
      }
      void toRobotJac(const Jacobian& from, Jacobian& to) const
      {
	for(int i=0; i < 6; i++) {
	  for(int j=0; j < mjoints; j++)
	    to(i,j) = jointSign[j]*from(i,j);
	}
      }
      void toKineJac(const Jacobian& from, Jacobian& to) const
      {
	for(int i=0; i < mjoints; i++) {
	  for(int j=0; j < 6; j++)
	    to(i,j) = jointSign[i]*from(i,j);
	}
      }
      
    public:
        KinematicsJointConverter(KinematicsInterface* original,
                                 const JointVelocities& _signs, 
                                 const JointPositions& _offsets )
            : kine(0)
        {
            this->setKinematics(original, _signs, _offsets);
        }

        ~KinematicsJointConverter() {
            delete kine;
        }

        void KinematicsJointConverter::setKinematics( KinematicsInterface *k,
                                                      const JointVelocities& _signs, 
                                                      const JointPositions& _offsets )
        {
            delete kine;
            kine = k;
            jointSign = _signs;
            jointOffset = _offsets;
            mjoints = k->maxNumberOfJoints();
            // if sizes are correct, nothing happens.
            jointSign.resize( mjoints, 1.0 );
            jointOffset.resize( mjoints, 0.0 );
            jointPos.resize( mjoints, 0.0 );
            jointVel.resize( mjoints, 0.0 );
	    jacForw.resize( 6 ,mjoints);
	    jacInv.resize( mjoints , 6);
	}

        virtual int getNumberOfJoints() const { return kine->getNumberOfJoints(); }
        virtual int maxNumberOfJoints() const { return kine->getNumberOfJoints(); }

        virtual bool jacobianForward( const JointVelocities& q,
                                      Jacobian& jac, Singularity& s ) const {
            this->toKine( q, jointPos );
            bool res = kine->jacobianForward( jointPos, jacForw, s);
	    this->toRobotJac( jacForw, jac);
	    if (!res)
	      return false;
	    return true;
	}
        
        virtual bool jacobianInverse( const JointPositions& q, Jacobian& jac, Singularity& s ) const {
            this->toKine( q, jointPos );
            bool res = kine->jacobianInverse( jointPos, jacInv, s);
	    this->toKineJac( jacInv, jac);
	    if (!res)
	      return false;
	    return true;
	}

        virtual bool positionForward( const JointPositions& q, ORO_Geometry::Frame& mp_base, Singularity& s ) const {
	  this->toKine(q, jointPos);
	  return kine->positionForward( jointPos, mp_base, s);
        }

        virtual bool positionInverse( const ORO_Geometry::Frame& mp_base, const Configuration conf, JointPositions& q, Singularity& s ) const {
            bool res = kine->positionInverse( mp_base, conf, jointPos, s);
            this->toRobot(jointPos, q);
            if (!res)
                return false;
            return true;
        }

        virtual bool velocityForward( const JointPositions& q, const JointVelocities& qdot, ORO_Geometry::Frame& pos_base, ORO_Geometry::Twist& vel_base, Singularity& s ) const {
            this->toKinedot(q, jointPos, qdot, jointVel);
            return kine->velocityForward( jointPos, jointVel, pos_base, vel_base, s);
        }

        virtual bool velocityInverse( const ORO_Geometry::Frame& pos_base, const Configuration conf, const ORO_Geometry::Twist& vel_base, JointPositions& q, JointVelocities& qdot, Singularity& s ) const {
            bool res = kine->velocityInverse( pos_base, conf, vel_base, jointPos, jointVel, s);
            this->toRobotdot(jointPos, q, jointVel, qdot);
	    if (!res)
                return false;
	    return true;
        }

        virtual bool velocityInverse( const JointPositions& q, const ORO_Geometry::Twist& vel_base, JointVelocities& qdot, Singularity& s ) const {
            this->toKine( q, jointPos);
            bool res = kine->velocityInverse( jointPos, vel_base, jointVel, s);
	    this->toRobotdot(jointVel, qdot);
            if (!res)
                return false;
            return true;
        }

        virtual void configurationGet(const JointPositions& q, Configuration& c) const {
            this->toKine( q, jointPos);
            this->configurationGet( jointPos, c);
        }

    };
}

#endif
