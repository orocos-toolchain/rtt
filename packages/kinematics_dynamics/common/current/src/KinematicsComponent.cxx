/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:33 CEST 2004  KinematicsComponent.cxx 

                        KinematicsComponent.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "kindyn/KinematicsComponent.hpp"
#include "kindyn/KinematicsJointConverter.hpp"

namespace ORO_KinDyn
{
    using namespace ORO_Geometry;
    using namespace ORO_CoreLib;

    KinematicsComponent::KinematicsComponent( KinematicsInterface* stateless, 
                                              JointVelocities _signs /* = JointVelocities() */, 
                                              JointPositions  _offsets /* = JointPositions()*/ )
        : kine(0),
          deltaX(0.5)
    {
        this->setKinematics(stateless, _signs, _offsets);
    }
  
    KinematicsComponent::~KinematicsComponent()
    {
    }

    void KinematicsComponent::setKinematics( KinematicsInterface *k,
                                             const JointVelocities& _signs, 
                                             const JointPositions& _offsets )
    {
        delete kine;
        kine = k;
        if ( kine == 0 ) 
            return;
        mjoints = kine->maxNumberOfJoints();
        // if sizes are correct, nothing happens.
        jointDirection.resize( mjoints, 1.0);
        jointVel.resize( mjoints, 0.0);
        jointState.resize( mjoints, 0.0 );
        jointDirection.resize( mjoints, 1.0);
        jointOffset.resize( mjoints, 0.0);
        if (_signs.size() != 0 || _offsets.size() != 0) {
            kine = new KinematicsJointConverter( kine, _signs, _offsets );
        }
        kine = k;
    }

    bool KinematicsComponent::jacobianForward( const JointPositions& q, Jacobian& j )
    {
        if ( kine == 0 )
            return false;
        jointState = q;
        return kine->jacobianForward(q,j, sings);
    }
  
    bool KinematicsComponent::jacobianInverse( const JointPositions& q, Jacobian& j )
    {
        if ( kine == 0 )
            return false;
        jointState = q;
        return kine->jacobianInverse(q, j, sings);
    }
    bool KinematicsComponent::positionForward( const JointPositions& q, ORO_Geometry::Frame& mp_base )
    {
        if ( kine == 0 )
            return false;
        jointState = q;
        return kine->positionForward(q,mp_base,sings);
    }
  
    bool KinematicsComponent::positionInverse( const ORO_Geometry::Frame& mp_base, JointPositions& q )
    {
        if ( kine == 0 )
            return false;
        bool res =  kine->positionInverse(mp_base, configState, q, sings);
        if ( !res || !adaptJoints(q) ) return false;
        return res;
    }
  
    bool KinematicsComponent::velocityInverse( const ORO_Geometry::Frame& pos_base, const ORO_Geometry::Twist& vel_base, JointPositions& q, JointVelocities& qdot )
    {
        if ( kine == 0 )
            return false;
        bool res = kine->velocityInverse(pos_base, configState, vel_base, q, qdot,sings);
        if ( !res || !adaptJoints(q) ) return false;
        return res;
    }

    bool KinematicsComponent::velocityInverse( const JointPositions& q, const ORO_Geometry::Twist& vel_base, JointVelocities& qdot )
    {
        if ( kine == 0 )
            return false;
        jointState = q;
        return kine->velocityInverse(q, vel_base,qdot,sings);	    
    }

    bool KinematicsComponent::velocityForward( const JointPositions& q, const JointVelocities& qdot, ORO_Geometry::Frame& pos_base, ORO_Geometry::Twist& vel_base )
    {
        if ( kine == 0 )
            return false;
        jointState = q;
        return kine->velocityForward( q, qdot, pos_base, vel_base, sings);
    }

    bool KinematicsComponent::stateSet(const JointPositions& q_new)
    {
        return this->setPosition(q_new);
    }

    bool KinematicsComponent::stateSet(const ORO_Geometry::Frame& f, Configuration c)
    {
        return this->setPosition(f,c);
    }

    bool KinematicsComponent::setPosition(const JointPositions& q_new)
    {
        if ( kine == 0 || jointState.size() != q_new.size() )
            return false;
        jointState = q_new;
        kine->configurationGet(jointState, configState);
        return true;
    }

    bool KinematicsComponent::setPosition(const ORO_Geometry::Frame& f, Configuration c)
    {
        if ( kine == 0 )
            return false;
        bool res = kine->positionInverse(f,c,jointState,sings);
        if ( !res )
            return false;
        configurationSet(c);
        return true;
    }

    bool KinematicsComponent::getPosition(JointPositions& q) const
    {
        if ( q.size() != jointState.size() )
            return false;
        q = jointState;
        return true;
    }

    bool KinematicsComponent::jointsGet(JointPositions& q) const
    {
        return this->getPosition( q );
    }

    Frame KinematicsComponent::getFrame() const {
        Frame eff;
        Singularity tmps;
        if (kine == 0 || false ==  kine->positionForward( jointState, eff, tmps ) )
            return Frame::Identity();
        return eff;
    }
   
    Twist KinematicsComponent::getTwist() const {
        Frame eff;
        Twist tw;
        Singularity tmps;
        if ( kine == 0 || false == kine->velocityForward( jointState, jointVel, eff, tw, tmps ) ) {
            return Twist::Zero();
        }
        return tw;
    }

    bool KinematicsComponent::setVelocity(const JointVelocities& qdot ) {
        if ( qdot.size() != jointVel.size() )
            return false;
        jointVel = qdot;
        return true;
    }

    bool KinematicsComponent::setVelocity(const Twist& t ) {
        if ( kine == 0 )
            return false;
        return kine->velocityInverse(jointState, t, jointVel, sings);
    }
   
    bool KinematicsComponent::adaptJoints( JointPositions& qtheoretic)
    {
        for (int i=0; i< mjoints; ++i)
            {
                double delta = qtheoretic[i] + jointOffset[i] - jointState[i];
                // detect delta > deltaX, which means to large for nominal movement
                if ( fabs(delta) > deltaX )
                    {   // did we go over the pivot point ?
                        if ( fabs( delta + jointDirection[i] * 2*M_PI ) > deltaX)
                            {
                                if ( fabs(delta - jointDirection[i] * 2* M_PI ) < deltaX )
                                    { // did the direction change over pivot point ?
                                        jointDirection[i] = -jointDirection[i];
                                        jointOffset[i] += jointDirection[i] * 2 * M_PI;
                                    }
                                else
                                    { return false; } // error, impossible !
                            }
                        else
                            {   // windup
                                jointOffset[i] += jointDirection[i] * 2 * M_PI;
                            }
                    } else
                        {   // normal, detect direction change
                            if ( delta * jointDirection[i] < 0 )
                                jointDirection[i] = - jointDirection[i];
                        }
                qtheoretic[i] += jointOffset[i];
                jointState[i] = qtheoretic[i];
            }
        return true;
    }

}
