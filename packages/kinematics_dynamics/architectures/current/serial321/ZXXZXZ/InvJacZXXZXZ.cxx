/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  InvJacZXXZXZ.cxx 

                        InvJacZXXZXZ.cxx -  description
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
** NAME: InvJacZXXZXZ.c
** CREATED: 6 Dec 1992
** AUTHOR: Herman Bruyninckx, PMA, KU Leuven, Belgium
**
** FUNCTION: Calculation of the inverse Jacobian matrix of a 6 rotational
**  dof robot with no offsets or eccentricities.
**  The kinematical definitions are as described in the references. 
**  The inverse Jacobian maps end effector velocities, expressed
**  with respect to the base frame, to joint velocities:
**
**   |qdot[0]|        |v_ee(x)|
**   |   .   | = JInv |   .   |
**   |qdot[5]|        |w_ee(z)|
**
**  `JInv' is a function of `q[i]'.
**
** ALGORITHM: From "InvVelZXXZXZ()", we wrote out in full the equations
**  linking `qdot[i]' with `v_ee(x)'...`w_ee(z)'.
**
** REFERENCES: - Low and Dubey, Int.J.Rob.Research,Vol. 5, No. 4, 1986
**  - Brochier en Haverhals, Studie voor on-line toepassing van
**    inverse kinematika en dynamica, Thesis 1988.
**  - "feathZXXZXZ.doc"
**  - "Kine6R.h"
**  - "InvVelZXXZXZ"
**
** EXPORTS: int InvJacZXXZXZ()
**
** INPUT: `p_d': pointer to ``Kine6RData''; used field:
**  `q': current joint angles.
** OUTPUT: `JInv': inverse Jacobian matrix.
**
** RETURNS: `0' if no singularities occured.
**  `1' otherwise.
*/

#include <math.h> /* cos,sin,fabs,atan2 */
#include "kindyn/SerialZXXZXZ.hpp"

namespace ORO_KinDyn
{

    bool SerialZXXZXZ::jacobianInverse ( const Double6D& q, double JInv[ 6 ][ 6 ], Singularity& s ) const
    {
        double Pwx, Pwy, Pwz;    /* Coordinates (in the base frame) of the wrist */
        double s1, c1, s4, c4, s5, c5; /* sin,cos of thetai */
        double s23, c23, s1s23, s1c23, c1s23, c1c23;
        double s23s4, s23c4;
        double CONST1;
        double dWv, dWh;
 
        s1 = sin( q[ 0 ] );
        c1 = cos( q[ 0 ] );
        s23 = sin( q[ 1 ] + q[ 2 ] );
        c23 = cos( q[ 1 ] + q[ 2 ] );
        s4 = sin( q[ 3 ] );
        c4 = cos( q[ 3 ] );
        s5 = sin( q[ 4 ] );
        c5 = cos( q[ 4 ] );
        s1s23 = s1 * s23;
        s1c23 = s1 * c23;
        c1s23 = c1 * s23;
        c1c23 = c1 * c23;

        dWh = sin( q[ 1 ] ) * l2 + s23 * l3;
        dWv = cos( q[ 1 ] ) * l2 + c23 * l3;

        if ( fabs( dWh ) < KINEMATICS_EPS )
            return false;

        if ( fabs( s5 ) < KINEMATICS_EPS )
            return false;

        CONST1 = l2 * sin( q[ 2 ] );

        if ( fabs( CONST1 ) < KINEMATICS_EPS )
            return false;

        /* Position of wrist: */
        Pwx = - s1 * dWh;

        Pwy = + c1 * dWh;

        Pwz = l1 + dWv;

        /* Eqs. (44)-(50), Low & Dubey: */
        JInv[ 0 ][ 0 ] = -c1 / dWh;

        JInv[ 0 ][ 1 ] = -s1 / dWh;

        JInv[ 0 ][ 2 ] = 0.0;

        JInv[ 0 ][ 3 ] = s1 * Pwz / dWh;

        JInv[ 0 ][ 4 ] = -c1 * Pwz / dWh;

        JInv[ 0 ][ 5 ] = ( c1 * Pwy - s1 * Pwx ) / dWh;

        JInv[ 1 ][ 0 ] = -s1s23 / CONST1;

        JInv[ 1 ][ 1 ] = c1s23 / CONST1;

        JInv[ 1 ][ 2 ] = c23 / CONST1;

        JInv[ 1 ][ 3 ] = ( -c1s23 * Pwz + c23 * Pwy ) / CONST1;

        JInv[ 1 ][ 4 ] = -( s1s23 * Pwz + c23 * Pwx ) / CONST1;

        JInv[ 1 ][ 5 ] = ( c1s23 * Pwx + s1s23 * Pwy ) / CONST1;

        dWv = dWv / dWh;

        dWh = dWh / CONST1 / l3;

        JInv[ 2 ][ 0 ] = s1 * dWh;

        JInv[ 2 ][ 1 ] = -c1 * dWh;

        JInv[ 2 ][ 2 ] = -dWv * dWh;

        JInv[ 2 ][ 3 ] = dWh * ( -dWv * Pwy + c1 * Pwz );

        JInv[ 2 ][ 4 ] = dWh * ( dWv * Pwx + s1 * Pwz );

        JInv[ 2 ][ 5 ] = -dWh * ( s1 * Pwy + c1 * Pwx );

        s23s4 = s23 * s4;

        JInv[ 4 ][ 0 ] = -c4 * ( JInv[ 1 ][ 0 ] + JInv[ 2 ][ 0 ] ) - s23s4 * JInv[ 0 ][ 0 ];

        JInv[ 4 ][ 1 ] = -c4 * ( JInv[ 1 ][ 1 ] + JInv[ 2 ][ 1 ] ) - s23s4 * JInv[ 0 ][ 1 ];

        JInv[ 4 ][ 2 ] = -c4 * ( JInv[ 1 ][ 2 ] + JInv[ 2 ][ 2 ] );

        JInv[ 4 ][ 3 ] = -c4 * ( JInv[ 1 ][ 3 ] + JInv[ 2 ][ 3 ] + c1 ) - s23s4 * JInv[ 0 ][ 3 ] + s1c23 * s4;

        JInv[ 4 ][ 4 ] = -c4 * ( JInv[ 1 ][ 4 ] + JInv[ 2 ][ 4 ] + s1 ) - s23s4 * JInv[ 0 ][ 4 ] - c1c23 * s4;

        JInv[ 4 ][ 5 ] = -c4 * ( JInv[ 1 ][ 5 ] + JInv[ 2 ][ 5 ] ) - s23s4 * ( JInv[ 0 ][ 5 ] - 1.0 );

        s23c4 = s23 * c4;

        JInv[ 5 ][ 0 ] = ( s23c4 * JInv[ 0 ][ 0 ] - s4 * ( JInv[ 1 ][ 0 ] + JInv[ 2 ][ 0 ] ) ) / s5;

        JInv[ 5 ][ 1 ] = ( s23c4 * JInv[ 0 ][ 1 ] - s4 * ( JInv[ 1 ][ 1 ] + JInv[ 2 ][ 1 ] ) ) / s5;

        JInv[ 5 ][ 2 ] = ( s23c4 * JInv[ 0 ][ 2 ] - s4 * ( JInv[ 1 ][ 2 ] + JInv[ 2 ][ 2 ] ) ) / s5;

        JInv[ 5 ][ 3 ] = ( s23c4 * JInv[ 0 ][ 3 ] - s1c23 * c4 - s4 * ( JInv[ 1 ][ 3 ] + JInv[ 2 ][ 3 ] + c1 ) ) / s5;

        JInv[ 5 ][ 4 ] = ( s23c4 * JInv[ 0 ][ 4 ] + c1c23 * c4 - s4 * ( JInv[ 1 ][ 4 ] + JInv[ 2 ][ 4 ] + s1 ) ) / s5;

        JInv[ 5 ][ 5 ] = ( s23c4 * ( JInv[ 0 ][ 5 ] - 1.0 ) - s4 * ( JInv[ 1 ][ 5 ] + JInv[ 2 ][ 5 ] ) ) / s5;

        JInv[ 3 ][ 0 ] = -c23 * JInv[ 0 ][ 0 ] - c5 * JInv[ 5 ][ 0 ];

        JInv[ 3 ][ 1 ] = -c23 * JInv[ 0 ][ 1 ] - c5 * JInv[ 5 ][ 1 ];

        JInv[ 3 ][ 2 ] = -c5 * JInv[ 5 ][ 2 ];

        JInv[ 3 ][ 3 ] = -c23 * JInv[ 0 ][ 3 ] - c5 * JInv[ 5 ][ 3 ] - s1s23;

        JInv[ 3 ][ 4 ] = -c23 * JInv[ 0 ][ 4 ] - c5 * JInv[ 5 ][ 4 ] + c1s23;

        JInv[ 3 ][ 5 ] = c23 * ( 1.0 - JInv[ 0 ][ 5 ] ) - c5 * JInv[ 5 ][ 5 ];

        return true;
    }
}
