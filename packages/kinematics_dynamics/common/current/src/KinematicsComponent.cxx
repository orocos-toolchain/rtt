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
#include "kindyn/KinematicsComponent.hpp"

namespace ORO_KinDyn
{

    void KinematicsComponent::setKinematics( KinematicsInterface *k,
                                             Double6D _signs, 
                                             Double6D _offsets )
    {
        kine = k;
        signs = _signs;
        offsets = _offsets;
    }

    bool KinematicsComponent::jacobianForward( const Double6D& q, double J[ 6 ][ 6 ] )
    {
        jointState = q;
        return kine->jacobianForward(q,J, sings);
    }
  
    bool KinematicsComponent::jacobianInverse( const Double6D& q, double J[ 6 ][ 6 ] )
    {
        jointState = q;
        return kine->jacobianInverse(q, J, sings);
    }
    bool KinematicsComponent::positionForward( const Double6D& q, ORO_Geometry::Frame& mp_base )
    {
        jointState = q;
        return kine->positionForward(q,mp_base,sings);
    }
  
    bool KinematicsComponent::positionInverse( const ORO_Geometry::Frame& mp_base, Double6D& q )
    {
        bool res =  kine->positionInverse(mp_base, configState, q, sings);
        if ( !adaptJoints(q) ) return false;
        return res;
    }
  
    bool KinematicsComponent::velocityInverse( const ORO_Geometry::Frame& pos_base, const ORO_Geometry::Twist& vel_base, Double6D& q, Double6D& qdot )
    {
        bool res= kine->velocityInverse(pos_base, vel_base, configState, q, qdot,sings);
        if (! adaptJoints(q) ) return false;
        return res;
    }

    bool KinematicsComponent::velocityInverse( const Double6D& q, const ORO_Geometry::Twist& vel_base, Double6D& qdot )
    {
        jointState = q;
        return kine->velocityInverse(q, vel_base,qdot,sings);	    
    }

    bool KinematicsComponent::velocityForward( const Double6D& q, const Double6D& qdot, ORO_Geometry::Frame& pos_base, ORO_Geometry::Twist& vel_base )
    {
        jointState = q;
        return kine->velocityForward( q, qdot, pos_base, vel_base, sings);
    }

    /**
     * Set the current joint positions for future estimations
     * The configuration represented by these joints will be used 
     * in inverse calculations
     */
    void KinematicsComponent::stateSet(const Double6D& q_new)
    {
        jointState = q_new;
        kine->configurationGet(jointState, configState);
    }

    void KinematicsComponent::stateSet(const ORO_Geometry::Frame& f, Configuration c)
    {
        kine->positionInverse(f,c,jointState,sings);
        configurationSet(c);
    }

    void KinematicsComponent::jointsGet(Double6D& q) const
    {
        q = jointState;
    }
   
    bool KinematicsComponent::adaptJoints( Double6D& qtheoretic)
    {
        for (int i=0; i<6; ++i)
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
