/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  ForPosZXXZXZ.c 

                        ForPosZXXZXZ.c -  description
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

#include <math.h> /* cos,sin,fabs */
#include "Status6R.h" /* Status6R,
BACKWARD, FORWARD, ELBOW_UP, ELBOW_DOWN, FLIP, NO_FLIP,
SING_WRIST_ABOVE_BASE, SING_ARM_EXTENDED,
SING_PARALLEL_WRIST,
EPS_WRIST_ABOVE_BASE, EPS_ARM_EXTENDED, EPS_PARALLEL_WRIST
* /
#include "Kine6R.h" /* Kine6RData */


int ForPosZXXZXZ ( Kine6RData *p_d )

{
    double c1, c23, c4, c5, c6, s1, s23, s4, s5, s6;
    double c1c23, c1s23, c4s5, s4s5, s1c23, s1s23, s5c6, s5s6;
    double P6x, P6y, P6z; /* Coordinates of the last link */
    double R;  /* distance (squared) shoulder-wrist */

    c1 = cos( p_d->q[ 0 ] );
    s1 = sin( p_d->q[ 0 ] );
    c23 = cos( p_d->q[ 1 ] + p_d->q[ 2 ] );
    s23 = sin( p_d->q[ 1 ] + p_d->q[ 2 ] );
    c4 = cos( p_d->q[ 3 ] );
    s4 = sin( p_d->q[ 3 ] );
    c5 = cos( p_d->q[ 4 ] );
    s5 = sin( p_d->q[ 4 ] );
    c6 = cos( p_d->q[ 5 ] );
    s6 = sin( p_d->q[ 5 ] );
    s5c6 = s5 * c6;
    s5s6 = s5 * s6;
    c4s5 = c4 * s5;
    s4s5 = s4 * s5;
    c1c23 = c1 * c23;
    c1s23 = c1 * s23;
    s1c23 = s1 * c23;
    s1s23 = s1 * s23;

    p_d->t6.R[ 1 ][ 0 ] = s4 * c5 * s6 - c4 * c6;
    p_d->t6.R[ 2 ][ 0 ] = c4 * c5 * s6 + s4 * c6;
    p_d->t6.R[ 0 ][ 0 ] = -s1c23 * p_d->t6.R[ 2 ][ 0 ] - c1 * p_d->t6.R[ 1 ][ 0 ] + s1s23 * s5s6;
    p_d->t6.R[ 1 ][ 0 ] = c1c23 * p_d->t6.R[ 2 ][ 0 ] - s1 * p_d->t6.R[ 1 ][ 0 ] - c1s23 * s5s6;
    p_d->t6.R[ 2 ][ 0 ] = - s23 * p_d->t6.R[ 2 ][ 0 ] - c23 * s5s6;

    p_d->t6.R[ 1 ][ 1 ] = s4 * c5 * c6 + c4 * s6;
    p_d->t6.R[ 2 ][ 1 ] = c4 * c5 * c6 - s4 * s6;
    p_d->t6.R[ 0 ][ 1 ] = -s1c23 * p_d->t6.R[ 2 ][ 1 ] - c1 * p_d->t6.R[ 1 ][ 1 ] + s1s23 * s5c6;
    p_d->t6.R[ 1 ][ 1 ] = c1c23 * p_d->t6.R[ 2 ][ 1 ] - s1 * p_d->t6.R[ 1 ][ 1 ] - c1s23 * s5c6;
    p_d->t6.R[ 2 ][ 1 ] = - s23 * p_d->t6.R[ 2 ][ 1 ] - c23 * s5c6;

    p_d->t6.R[ 0 ][ 2 ] = -s1c23 * c4s5 - c1 * s4s5 - s1s23 * c5;
    p_d->t6.R[ 1 ][ 2 ] = c1c23 * c4s5 - s1 * s4s5 + c1s23 * c5;
    p_d->t6.R[ 2 ][ 2 ] = - s23 * c4s5 + c23 * c5;

    /* vertical distance of wrist from shoulder: */
    p_d->dWv = cos( p_d->q[ 1 ] ) * p_d->l2 + c23 * p_d->l3;
    /* horizontal distance of wrist from shoulder: */
    p_d->dWh = sin( p_d->q[ 1 ] ) * p_d->l2 + s23 * p_d->l3;

    /* Position of last link: */
    P6x = p_d->t6.R[ 0 ][ 2 ] * p_d->l6;
    P6y = p_d->t6.R[ 1 ][ 2 ] * p_d->l6;
    P6z = p_d->t6.R[ 2 ][ 2 ] * p_d->l6;

    /* Position of wrist: */
    p_d->Pwx = - s1 * p_d->dWh;
    p_d->Pwy = + c1 * p_d->dWh;
    p_d->Pwz = p_d->l1 + p_d->dWv;

    /* End effector position = position of last link + position of wrist: */
    p_d->t6.p[ 0 ] = P6x + p_d->Pwx;
    p_d->t6.p[ 1 ] = P6y + p_d->Pwy;
    p_d->t6.p[ 2 ] = P6z + p_d->Pwz;

    /* CALCULATION OF CONFIGURATION: */

    if ( p_d->dWh >= 0. )
        p_d->status = FORWARD;
    else
        p_d->status = BACKWARD;

    if ( p_d->q[ 4 ] < 0. )
        p_d->status |= FLIP;

    if ( p_d->q[ 2 ] < 0. )
        p_d->status |= ELBOW_DOWN;

    /* CALCULATION OF SINGULARITIES: */
    if ( fabs( p_d->dWh ) < EPS_WRIST_ABOVE_BASE )
        p_d->status |= SING_WRIST_ABOVE_BASE;

    R = p_d->dWv * p_d->dWv + p_d->dWh * p_d->dWh;

    if ( R > ( p_d->max_wrist_dist - EPS_ARM_EXTENDED * EPS_ARM_EXTENDED ) )
    {
        p_d->status |= SING_ARM_EXTENDED;

        if ( R > p_d->max_wrist_dist )
            p_d->status |= SING_OUT_OF_REACH;
    }

    if ( fabs( s5 ) < EPS_PARALLEL_WRIST )
        p_d->status |= SING_PARALLEL_WRIST;

    if ( p_d->status & SING_BITS )
        return ( 1 );

    return ( 0 );
}
