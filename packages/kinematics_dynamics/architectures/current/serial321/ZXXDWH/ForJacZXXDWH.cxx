/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:35 CEST 2004  ForJacZXXDWH.cxx 

                        ForJacZXXDWH.cxx -  description
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
** FILE: ForJacZXXDWH.c
** CREATED: 12 nov 1992
** AUTHOR: Herman Bruyninckx, PMA, KU Leuven, Belgium.
**
** FUNCTION: Calculates the jacobian matrix of a ZXXDWH robot.
**  The Jacobian maps joint velocities to end effector velocities,
**  expressed with respect to the base frame:
**
**   |v_ee(x)|        |qdot[0]|
**   |   .   | =   J  |   .   |
**   |w_ee(z)|        |qdot[5]|
**
**  `J' is a function of `q[i]'.
**
** ALGORITHM: We took the Jacobian of the `equivalent' robot
**  (``ForJacZXXZXZ.c''), and multiplied it (on the right) by `A6':
**          | I3  0 |        | 1  B  0 |
**     A6 = |       |,  A3 = | 0  A  0 |,
**          | 0  A3 |        | 1  -B 0 |
**  `A' is the factor multiplying the fifth joint velocity of
**  a ZXXZXZ robot to obtain the fifth joint velocity of the ZXXDWH.
**  `B' is `A' * `alphadot' (see e.g. ``ForVelZXXDWH.c'').
**
** EXPORTS: int ForJacZXXDWH()
**
** INPUT: `p_d': pointer to ``Kine6RData'' struct; used fields:
**  `q': current joint angles.
** OUTPUT: `J': Jacobian matrix.
**  `status': coded configurations and singularities.
**
** RETURNS: `0' if no singularities occured.
**  `1' otherwise.
**
** CAVEATS: Eq.(37) of Featherstone is wrong in the Y component!
**  The exact equation is given in Eq.(32).
**
** REFERENCES: - Featherstone, R., I. J. Rob. Research, 1983.
**  - Brochier en Haverhals, Studie voor on-line toepassing van
**    inverse kinematika en dynamica, Thesis 1988.
**  - "feathZXXDWH.doc"
**  - "Kine6R.h"
**  - "ForVelZXXDWH.c"
*/

#include <math.h> /* cos,sin,fabs */
#include "kindyn/SerialZXXDWH.hpp"

#define SQRT3t2 (double)(3.46410161513775) /* 2 sqrt(3) */

namespace ORO_KinDyn
{

    bool SerialZXXDWH::jacobianForward( const Double6D& q, double J[ 6 ][ 6 ], Singularity& s ) const
    {
        double c1, c23, c4, c5, s1, s23, s4, s5, s4s5, c4s5;
        double c1c23, c1s23, s1c23, s1s23;
        double Pwx, Pwy, Pwz; /* Coordinates of the wrist */
        double A, B;  /* Compensation factors */
        double c5_eq, q_eq[ 6 ], alpha;
        int i;
        double dWv, dWh;
        
        s.singularitySet(Singularity::None);

        c5 = cos( q[ 4 ] );
        s5 = sin( q[ 4 ] );
        c5_eq = ( c5 + 3. ) / 4;

        if ( q[ 4 ] < -KINEMATICS_EPS )
        {
//            configSet( SerialConfiguration::Flip );
            alpha = atan2( -s5, 0.8660254037844386 * ( c5 - 1. ) );
            q_eq[ 4 ] = -2. * acos( c5_eq );
            A = -2. * s5 / sqrt( ( 1. - c5 ) * ( c5 + 7. ) );
        }
        else
        {
            //configSet( SerialConfiguration::NoFlip );

            if ( q[ 4 ] < KINEMATICS_EPS )
            {
                alpha = M_PI / 2.;
                q_eq[ 4 ] = 0.0;
                A = 1.;
            }

            else
            {
                alpha = atan2( s5, 0.8660254037844386 * ( 1. - c5 ) );
                q_eq[ 4 ] = 2. * acos( c5_eq );
                A = 2. * s5 / sqrt( ( 1. - c5 ) * ( c5 + 7. ) );
            }
        }

        B = SQRT3t2 / ( 7. + c5 );
        q_eq[ 3 ] = q[ 3 ] + alpha;
        q_eq[ 5 ] = q[ 5 ] - alpha;


        c1 = cos( q[ 0 ] );
        s1 = sin( q[ 0 ] );
        c23 = cos( q[ 1 ] + q[ 2 ] );
        s23 = sin( q[ 1 ] + q[ 2 ] );
        c4 = cos( q_eq[ 3 ] );
        s4 = sin( q_eq[ 3 ] );
        c5 = cos( q_eq[ 4 ] );
        s5 = sin( q_eq[ 4 ] );
        c4s5 = c4 * s5;
        s4s5 = s4 * s5;
        c1c23 = c1 * c23;
        c1s23 = c1 * s23;
        s1c23 = s1 * c23;
        s1s23 = s1 * s23;

        dWv = cos( q[ 1 ] ) * l2 + c23 * l3;
        dWh = sin( q[ 1 ] ) * l2 + s23 * l3;

        Pwx = -s1 * dWh;
        Pwy = c1 * dWh;
        Pwz = l1 + dWv;

        /* Featherstone, eq.(37)-(43) */
        /* Angular velocity components: */
        J[ 3 ][ 0 ] = J[ 4 ][ 0 ] = J[ 5 ][ 1 ] = J[ 5 ][ 2 ] = 0.;

        J[ 3 ][ 1 ] = J[ 3 ][ 2 ] = - c1;
        J[ 3 ][ 3 ] = - s1s23;
        J[ 3 ][ 4 ] = - c1 * c4 + s1c23 * s4;
        J[ 3 ][ 5 ] = - s1 * ( c23 * c4s5 + s23 * c5 ) - c1 * s4s5;

        J[ 4 ][ 1 ] = J[ 4 ][ 2 ] = - s1;
        J[ 4 ][ 3 ] = c1s23;
        J[ 4 ][ 4 ] = - s1 * c4 - c1c23 * s4;
        J[ 4 ][ 5 ] = - s1 * s4s5 + c4s5 * c1c23 + c1s23 * c5;

        J[ 5 ][ 0 ] = 1.;
        J[ 5 ][ 3 ] = c23;
        J[ 5 ][ 4 ] = s23 * s4;
        J[ 5 ][ 5 ] = c23 * c5 - s23 * c4s5;

        /* Linear velocity components: */
        J[ 0 ][ 0 ] = - c1 * dWh + Pwy;
        J[ 0 ][ 1 ] = J[ 0 ][ 2 ] = s1 * Pwz;
        J[ 0 ][ 1 ] -= s1 * dWv;
        J[ 0 ][ 2 ] -= s1c23 * l3;
        J[ 0 ][ 3 ] = J[ 5 ][ 3 ] * Pwy - J[ 4 ][ 3 ] * Pwz;
        J[ 0 ][ 4 ] = J[ 5 ][ 4 ] * Pwy - J[ 4 ][ 4 ] * Pwz;
        J[ 0 ][ 5 ] = J[ 5 ][ 5 ] * Pwy - J[ 4 ][ 5 ] * Pwz;

        J[ 1 ][ 0 ] = -s1 * dWh - Pwx;
        J[ 1 ][ 1 ] = J[ 1 ][ 2 ] = -c1 * Pwz;
        J[ 1 ][ 1 ] += c1 * dWv;
        J[ 1 ][ 2 ] += c1c23 * l3;
        J[ 1 ][ 3 ] = J[ 3 ][ 3 ] * Pwz - J[ 5 ][ 3 ] * Pwx;
        J[ 1 ][ 4 ] = J[ 3 ][ 4 ] * Pwz - J[ 5 ][ 4 ] * Pwx;
        J[ 1 ][ 5 ] = J[ 3 ][ 5 ] * Pwz - J[ 5 ][ 5 ] * Pwx;

        J[ 2 ][ 0 ] = 0.;
        J[ 2 ][ 1 ] = J[ 2 ][ 2 ] = c1 * Pwy - s1 * Pwx;
        J[ 2 ][ 1 ] -= dWh;
        J[ 2 ][ 2 ] -= s23 * l3;
        J[ 2 ][ 3 ] = J[ 4 ][ 3 ] * Pwx - J[ 3 ][ 3 ] * Pwy;
        J[ 2 ][ 4 ] = J[ 4 ][ 4 ] * Pwx - J[ 3 ][ 4 ] * Pwy;
        J[ 2 ][ 5 ] = J[ 4 ][ 5 ] * Pwx - J[ 3 ][ 5 ] * Pwy;

        /* Compensations for fourth, fifth and sixth rows of `J': */

        for ( i = 0;i < 6;i++ )
        {
            J[ i ][ 4 ] = A * J[ i ][ 4 ] + B * ( -J[ i ][ 3 ] + J[ i ][ 5 ] );
        }

        /* Calculation of configuration: */
        /*
        if ( dWh >= 0. )
            set( SerialConfiguration::Forward );
        else
            set( SerialConfiguration::Backward );

        if ( q_eq[ 4 ] < 0. )
            set( SerialConfiguration::Flip);

        if ( q[ 2 ] < 0. )
            set( SerialConfiguration::ElbowDown );
        */

        /* Calculation of singularities: */
        if ( fabs( dWh ) < EPS_WRIST_ABOVE_BASE )
            s.singularitySet( Singularity::WristAboveBase );

        if ( ( dWv * dWv + dWh * dWh ) >
                ( max_wrist_dist - EPS_ARM_EXTENDED * EPS_ARM_EXTENDED )
           )
            s.singularitySet( Singularity::ArmExtended );

        if ( fabs( s5 ) < EPS_PARALLEL_WRIST )
            s.singularitySet( Singularity::ParallelWrist ); 

        if ( s.isSingular( Singularity::AnySingularity ) )
            return false;
        else
            return true;
    }

}
