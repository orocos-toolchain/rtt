/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:35 CEST 2004  SerialmZXXmZXmZ.cxx 

                        SerialmZXXmZXmZ.cxx -  description
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
#include "kindyn/SerialmZXXmZXmZ.hpp"
#include <geometry/frames.h>

namespace ORO_KinDyn
{
    using namespace RTT;

    bool SerialmZXXmZXmZ::positionForward( const Double6D& original_q, ORO_Geometry::Frame& mp_base, Singularity& s ) const
    {
        Double6D q = original_q;
	//Change from mZXXmZXmZ to ZXXZXZ and changing the directions of axis 0, 3 and 5
        q[ 0 ] = - q[ 0 ];
        q[ 3 ] = - q[ 3 ];
        q[ 5 ] = - q[ 5 ];

        return zxxzxz.positionForward(q, mp_base, s);
    }

    bool SerialmZXXmZXmZ::positionInverse ( const ORO_Geometry::Frame& mp_base, Configuration c,  Double6D& q, Singularity& s ) const
    {
        bool result = zxxzxz.positionInverse( mp_base, c, q, s );

        if (result)
            {
	      q[ 0 ] = - q[ 0 ];
	      q[ 3 ] = - q[ 3 ];
	      q[ 5 ] = - q[ 5 ];
            }
        return result;
    }


    bool SerialmZXXmZXmZ::velocityForward ( const Double6D& original_q, const Double6D& original_qdot, ORO_Geometry::Frame& mp_base, ORO_Geometry::Twist& vel_base, Singularity& s ) const
    {
      Double6D q = original_q;
      q[0] = - q[0];
      q[3] = - q[3];
      q[5] = - q[5];
       
      Double6D qdot = original_qdot;
      qdot[ 0 ] = - qdot[ 0 ];
      qdot[ 3 ] = - qdot[ 3 ];
      qdot[ 5 ] = - qdot[ 5 ];
	
      return zxxzxz.velocityForward( q, qdot, mp_base, vel_base, s);
    }

    bool SerialmZXXmZXmZ::velocityInverse ( const ORO_Geometry::Frame& vel_base, Configuration c, const ORO_Geometry::Twist& vel, Double6D& q, Double6D& qdot, Singularity& s ) const
    {

        bool result = zxxzxz.velocityInverse(vel_base, c, vel, q, qdot, s);

        if (result)
            {
                q[0] = - q[0];
                q[3] = - q[3];
                q[5] = - q[5];

                qdot[ 0 ] = - qdot[ 0 ] ;
                qdot[ 3 ] = - qdot[ 3 ];
                qdot[ 5 ] = - qdot[ 5 ] ;
            }

        return result;
    }

    bool SerialmZXXmZXmZ::velocityInverse ( const Double6D& q_or, const ORO_Geometry::Twist& vel, Double6D& qdot, Singularity& s ) const
    {
        Double6D q(q_or);
        q[0] = - q[0];
        q[3] = - q[3];
        q[5] = - q[5];

        bool result = zxxzxz.velocityInverse(q, vel, qdot, s);

        if (result)
            {
                qdot[ 0 ] = - qdot[ 0 ] ;
                qdot[ 3 ] = - qdot[ 3 ];
                qdot[ 5 ] = - qdot[ 5 ] ;
            }

        return result;
    }

    bool SerialmZXXmZXmZ::jacobianInverse ( const Double6D& q_or, double JInv[ 6 ][ 6 ], Singularity& s ) const
    {
        Double6D q(q_or);

        q[0] = - q[0];
        q[3] = - q[3];
        q[5] = - q[5];

        return zxxzxz.jacobianInverse(q, JInv, s);
    }

    bool SerialmZXXmZXmZ::jacobianForward( const Double6D& q_or, double J[ 6 ][ 6 ], Singularity& s ) const
    {
        Double6D q(q_or);

        q[0] = - q[0];
        q[3] = - q[3];
        q[5] = - q[5];

        return zxxzxz.jacobianForward(q, J, s);
    }

    void SerialmZXXmZXmZ::configurationGet(const Double6D& q_or, Configuration& c) const
    {
        Double6D q(q_or);

        q[0] = - q[0];
        q[3] = - q[3];
        q[5] = - q[5];

        zxxzxz.configurationGet(q,c);
    }
    
}

