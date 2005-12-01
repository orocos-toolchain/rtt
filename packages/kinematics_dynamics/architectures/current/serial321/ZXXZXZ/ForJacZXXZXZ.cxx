/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  ForJacZXXZXZ.cxx 

                        ForJacZXXZXZ.cxx -  description
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
** FILE: ForJacZXXZXZ.c
** CREATED: 6 Dec 1992
** AUTHOR: Herman Bruyninckx, PMA, KU Leuven, Belgium.
**
** FUNCTION: Calculates the jacobian matrix of a robot with 6 revolute
**  joints, no offsets, and no eccentricities.
**  The Jacobian maps joint velocities to end effector velocities,
**  expressed with respect to the base frame:
**
**   |v_ee(x)|        |qdot[0]|
**   |   .   | =   J  |   .   |
**   |w_ee(z)|        |qdot[5]|
**
**  `J' is a function of `q[i]'.
**
** ALGORITHM: From "ForVelZXXZXZ()()", we wrote out in full the equations
**  linking `qdot[i]' with `v_ee(x)'...`w_ee(z)'.
**
** REFERENCES: - Featherstone, R., I. J. Rob. Research, 1983.
**  - Brochier en Haverhals, Studie voor on-line toepassing van
**    inverse kinematika en dynamica, Thesis 1988.
**  - "feathZXXZXZ.doc"
**  - "Kine6R.h"
**  - "ForVelZXXZXZ.c"
**
** EXPORTS: int ForJacZXXZXZ()
**
** INPUT: `p_d': pointer to ``Kine6RData'' struct; used fields:
**  `q': current joint angles.
** OUTPUT: `J': Jacobian matrix.
**  `p_d->status': coded configurations and singularities.
**
** RETURNS: `0' if no singularities occured.
**  `1' otherwise.
**
** CAVEATS: Eq.(37) of Featherstone is wrong in the Y component!
**  The exact equation is given in Eq.(32).
*/

#include <math.h> /* cos,sin,fabs */
#include "kindyn/SerialZXXZXZ.hpp"

#define SQRT3t2 (double)(3.46410161513775) /* 2 sqrt(3) */

namespace ORO_KinDyn
{
    using namespace ORO_CoreLib;

    bool SerialZXXZXZ::jacobianForward( const Double6D& q, double J[ 6 ][ 6 ], Singularity& s ) const
    {

        double c1, c23, c4, c5, s1, s23, s4, s5, s4s5, c4s5;
        double c1c23, c1s23, s1c23, s1s23;
        double Pwx, Pwy, Pwz; /* Coordinates of the wrist */
        double dWv, dWh;
        
        s.singularitySet( Singularity::None );
  
        c1 = cos( q[ 0 ] );
        s1 = sin( q[ 0 ] );
        c23 = cos( q[ 1 ] + q[ 2 ] );
        s23 = sin( q[ 1 ] + q[ 2 ] );
        c4 = cos( q[ 3 ] );
        s4 = sin( q[ 3 ] );
        c5 = cos( q[ 4 ] );
        s5 = sin( q[ 4 ] );
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

        /* Calculation of configuration: */
#if 0
        if ( dWh >= 0. )
            set( Serial321Configuration::Forward );
        else
            set( Serial321Configuration::Backward );

        if ( q[ 4 ] < 0. )
            set( Serial321Configuration::Flip );

        if ( q[ 2 ] < 0. )
            set( Serial321Configuration::ElbowDown );
#endif
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
