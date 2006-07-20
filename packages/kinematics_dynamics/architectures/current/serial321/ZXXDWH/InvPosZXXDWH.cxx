/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:35 CEST 2004  InvPosZXXDWH.cxx 

                        InvPosZXXDWH.cxx -  description
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
/*
** NAME: InvPosZXXDWH.c
** CREATED: 12 nov 1992
** AUTHOR: Herman Bruyninckx, PMA, KU Leuven, Belgium
**
** FUNCTION: Calculation of the inverse POSITION kinematics of
**  a ZXXDWH robot,
**  as a particular case of a 6 rotational dof robot with no
**  offsets or eccentricities. The kinematical definitions are as
**  described in the references, with the exception that this
**  robot has a more complicated spherical wrist. 
**  *********************************************************
**  ***   Only the input configuration is calculated.     ***
**  *********************************************************
**
** ALGORITHM: The inverse kinematics of an `equivalent' ZXXZXZ robot is first
**  calculated. The equivalent robot has a spherical wrist, with
**  orthogonal axes.
**  Then, the fifth joint angle is determined, together with a
**  correction term for the fourth and sixth joint angles,
**  as described in Willems and Asselberghs (eqs.(3.1)-(3.8)).
**
** EXPORTS: int InvPosZXXDWH()
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
**  At singularities we made the choice to let ``InvPosZXXDWH()''
**  return the OLD joint values, i.e. these given TO the function
**  (or whatever is contained in `q' on entry!).
** 
** REFERENCES: - Low and Dubey, Int.J.Rob.Research,Vol. 5, No. 4, 1986
**  - Brochier en Haverhals, Studie voor on-line toepassing van
**    inverse kinematika en dynamica, Thesis KULeuven, PMA, 1988.
**  - Willems and Asselberghs, Ontwerp en evaluatie van
**    uitwendige krachtcontrolemethoden voor industriele robots,
**    Thesis 92 EP 22, KULeuven, PMA, 1992.
**  - "feathZXXDWH.doc"
**  - "Kine6R.h"
*/

#define SQRT3d2 (double)(0.8660254037844386) /* sqrt(3)/2 */

#include <math.h> /* cos,sin,fabs,atan2 */
#include <geometry/frames.h>
#include "kindyn/SerialZXXDWH.hpp"

// for debugging
#ifdef __cplusplus
extern "C"
#endif
    int rtos_printf( const char* fmt, ... );

namespace ORO_KinDyn
{
    using namespace RTT;

