/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:35 CEST 2004  ForVelZXXDWH.cxx 

                        ForVelZXXDWH.cxx -  description
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
** NAME: ForVelZXXDWH.c
** CREATED: 18 nov 92
** AUTHOR: Herman Bruyninckx, PMA, KU Leuven, Belgium.
**
** EXPORTS: ForVelZXXDWH();
**
** FUNCTION: calculation of the forward POSITION and VELOCITY kinematics of
**  a ZXXDWH robot.
**
** ALGORITHM: First, compensation factors `alpha' and `alphadot' are
**  calculated, and the wrist joint angles and velocities are
**  adapted accordingly. Then, the forward position and velocity
**  kinematics for an `equivalent' ZXXZXZ robot is calculated
**  (cf. ForVelZXXZXZ.c).
**
**  The kinematical definitions are as described in the references.
**
** INPUT: `p_d': pointer to ``Kine6RData'' struct. Used fields are:
**  `q': the current joint angles.
**  `qdot': the current joint velocities.
** OUTPUT: `p_d': used fields:
**  `t6': pointer to the homogeneous transform expressing
**   the location of the end effector wrt the base;
**  `vel': the end effector velocity twist wrt the base.
**  `status': pointer to coded configuration and singularities;
**
** CAVEATS: joint angles are THEORETICAL, i.e., NO range checking is done!!
**
** REFERENCE: - Featherstone, Int.J.Rob.Research, Vol. 2, 1983.
**  - Low and Dubey, Int.J.Rob.Research,Vol. 5, No. 4, 1986
**  - Brochier and Haverhals, ``Studie voor on-line toepassing van
**    inverse kinematika en dynamica,'' Thesis 1988.("kukavo2.c")
**  - Willems and Asselberghs, ``Ontwerp en evaluatie van
**    uitwendige krachtcontrolemethoden voor industriele robots,''
**    Thesis 92 EP 22, KULeuven, PMA, 1992.
**  - "feathZXXDWH.doc"
**  - "Kine6R.h"
*/

#include <math.h> /* cos,sin,fabs */
#include "kindyn/SerialZXXDWH.hpp"

namespace ORO_KinDyn
{

