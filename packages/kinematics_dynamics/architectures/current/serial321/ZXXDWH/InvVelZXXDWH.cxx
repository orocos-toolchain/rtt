/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:35 CEST 2004  InvVelZXXDWH.cxx 

                        InvVelZXXDWH.cxx -  description
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
** NAME: InvVelZXXDWH.c
** CREATED: 18 nov 1992
** AUTHOR: Herman Bruyninckx, PMA, KU Leuven, Belgium
**
** FUNCTION: Calculation of the inverse POSITION and VELOCITY kinematics of
**  a ZXXDWH robot,
**  as a particular case of a 6 rotational dof robot with no
**  offsets or eccentricities. The kinematical definitions are as
**  described in the references, with the exception that this
**  robot has a more complicated spherical wrist. 
**  *********************************************************
**  ***   Only the input configuration is calculated.     ***
**  *********************************************************
**
** ALGORITHM: The inverse kinematics of an `equivalent' ZXXDWH robot is first
**  calculated. The equivalent robot has a spherical wrist, with
**  orthogonal axes.
**  Then, the fifth joint angle is determined, together with a
**  correction term for the fourth and sixth joint angles,
**  as described in Willems and Asselberghs (eqs.(3.1)-(3.8)).
**  Then, a similar approach is applied to find the joint
**  velocities.
**
** EXPORTS: int InvVelZXXDWH()
**
** INPUT: `p_d': pointer to ``Kine6RData'' struct; used fields:
**  `status':desired configuration.
**  `t6': the homogeneous transformation matrix of the end
**   effector wrt the base.
**  `vel': end effector twist wrt the base.
** RETURNS: `0': if desired output can be calculated;
**  `1': otherwise.
** OUTPUT: `status':possible singularities.
**  `q': joint angles.
**  `qdot': joint velocities.
** 
** CAVEATS: Joint angles are THEORETICAL, i.e., NO range checking is done!!
**
**  Angles are in radians; lengths are in millimeter.
**
**  At singularities we made the choices:
**  1) to let ``InvVelZXXDWH()'' return the OLD joint values, i.e.
**     these given TO the function (or whatever is contained in `q'
**     on entry!).
**  2) NOT to calculate `qdot'.
** 
** REFERENCES: - Low and Dubey, Int.J.Rob.Research,Vol. 5, No. 4, 1986
**  - Brochier en Haverhals, Studie voor on-line toepassing van
**    inverse kinematika en dynamica, Thesis KULeuven, PMA, 1988.
**  - Willems and Asselberghs, Ontwerp en evaluatie van
**    uitwendige krachtcontrolemethoden voor industriele robots,
**    Thesis 92 EP 22, KULeuven, PMA, 1992.
**  - "feathZXXZXZ.doc"
*/

#include <iostream>

#define SQRT3d2 (double)(0.8660254037844386) /* sqrt(3)/2 */
#define SQRT3t2 (double)(3.46410161513775) /* sqrt(3)*2 */

#include "kindyn/SerialZXXDWH.hpp"

namespace ORO_KinDyn
{

    using namespace std;
    bool SerialZXXDWH::velocityInverse ( const ORO_Geometry::Frame& pos_base, const ORO_Geometry::Twist& vel, Configuration c, Double6D& q, Double6D& qdot, Singularity& s ) const
    {
        double P6x, P6y, P6z; /* Coordinates (in the base frame) of the 6th link */
        double Vwx, Vwy, Vwz; /* Translational velocity components of wrist */
        double Wwx, Wwy, Wwz /* Rotational velocity components of wrist */;
        double Rxy_2;  /* square of XY distance betw. wrist & base of link2 */
        double Rxyz_2;  /* square of XYZ distance betw. wrist & base of link2 */
        double s1, c1, s2, c2, s3, c3, s5, c5; /* sin,cos of thetai */
        double s23, c23, s1c23, c1c23; /* temporary... */
        double d13, d23;
        double temp1, temp2, temp3;   /* ...storage */
        Double6D q_eq, qdot_eq; /* Joint values of `equivalent' robot */
        /* cosines and sines of `equivalent' robot: */
        double c4_eq, s4_eq, c5_eq, s5_eq, c6_eq, s6_eq;
        double alpha, alphadot;  /* compensation terms for joints 4 and 6 */
        double Pwx, Pwy, Pwz;
        double dWh, dWv;

        /* Clear singularity bits without changing configuration code: */
        s.singularitySet( Singularity::None );

        /* Coordinates of last link wrt base: */
        P6x = pos_base.M( 0, 2 ) * l6;
        P6y = pos_base.M( 1, 2 ) * l6;
        P6z = pos_base.M( 2, 2 ) * l6;

        /* Wrist position: (Low & Dubey eq.(34)) */
        Pwx = pos_base.p[ 0 ] - P6x;
        Pwy = pos_base.p[ 1 ] - P6y;
        Pwz = pos_base.p[ 2 ] - P6z;

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

        d13 = pos_base.M( 0, 2 ) * c1 + pos_base.M( 1, 2 ) * s1;

        d23 = -pos_base.M( 0, 2 ) * s1c23 + pos_base.M( 1, 2 ) * c1c23 - pos_base.M( 2, 2 ) * s23;

        /* Calculation of theta4, theta5 and theta6 of `equivalent' robot:
           (formulas (39),(40),(41), Low and Dubey) */
        if ( ( fabs( d13 ) < KINEMATICS_EPS ) && ( fabs( d23 ) < KINEMATICS_EPS ) )
        {
            /* Z-axes of links 4 and 6 are parallel! q[3] keeps its previous value! */
            s4_eq = sin( q[ 3 ] );
            c4_eq = cos( q[ 3 ] );
            q_eq[ 4 ] = 0.0;
            s5_eq = 0;
            c5_eq = 1;
        }

        else
        {
            if ( c.isConfig( Serial321Configuration::Flip ) )
                q_eq[ 3 ] = atan2( d13, -d23 );
            else
                q_eq[ 3 ] = atan2( -d13, d23 );

            c4_eq = cos( q_eq[ 3 ] );

            if ( fabs( d23 ) > KINEMATICS_EPS )
                s4_eq = - c4_eq * d13 / d23;
            else
                s4_eq = sin( q_eq[ 3 ] );

            c5_eq = -pos_base.M( 0, 2 ) * s1 * s23 + pos_base.M( 1, 2 ) * c1 * s23 + pos_base.M( 2, 2 ) * c23;

            s5_eq = d23 * c4_eq - d13 * s4_eq;

            q_eq[ 4 ] = atan2( s5_eq, c5_eq );
        }

        if ( fabs( s5_eq ) < EPS_PARALLEL_WRIST )
            s.singularitySet( Singularity::ParallelWrist );

        s6_eq = -( pos_base.M( 0, 1 ) * c1 + pos_base.M( 1, 1 ) * s1 ) * c4_eq + ( pos_base.M( 0, 1 ) * s1c23 - pos_base.M( 1, 1 ) * c1c23 + pos_base.M( 2, 1 ) * s23 ) * s4_eq;

        c6_eq = ( pos_base.M( 0, 0 ) * c1 + pos_base.M( 1, 0 ) * s1 ) * c4_eq + ( -pos_base.M( 0, 0 ) * s1c23 + pos_base.M( 1, 0 ) * c1c23 - pos_base.M( 2, 0 ) * s23 ) * s4_eq;

        q_eq[ 5 ] = atan2( s6_eq, c6_eq );

        /* If singularities are detected, it is useless to calculate joint velocities
           or accelerations, because some of them will go to infinity... */
        if ( s.isSingular( Singularity::AnySingularity ) )
            return false;


        /* Inverse velocity calculations: */

        /* Calculation of wrist velocity: Low & Dubey, eq.(47) */
        Vwx = vel( 0 ) + vel( 4 ) * Pwz - vel( 5 ) * Pwy;

        Vwy = vel( 1 ) + vel( 5 ) * Pwx - vel( 3 ) * Pwz;

        Vwz = vel( 2 ) + vel( 3 ) * Pwy - vel( 4 ) * Pwx;

        /* First, second and third joint velocities: eqs.(44)-(45), Low & Dubey,
           eqs.(6.24),(6.25) Brochier & Haverhals */
        qdot[ 0 ] = -( c1 * Vwx + s1 * Vwy ) / dWh;

        temp3 = c1 * Vwy - s1 * Vwx;

        qdot[ 1 ] = ( s23 * temp3 + c23 * Vwz ) / l2 / s3;

        qdot[ 2 ] = -( dWh * temp3 + dWv * Vwz ) / l2 / l3 / s3;

        /* Velocity of end effector wrt the wrist: */
        Wwx = c1 * vel( 4 ) - s1 * vel( 3 );

        Wwy = qdot[ 1 ] + qdot[ 2 ] + c1 * vel( 3 ) + s1 * vel( 4 );

        Wwz = c23 * Wwx - s23 * ( vel( 5 ) - qdot[ 0 ] );

        /* Fourth, fifth and sixth joint velocities: eqs.(48)-(50) Low & Dubey */
        qdot_eq[ 4 ] = -c4_eq * Wwy - s4_eq * Wwz;

        qdot_eq[ 5 ] = ( c4_eq * Wwz - s4_eq * Wwy ) / s5_eq;

        qdot_eq[ 3 ] = c23 * ( vel( 5 ) - qdot[ 0 ] ) + s23 * Wwx - c5_eq * qdot_eq[ 5 ];

        /* This far, the `equivalent' robot's inverse velocity kinematics have been
           calculated. Now, we determine the correction factors `alpha' and `alphadot'
           for the fourth and sixth joint positions and velocities.
           (eqs. (3.1) -(3.8), Willems and Asselberghs). 
           `alphadot' is the time derivative of `alpha'. */
        c5 = 4. * cos ( q_eq[ 4 ] / 2. ) - 3.;

        if ( q_eq[ 4 ] < -KINEMATICS_EPS )
        {
            q[ 4 ] = -acos( c5 );
            s5 = sin( q[ 4 ] );
            alpha = atan2 ( -s5, SQRT3d2 * ( c5 - 1. ) );
            qdot[ 4 ] = -sqrt( ( 1. - c5 ) * ( 7. + c5 ) ) * qdot_eq[ 4 ] / 2. / s5;
        }

        else
        {
            if ( q_eq[ 4 ] < KINEMATICS_EPS )
            {
                alpha = M_PI_2;
                q[ 4 ] = 0.0;
                qdot[ 4 ] = qdot_eq[ 4 ];
            }

            else
            {
                q[ 4 ] = acos( c5 );
                s5 = sin( q[ 4 ] );
                alpha = atan2 ( s5, SQRT3d2 * ( 1. - c5 ) );
                qdot[ 4 ] = sqrt( ( 1. - c5 ) * ( 7. + c5 ) ) * qdot_eq[ 4 ] / 2. / s5;
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

        alphadot = -SQRT3t2 / ( 7. + c5 ) * qdot[ 4 ];

        qdot[ 3 ] = qdot_eq[ 3 ] - alphadot;

        qdot[ 5 ] = qdot_eq[ 5 ] + alphadot;

        return true;
    }



    /** 
     * More efficitient algorithm starting from q instead of mp_base_frame:
     * 1. calculate q[] --> q_eq[]
     * 2. now calculate with q_eq[] values en get qdot_eq[]
     * 3. convert qdot_eq[] --> qdot[]
     * 4. return qdot
     */
    bool SerialZXXDWH::velocityInverse( const Double6D& q, const ORO_Geometry::Twist& vel, Double6D& qdot, Singularity& s ) const
    {
        double Vwx, Vwy, Vwz; /* Translational velocity components of wrist */
        double Wwx, Wwy, Wwz /* Rotational velocity components of wrist */;
        //        double Rxy_2;  /* square of XY distance betw. wrist & base of link2 */
        //        double Rxyz_2;  /* square of XYZ distance betw. wrist & base of link2 */
        double s1, c1, s3, s4, c4, s5, c5, s6, c6; /* sin,cos of thetai */
        double s23, c23;//, s1c23, c1c23; /* temporary... */
        //        double d13, d23;
        //        double temp1, temp2, temp3;   /* ...storage */
        Double6D q_eq, qdot_eq; /* Joint values of `equivalent' robot */
        /* cosines and sines of `equivalent' robot: */
        double c4_eq, s4_eq, c5_eq, s5_eq; //, c6_eq, s6_eq;
        double alpha, alphadot;  /* compensation terms for joints 4 and 6 */
        double Pwx, Pwy, Pwz;
        double dWh, dWv;

        /* We first calculate the joint values of the `equivalent' robot.
           Since only the wrist of both robots differs, only joints 4, 5 and 6
           have to be compensated (eq.(3.1)-(3.8), Willems and Asselberghs.
           */
        c5 = cos( q[ 4 ] );
        s5 = sin( q[ 4 ] );
        c5_eq = ( c5 + 3. ) / 4; // XXX BUG XXX c5_eq is wrongly calculated !

        if ( q[ 4 ] < -KINEMATICS_EPS )
        {
            //            set( Configuration::Flip );
            alpha = atan2( -s5, 0.8660254037844386 * ( c5 - 1. ) );
            q_eq[ 4 ] = -2. * acos( c5_eq );
            //qdot_eq[ 4 ] = -2. * s5 * qdot[ 4 ] / sqrt( ( 1. - c5 ) * ( c5 + 7. ) );
        }

        else
        {
            //            set( Configuration::NoFlip );

            if ( q[ 4 ] < KINEMATICS_EPS )
            {
                alpha = M_PI / 2.;
                q_eq[ 4 ] = 0.0;
                //qdot_eq[ 4 ] = qdot[ 4 ];
            }

            else
            {
                alpha = atan2( s5, 0.8660254037844386 * ( 1. - c5 ) );
                q_eq[ 4 ] = 2. * acos( c5_eq );
                //qdot_eq[ 4 ] = 2. * s5 * qdot[ 4 ] / sqrt( ( 1. - c5 ) * ( c5 + 7. ) );
            }
        }
        c5_eq = cos(q_eq[4]);

        //alphadot = -3.46410161513775 / ( 7. + c5 ) * qdot[ 4 ];
        q_eq[ 3 ] = q[ 3 ] + alpha;
        q_eq[ 5 ] = q[ 5 ] - alpha;
        //qdot_eq[ 3 ] = qdot[ 3 ] + alphadot;
        //qdot_eq[ 5 ] = qdot[ 5 ] - alphadot;



        /* From here on, the kinematics are identical to those of the ZXXZXZ: */
        c1 = cos( q[ 0 ] );
        s1 = sin( q[ 0 ] );
        s3 = sin( q[ 2 ] );
        c4_eq = cos( q_eq[ 3 ] );
        s4_eq = sin( q_eq[ 3 ] );        
        s5_eq = sin( q_eq[ 4 ] );        
        c23 = cos( q[ 1 ] + q[ 2 ] );
        s23 = sin( q[ 1 ] + q[ 2 ] );
        c4 = cos( q_eq[ 3 ] );
        s4 = sin( q_eq[ 3 ] );
        c5 = cos( q_eq[ 4 ] );
        s5 = sin( q_eq[ 4 ] );
        c6 = cos( q_eq[ 5 ] );
        s6 = sin( q_eq[ 5 ] );
        //s5c6 = s5 * c6;
        //s5s6 = s5 * s6;
        //c4s5 = c4 * s5;
        //s4s5 = s4 * s5;
        //c1c23 = c1 * c23;
        //c1s23 = c1 * s23;
        //s1c23 = s1 * c23;
        //s1s23 = s1 * s23;

        /* vertical distance of wrist from shoulder: */
        dWv = cos( q[ 1 ] ) * l2 + c23 * l3;
        /* horizontal distance of wrist from shoulder: */
        dWh = sin( q[ 1 ] ) * l2 + s23 * l3;

        /* Position of wrist: */
        Pwx = - s1 * dWh;
        Pwy = + c1 * dWh;
        Pwz = l1 + dWv;

        //************************************************************************************
        //************************************************************************************
        //   NO BUG UNTIL HERE, q_eq is equal with long algorithm.
        //************************************************************************************
        //************************************************************************************
        //************************************************************************************
        /* Inverse velocity calculations: */

        /* Calculation of wrist velocity: Low & Dubey, eq.(47) */
        Vwx = vel( 0 ) + vel( 4 ) * Pwz - vel( 5 ) * Pwy;

        Vwy = vel( 1 ) + vel( 5 ) * Pwx - vel( 3 ) * Pwz;

        Vwz = vel( 2 ) + vel( 3 ) * Pwy - vel( 4 ) * Pwx;

        /* First, second and third joint velocities: eqs.(44)-(45), Low & Dubey,
           eqs.(6.24),(6.25) Brochier & Haverhals */
        qdot[ 0 ] = -( c1 * Vwx + s1 * Vwy ) / dWh;

        double temp3 = c1 * Vwy - s1 * Vwx;

        qdot[ 1 ] = ( s23 * temp3 + c23 * Vwz ) / l2 / s3;

        qdot[ 2 ] = -( dWh * temp3 + dWv * Vwz ) / l2 / l3 / s3;

        /* Velocity of end effector wrt the wrist: */
        Wwx = c1 * vel( 4 ) - s1 * vel( 3 );

        Wwy = qdot[ 1 ] + qdot[ 2 ] + c1 * vel( 3 ) + s1 * vel( 4 );

        Wwz = c23 * Wwx - s23 * ( vel( 5 ) - qdot[ 0 ] );

        /* Fourth, fifth and sixth joint velocities: eqs.(48)-(50) Low & Dubey */
        qdot_eq[ 4 ] = -c4_eq * Wwy - s4_eq * Wwz;

        qdot_eq[ 5 ] = ( c4_eq * Wwz - s4_eq * Wwy ) / s5_eq;

        qdot_eq[ 3 ] = c23 * ( vel( 5 ) - qdot[ 0 ] ) + s23 * Wwx - c5_eq * qdot_eq[ 5 ];

        /* This far, the `equivalent' robot's inverse velocity kinematics have been
           calculated. Now, we determine the correction factors `alpha' and `alphadot'
           for the fourth and sixth joint positions and velocities.
           (eqs. (3.1) -(3.8), Willems and Asselberghs). 
           `alphadot' is the time derivative of `alpha'. */
        c5 = 4. * cos ( q_eq[ 4 ] / 2. ) - 3.;

        if ( q_eq[ 4 ] < -KINEMATICS_EPS )
        {
            //q[ 4 ] = -acos( c5 );
            //s5 = sin( q[ 4 ] );
            //alpha = atan2 ( -s5, SQRT3d2 * ( c5 - 1. ) );
            qdot[ 4 ] = -sqrt( ( 1. - c5 ) * ( 7. + c5 ) ) * qdot_eq[ 4 ] / 2. / s5;
        }

        else
        {
            if ( q_eq[ 4 ] < KINEMATICS_EPS )
            {
                //alpha = M_PI_2;
                //q[ 4 ] = 0.0;
                qdot[ 4 ] = qdot_eq[ 4 ];
            }

            else
            {
                //q[ 4 ] = acos( c5 );
                //s5 = sin( q[ 4 ] );
                //alpha = atan2 ( s5, SQRT3d2 * ( 1. - c5 ) );
                qdot[ 4 ] = sqrt( ( 1. - c5 ) * ( 7. + c5 ) ) * qdot_eq[ 4 ] / 2. / s5;
            }
        }
        /*
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
        */
        alphadot = -SQRT3t2 / ( 7. + c5 ) * qdot[ 4 ];

        qdot[ 3 ] = qdot_eq[ 3 ] - alphadot;

        qdot[ 5 ] = qdot_eq[ 5 ] + alphadot;

        return true;

    }
}
