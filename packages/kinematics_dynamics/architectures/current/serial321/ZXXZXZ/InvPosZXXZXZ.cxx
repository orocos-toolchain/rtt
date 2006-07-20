/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  InvPosZXXZXZ.cxx 

                        InvPosZXXZXZ.cxx -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 
/*
** NAME: InvPosZXXZXZ.c
** CREATED: 9 nov 1992
** AUTHOR: Herman Bruyninckx, PMA, KULeuven, Belgium
**
** FUNCTION: Calculation of the inverse POSITION kinematics
**  of a 6 rotational dof robot with no shoulder offset, and no
**  eccentricities. The kinematical definitions are as
**  described in the references. 
**  ******************************************************
**  ***   Only the input configuration is calculated.  ***
**  ******************************************************
**
** REFERENCES: - Low and Dubey, Int.J.Rob.Research,Vol. 5, No. 4, 1986
**  - Brochier en Haverhals, Studie voor on-line toepassing van
**    inverse kinematika en dynamica, Thesis 1988.
**  - "feathZXXZXZ.doc"
**  - "Kine6R.h"
**
** EXPORTS: int InvPosZXXZXZ();
**
** INPUT: `p_d': pointer to ``Kine6RData'' struct; used fields:
**  `status':desired configuration.
**  `t6': the homogeneous transformation matrix of the end
**   effector wrt the base.
** RETURNS: `0': if desired output can be calculated;
**  `1': otherwise.
** OUTPUT: `status':possible singularities.
**  `q': joint angles.
** 
** CAVEATS: Joint angles are THEORETICAL, i.e., NO range checking is done!!
**
**  Angles are in radians; lengths are in millimeter.
**
**  At singularities ``InvPosZXXZXZ()'' returns the OLD joint
**  values, i.e.  these given TO the function (or whatever is
**  contained in `q' on entry!).
*/

#include <math.h> /* cos,sin,fabs,atan2 */
#include <geometry/frames.h>
#include "kindyn/SerialZXXZXZ.hpp"

namespace ORO_KinDyn
{
    using namespace RTT;

