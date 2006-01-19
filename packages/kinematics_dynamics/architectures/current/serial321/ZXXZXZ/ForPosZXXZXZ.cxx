/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  ForPosZXXZXZ.cxx 

                        ForPosZXXZXZ.cxx -  description
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
** NAME: ForPosZXXZXZ.c
** CREATED: 9 nov 1992
** AUTHOR: Herman Bruyninckx, PMA, KU Leuven, Belgium.
**
** EXPORTS: ForPosZXXZXZ();
**
** FUNCTION: calculation of the forward POSITION kinematics of
**  a 6 rotational dof robot with no shoulder offset or
**  eccentricity.
**  The kinematical definitions are as described in the references.
**
** INPUT: `p_d': pointer to ``Kine6RData'' struct. Used fields are:
**  `q': the current joint angles.
** OUTPUT: `p_d': used fields:
**  `t6': pointer to the homogeneous transform expressing
**   the location of the end effector wrt the base;
**  `status': pointer to coded configuration and singularities;
**
** CAVEATS: joint angles are THEORETICAL, i.e., NO range checking is done!!
**
** REFERENCE: - Featherstone, Int.J.Rob.Research, Vol. 2, 1983.
**  - Low and Dubey, Int.J.Rob.Research,Vol. 5, No. 4, 1986
**  - Brochier en Haverhals, Studie voor on-line toepassing van
**    inverse kinematika en dynamica, Thesis 1988.
**  - "feathZXXZXZ.doc"
**  - "Kine6R.h"
*/

#include "kindyn/SerialZXXZXZ.hpp"
#include <geometry/frames.h>

namespace ORO_KinDyn
{

    using namespace ORO_CoreLib;

    /**
     * 
     * @param q the current joint angles
     * @param mp_base homegeneous transform expressing the location of the end effector wrt the base
     * @return True on success, false on failure
     * 
     */
    bool SerialZXXZXZ::positionForward( const Double6D& q, ORO_Geometry::Frame& mp_base, Singularity& s ) const
    {
        double c1, c23, c4, c5, c6, s1, s23, s4, s5, s6;
        double c1c23, c1s23, c4s5, s4s5, s1c23, s1s23, s5c6, s5s6;
        double P6x, P6y, P6z; /* Coordinates of the last link */
        double R;  /* distance (squared) shoulder-wrist */
        double dWh, dWv;
        double Pwx, Pwy, Pwz;


        c1 = cos( q[ 0 ] );
        s1 = sin( q[ 0 ] );
        c23 = cos( q[ 1 ] + q[ 2 ] );
        s23 = sin( q[ 1 ] + q[ 2 ] );
        c4 = cos( q[ 3 ] );
        s4 = sin( q[ 3 ] );
        c5 = cos( q[ 4 ] );
        s5 = sin( q[ 4 ] );
        c6 = cos( q[ 5 ] );
        s6 = sin( q[ 5 ] );
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
        mp_base.M( 0, 0 ) = -s1c23 * mp_base.M( 2, 0 ) - c1 * mp_base.M( 1, 0 ) + s1s23 * s5s6;//OK
        mp_base.M( 1, 0 ) = c1c23 * mp_base.M( 2, 0 ) - s1 * mp_base.M( 1, 0 ) - c1s23 * s5s6;//OK
	mp_base.M( 2, 0 ) = -s23 * mp_base.M( 2, 0 ) - c23 * s5s6;//OK

        mp_base.M( 1, 1 ) = s4 * c5 * c6 + c4 * s6;
        mp_base.M( 2, 1 ) = c4 * c5 * c6 - s4 * s6;
        mp_base.M( 0, 1 ) = -s1c23 * mp_base.M( 2, 1 ) - c1 * mp_base.M( 1, 1 ) + s1s23 * s5c6;//OK
        mp_base.M( 1, 1 ) = c1c23 * mp_base.M( 2, 1 ) - s1 * mp_base.M( 1, 1 ) - c1s23 * s5c6;//OK
        mp_base.M( 2, 1 ) = - s23 * mp_base.M( 2, 1 ) - c23 * s5c6;//OK


        mp_base.M( 0, 2 ) = - s1c23 * c4s5 - c1 * s4s5 - s1s23 * c5;//OK
        mp_base.M( 1, 2 ) = c1c23 * c4s5 - s1 * s4s5 + c1s23 * c5;//OK
        mp_base.M( 2, 2 ) = - s23 * c4s5 + c23 * c5;//OK

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

        /* CALCULATION OF CONFIGURATION: */
/*
        if ( dWh >= 0. )
            set( Serial321Configuration::Forward );
        else
            set( Serial321Configuration::Backward );

        if ( q[ 4 ] < 0. )
            set( Serial321Configuration::Flip );

        if ( q[ 2 ] < 0. )
            set( Serial321Configuration::ElbowDown );
*/
        /* CALCULATION OF SINGULARITIES: */
        s.singularitySet( Singularity::None );
        if ( fabs( dWh ) < EPS_WRIST_ABOVE_BASE )
            s.singularitySet( Singularity::WristAboveBase );

        R = dWv * dWv + dWh * dWh;

        if ( R > ( max_wrist_dist - EPS_ARM_EXTENDED * EPS_ARM_EXTENDED ) )
        {
            s.singularitySet( Singularity::ArmExtended );

            if ( R > max_wrist_dist )
                s.singularitySet( Singularity::OutOfReach );
        }

        if ( fabs( s5 ) < EPS_PARALLEL_WRIST )
            s.singularitySet( Singularity::ParallelWrist );

        if ( s.isSingular( Singularity::AnySingularity ) )
            return false;
        else
            return true;
    }
}