    bool SerialZXXDWH::positionInverse ( const ORO_Geometry::Frame& mp_base, Configuration c,  Double6D& q, Singularity& s ) const
    {
        double P6x, P6y, P6z; /* Coordinates (in the base frame) of the 6th link */
        double Rxy_2;  /* square of XY distance betw. wrist & base of link2 */
        double Rxyz_2;  /* square of XYZ distance betw. wrist & base of link2 */
        double s1, c1, s2, c2, s3, c3, s5, c5; /* sin,cos of thetai */
        double s23, c23, s1c23, c1c23; /* temporary... */
        double d13, d23;
        double temp1, temp2;   /* ...storage */
        double q_eq[ 6 ]; /* Joint values of `equivalent' robot */
        /* cosines and sines of `equivalent' robot: */
        double c4_eq, s4_eq, c5_eq, s5_eq, c6_eq, s6_eq;
        double alpha;  /* compensation term for joints 4 and 6 */
        double Pwx, Pwy, Pwz;
        double dWv, dWh;

        // q to return when singularity is detected
        Double6D q_orig(q);

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

        /* By Peter Soetens */

        if ( Rxy_2 < 0.0 )
            return false;

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

            /*    rtos_printf("ARM ERROR");*/
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

        if ( 1.0 - c3 * c3 < 0.0 )
            return false;

        if ( c.isConfig( Serial321Configuration::ElbowDown ) )
            s3 = -sqrt ( 1.0 - c3 * c3 );
        else
            s3 = sqrt( 1.0 - c3 * c3 );

        q[ 2 ] = atan2 ( s3, c3 );

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

        /* Calculation of theta4, theta5 and theta6 of `equivalent' robot:
           (formulas (39),(40),(41), Low and Dubey) */
        if ( ( fabs( d13 ) < KINEMATICS_EPS ) && ( fabs( d23 ) < KINEMATICS_EPS ) )
        {
            /* Z-axes of links 4 and 6 are parallel! q[3] keeps its previous value! */
            s4_eq = sin( q[ 3 ] );
            c4_eq = cos( q[ 3 ] );
            q_eq[ 4 ] = 0.0;
            q_eq[ 3 ] = q[ 3 ];
            s5_eq = 0;
        }

        else
        {
            if ( c.isConfig( Serial321Configuration::Flip ) )
                q_eq[ 3 ] = atan2 ( d13, -d23 );
            else
                q_eq[ 3 ] = atan2 ( -d13, d23 );

            c4_eq = cos ( q_eq[ 3 ] );

            if ( fabs( d23 ) > KINEMATICS_EPS )
                s4_eq = - c4_eq * d13 / d23;
            else
                s4_eq = sin ( q_eq[ 3 ] );

            c5_eq = -mp_base.M( 0, 2 ) * s1 * s23 + mp_base.M( 1, 2 ) * c1 * s23 + mp_base.M( 2, 2 ) * c23;

            s5_eq = d23 * c4_eq - d13 * s4_eq;

            q_eq[ 4 ] = atan2 ( s5_eq, c5_eq );
        }

        if ( fabs( s5_eq ) < EPS_PARALLEL_WRIST )
            s.singularitySet( Singularity::ParallelWrist );

        s6_eq = -( mp_base.M( 0, 1 ) * c1 + mp_base.M( 1, 1 ) * s1 ) * c4_eq + ( mp_base.M( 0, 1 ) * s1c23 - mp_base.M( 1, 1 ) * c1c23 + mp_base.M( 2, 1 ) * s23 ) * s4_eq;

        c6_eq = ( mp_base.M( 0, 0 ) * c1 + mp_base.M( 1, 0 ) * s1 ) * c4_eq + ( -mp_base.M( 0, 0 ) * s1c23 + mp_base.M( 1, 0 ) * c1c23 - mp_base.M( 2, 0 ) * s23 ) * s4_eq;

        q_eq[ 5 ] = atan2 ( s6_eq, c6_eq );

        /* This far, the `equivalent' robot's inverse kinematics have been
           calculated. Now, we determine the correction factor `alpha'
           for the fourth and sixth joint positions.
           (eqs. (3.1) -(3.8), Willems and Asselberghs). */

        /* remark : abs(q_eq[4]) is always smaller than 120 degrees */
        /* XXX Except when we have rounding errors ! (which happens) */

        c5 = 4. * cos ( q_eq[ 4 ] / 2. ) - 3.;
        if (c5 > 1.) c5 = 1; else if (c5 <-1.) c5=-1.; // correction for rounding errors

        if ( q_eq[ 4 ] < -KINEMATICS_EPS )
        {
            q[ 4 ] = -acos( c5 );
            s5 = sin( q[ 4 ] );
            alpha = atan2 ( -s5, SQRT3d2 * ( c5 - 1. ) );
        }

        else
        {
            if ( q_eq[ 4 ] < KINEMATICS_EPS )
            {
                alpha = M_PI_2;
                q[ 4 ] = 0.0;
            }

            else
            {
                q[ 4 ] = acos( c5 );
                s5 = sin( q[ 4 ] );
                alpha = atan2 ( s5, SQRT3d2 * ( 1. - c5 ) );
            }
        }

        q[ 3 ] = q_eq[ 3 ] - alpha;

        if ( q[ 3 ] >= M_PI )
            q[ 3 ] -= M_PI_T2;
        else if ( q[ 3 ] <= -M_PI )
            q[ 3 ] += M_PI_T2;

        q[ 5 ] = q_eq[ 5 ] + alpha;

        if ( q[ 5 ] >= M_PI )
            q[ 5 ] -= M_PI_T2;
        else if ( q[ 5 ] <= -M_PI )
            q[ 5 ] += M_PI_T2;

        if ( s.isSingular( Singularity::AnySingularity ) )
            return false;
        else
            return true;

    }

}