    bool SerialZXXDWH::velocityForward ( const Double6D& q, const Double6D& qdot, ORO_Geometry::Frame& mp_base, ORO_Geometry::Twist& vel_base, Singularity& s ) const
    {
        double c1, c23, c4, c5, c6, s1, s23, s4, s5, s6;
        double c1c23, c1s23, c4s5, s4s5, s1c23, s1s23, s5c6, s5s6;
        double P6x, P6y, P6z; /* Coordinates of the last link */

        double c5_eq;
        double Wwy, Wwz, Wx, Wy, Vx, Vy;
        /* Compensation factors and compensated joint values for the wrist: */
        double alpha, alphadot;
        double q_eq[ 6 ];
        double qdot_eq[ 6 ];

	double dWv, dWh;
	double Pwx, Pwy, Pwz;

        /* We first calculate the joint values of the `equivalent' robot.
           Since only the wrist of both robots differs, only joints 4, 5 and 6
           have to be compensated (eq.(3.1)-(3.8), Willems and Asselberghs.
           */
        c5 = cos( q[ 4 ] );
        s5 = sin( q[ 4 ] );
        c5_eq = ( c5 + 3. ) / 4;

        if ( q[ 4 ] < -KINEMATICS_EPS )
        {
            //            set( SerialConfiguration::Flip );
            alpha = atan2( -s5, 0.8660254037844386 * ( c5 - 1. ) );
            q_eq[ 4 ] = -2. * acos( c5_eq );
            qdot_eq[ 4 ] = -2. * s5 * qdot[ 4 ] / sqrt( ( 1. - c5 ) * ( c5 + 7. ) );
        }

        else
        {
            //            set( SerialConfiguration::NoFlip );

            if ( q[ 4 ] < KINEMATICS_EPS )
            {
                alpha = M_PI / 2.;
                q_eq[ 4 ] = 0.0;
                qdot_eq[ 4 ] = qdot[ 4 ];
            }

            else
            {
                alpha = atan2( s5, 0.8660254037844386 * ( 1. - c5 ) );
                q_eq[ 4 ] = 2. * acos( c5_eq );
                qdot_eq[ 4 ] = 2. * s5 * qdot[ 4 ] / sqrt( ( 1. - c5 ) * ( c5 + 7. ) );
            }
        }

        alphadot = -3.46410161513775 / ( 7. + c5 ) * qdot[ 4 ];
        q_eq[ 3 ] = q[ 3 ] + alpha;
        q_eq[ 5 ] = q[ 5 ] - alpha;
        qdot_eq[ 3 ] = qdot[ 3 ] + alphadot;
        qdot_eq[ 5 ] = qdot[ 5 ] - alphadot;



        /* From here on, the kinematics are identical to those of the ZXXZXZ: */
        c1 = cos( q[ 0 ] );
        s1 = sin( q[ 0 ] );
        c23 = cos( q[ 1 ] + q[ 2 ] );
        s23 = sin( q[ 1 ] + q[ 2 ] );
        c4 = cos( q_eq[ 3 ] );
        s4 = sin( q_eq[ 3 ] );
        c5 = cos( q_eq[ 4 ] );
        s5 = sin( q_eq[ 4 ] );
        c6 = cos( q_eq[ 5 ] );
        s6 = sin( q_eq[ 5 ] );
        s5c6 = s5 * c6;
        s5s6 = s5 * s6;
        c4s5 = c4 * s5;
        s4s5 = s4 * s5;
        c1c23 = c1 * c23;
        c1s23 = c1 * s23;
        s1c23 = s1 * c23;
        s1s23 = s1 * s23;

        mp_base.M( 1, 0 ) = s4 * c5 * s6 - c4 * c6;
        mp_base.M( 2, 0 ) = c4 * c5 * s6 + s4 * c6;
        mp_base.M( 0, 0 ) = -s1c23 * mp_base.M( 2, 0 ) - c1 * mp_base.M( 1, 0 ) + s1s23 * s5s6;
        mp_base.M( 1, 0 ) = c1c23 * mp_base.M( 2, 0 ) - s1 * mp_base.M( 1, 0 ) - c1s23 * s5s6;
        mp_base.M( 2, 0 ) = - s23 * mp_base.M( 2, 0 ) - c23 * s5s6;

        mp_base.M( 1, 1 ) = s4 * c5 * c6 + c4 * s6;
        mp_base.M( 2, 1 ) = c4 * c5 * c6 - s4 * s6;
        mp_base.M( 0, 1 ) = -s1c23 * mp_base.M( 2, 1 ) - c1 * mp_base.M( 1, 1 ) + s1s23 * s5c6;
        mp_base.M( 1, 1 ) = c1c23 * mp_base.M( 2, 1 ) - s1 * mp_base.M( 1, 1 ) - c1s23 * s5c6;
        mp_base.M( 2, 1 ) = - s23 * mp_base.M( 2, 1 ) - c23 * s5c6;

        mp_base.M( 0, 2 ) = -s1c23 * c4s5 - c1 * s4s5 - s1s23 * c5;
        mp_base.M( 1, 2 ) = c1c23 * c4s5 - s1 * s4s5 + c1s23 * c5;
        mp_base.M( 2, 2 ) = - s23 * c4s5 + c23 * c5;

        /* vertical distance of wrist from shoulder: */
        dWv = cos( q[ 1 ] ) * l2 + c23 * l3;
        /* horizontal distance of wrist from shoulder: */
        dWh = sin( q[ 1 ] ) * l2 + s23 * l3;

        /* Position of last link: */
        P6x = mp_base.M( 0, 2 ) * l6;
        P6y = mp_base.M( 1, 2 ) * l6;
        P6z = mp_base.M( 2, 2 ) * l6;

        /* Position of wrist: */
        Pwx = - s1 * dWh;
        Pwy = + c1 * dWh;
        Pwz = l1 + dWv;

        /* End effector position = position of last link + position of wrist: */
        mp_base.p[ 0 ] = P6x + Pwx;
        mp_base.p[ 1 ] = P6y + Pwy;
        mp_base.p[ 2 ] = P6z + Pwz;
#if 0
        /* CALCULATION OF CONFIGURATION: */

        if ( dWh >= 0.0 )
            set( SerialConfiguration::Forward );
        else
            set( SerialConfiguration::Backward );

        if ( q[ 4 ] < 0. )
            set( SerialConfiguration::Flip );

        if ( q[ 2 ] < 0. )
            set( SerialConfiguration::ElbowDown );
#endif

        /* CALCULATION OF SINGULARITIES: */
        if ( fabs( dWh ) < EPS_WRIST_ABOVE_BASE )
            s.singularitySet( Singularity::WristAboveBase );

        if ( ( dWv * dWv + dWh * dWh ) >
                ( max_wrist_dist - EPS_ARM_EXTENDED * EPS_ARM_EXTENDED )
           )
            s.singularitySet( Singularity::ArmExtended );

        if ( fabs( s5 ) < EPS_PARALLEL_WRIST )
            s.singularitySet( Singularity::ParallelWrist );

        /* Forward velocity transformation according to Featherstone eqs.(37)-(43) */
        Wwy = -s4 * qdot_eq[ 4 ] + c4s5 * qdot_eq[ 5 ];

        Wwz = qdot_eq[ 3 ] + c5 * qdot_eq[ 5 ];

        Wx = -c4 * qdot_eq[ 4 ] - s4s5 * qdot_eq[ 5 ] - qdot[ 1 ] - qdot[ 2 ];

        Wy = c23 * Wwy + s23 * Wwz;

        vel_base( 3 ) = c1 * Wx - s1 * Wy;

        vel_base( 4 ) = c1 * Wy + s1 * Wx;

        vel_base( 5 ) = qdot[ 0 ] + c23 * Wwz - s23 * Wwy;

        Vx = -dWh * qdot[ 0 ];

        Vy = dWv * qdot[ 1 ] + l3 * c23 * qdot[ 2 ];

        vel_base( 0 ) = c1 * Vx - s1 * Vy - vel_base( 4 ) * Pwz + vel_base( 5 ) * Pwy;

        vel_base( 1 ) = c1 * Vy + s1 * Vx - vel_base( 5 ) * Pwx + vel_base( 3 ) * Pwz;

        vel_base( 2 ) = -dWh * qdot[ 1 ] - l3 * s23 * qdot[ 2 ] - vel_base( 3 ) * Pwy + vel_base( 4 ) * Pwx;

        if ( s.isSingular( Singularity::AnySingularity ) )
            return false;
        else
            return true;
    }
}