    bool SerialZXXZXZ::positionInverse ( const ORO_Geometry::Frame& mp_base, Configuration c,  Double6D& q, Singularity& s ) const
    {
        double P6x, P6y, P6z; /* Coordinates (in the base frame) of the 6th link */
        double Rxy_2;  /* square of XY distance betw. wrist & base of link2 */
        double Rxyz_2;  /* square of XYZ distance betw. wrist & base of link2 */
        double s1, c1, s2, c2, s3, c3, s4, c4, s5, c5, s6, c6; /* sin,cos of thetai */
        double s23, c23, s1c23, c1c23; /* temporary... */
        double d13, d23;
        double temp1, temp2;   /* ...storage */
        double Pwx, Pwy, Pwz;
        double dWv, dWh;

        s.singularitySet( Singularity::None );

        /* Coordinates of last link wrt base: */
        P6x = mp_base.M( 0, 2 ) * l6;
        P6y = mp_base.M( 1, 2 ) * l6;
        P6z = mp_base.M( 2, 2 ) * l6;

        /* Wrist position: (Low & Dubey eq.(34)) */
        Pwx = mp_base.p[ 0 ] - P6x;
        Pwy = mp_base.p[ 1 ] - P6y;
        Pwz = mp_base.p[ 2 ] - P6z;

        /* Calculation of relevant lengths: */
        dWv = Pwz - l1;
        Rxy_2 = Pwx * Pwx + Pwy * Pwy;
        Rxyz_2 = Rxy_2 + dWv * dWv;

        if ( c.isConfig( Serial321Configuration::Backward ) )
            dWh = -sqrt( Rxy_2 );
        else
            dWh = sqrt( Rxy_2 );

        /*  Arm extended? */
        if ( Rxyz_2 > ( max_wrist_dist - EPS_ARM_EXTENDED * EPS_ARM_EXTENDED ) )
        {
            s.singularitySet( Singularity::ArmExtended );
            /* Out of reach? */

            if ( Rxyz_2 > max_wrist_dist )
                s.singularitySet( Singularity::OutOfReach );

            return false;
        }

        /* Wrist is above shoulder? */
        if ( fabs( dWh ) < EPS_WRIST_ABOVE_BASE )
        {
            s.singularitySet( Singularity::WristAboveBase );
            /*    rtos_printf("WRIST ERROR");*/
            return false;
        }


        /* Calculation of q[0] by geometry of triangle formed by dWh and Rxy_2: */
        s1 = -Pwx * dWh / Rxy_2;

        c1 = Pwy * dWh / Rxy_2;

        q[ 0 ] = atan2( s1, c1 );

        /* Calculation of q[2]: (formulas (B6) and (36), Low and Dubey) */
        c3 = ( Rxyz_2 - const1 ) / const2;

        if ( c.isConfig( Serial321Configuration::ElbowDown ) )
            s3 = -sqrt( 1.0 - c3 * c3 );
        else
            s3 = sqrt( 1.0 - c3 * c3 );

        q[ 2 ] = atan2( s3, c3 );

        /* Calculation of q[1]: (formulas (37), Low and Dubey) */
        temp1 = l3 * c3 + l2;

        temp2 = l3 * s3;

        c2 = ( dWv * temp1 + dWh * temp2 ) / Rxyz_2;

        s2 = ( dWh - c2 * temp2 ) / temp1;

        q[ 1 ] = atan2( s2, c2 );

        /* Orientation of wrist wrt arm: Dij (formulas (42), Low and Dubey) */
        s23 = s2 * c3 + c2 * s3;

        c23 = c2 * c3 - s2 * s3;

        s1c23 = s1 * c23;

        c1c23 = c1 * c23;

        d13 = mp_base.M( 0, 2 ) * c1 + mp_base.M( 1, 2 ) * s1;

        d23 = -mp_base.M( 0, 2 ) * s1c23 + mp_base.M( 1, 2 ) * c1c23 - mp_base.M( 2, 2 ) * s23;

        /* Calculation of q[3], q[4] and q[5], eqs.(39)-(41) Low & Dubey: */
        if ( ( fabs( d13 ) < KINEMATICS_EPS ) && ( fabs( d23 ) < KINEMATICS_EPS ) )
        {
            /* Z-axes of links 4 and 6 are parallel! q[3] keeps its previous value! */
            s4 = sin( q[ 3 ] );
            c4 = cos( q[ 3 ] );
            s5 = 0.0;
            q[ 4 ] = 0.0;
        }

        else
        {
            if ( c.isConfig( Serial321Configuration::Flip ) )
                q[ 3 ] = atan2( d13, -d23 );
            else
                q[ 3 ] = atan2( -d13, d23 );

            c4 = cos( q[ 3 ] );

            if ( fabs( d23 ) > KINEMATICS_EPS )
                s4 = - c4 * d13 / d23;
            else
                s4 = sin( q[ 3 ] );

            c5 = -mp_base.M( 0, 2 ) * s1 * s23 + mp_base.M( 1, 2 ) * c1 * s23 + mp_base.M( 2, 2 ) * c23;

            s5 = d23 * c4 - d13 * s4;

            q[ 4 ] = atan2( s5, c5 );
        }

        if ( fabs( s5 ) < EPS_PARALLEL_WRIST )
            s.singularitySet( Singularity::ParallelWrist );

        s6 = -( mp_base.M( 0, 1 ) * c1 + mp_base.M( 1, 1 ) * s1 ) * c4 + ( mp_base.M( 0, 1 ) * s1c23 - mp_base.M( 1, 1 ) * c1c23 + mp_base.M( 2, 1 ) * s23 ) * s4;

        c6 = ( mp_base.M( 0, 0 ) * c1 + mp_base.M( 1, 0 ) * s1 ) * c4 + ( -mp_base.M( 0, 0 ) * s1c23 + mp_base.M( 1, 0 ) * c1c23 - mp_base.M( 2, 0 ) * s23 ) * s4;

        q[ 5 ] = atan2( s6, c6 );

        if ( s.isSingular( Singularity::AnySingularity ) )
            return false;
        else
            return true;
    }
}
