/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  InvVelZXXZXZ.c 

                        InvVelZXXZXZ.c -  description
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
** NAME: InvVelZXXZXZ.c
** CREATED: 6 Dec 1992
** AUTHOR: Herman Bruyninckx, PMA, KULeuven, Belgium
**
** FUNCTION: Calculation of the inverse POSITION and VELOCITY kinematics
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
** EXPORTS: int InvVelZXXZXZ();
**
** INPUT: `p_d': pointer to ``Kine6RData'' struct; used fields:
**  `status':desired configuration.
**  `t6': the homogeneous transformation matrix of the end
**   effector wrt the base.
**  `vel': end effector twist, expressed wrt the base.
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
**  At singularities, we made the choice
**  1) to let ``InvVelZXXZXZ()'' return the OLD joint values, i.e.
**     these given TO the function (or whatever is contained in `q'
**     on entry!).
**  2) not to calculate `qdot'.
*/

#include <math.h> /* cos,sin,fabs,atan2 */
#include "Status6R.h" /* Status6R,
BACKWARD, FORWARD, ELBOW_UP, ELBOW_DOWN, FLIP, NO_FLIP,
SING_WRIST_ABOVE_BASE, SING_ARM_EXTENDED,
SING_PARALLEL_WRIST, SING_OUT_OF_REACH, SING_BITS,
EPS_WRIST_ABOVE_BASE, EPS_ARM_EXTENDED, EPS_PARALLEL_WRIST,
KINEMATICS_EPS * /
#include "Kine6R.h" /* Kine6RData */


int InvVelZXXZXZ ( Kine6RData *p_d )

{
    double P6x, P6y, P6z; /* Coordinates (in the base frame) of the 6th link */
    double Vwx, Vwy, Vwz; /* Translational velocity components of wrist */
    double Wwx, Wwy, Wwz /* Rotational velocity components of wrist */;
    double Rxy_2;  /* square of XY distance betw. wrist & base of link2 */
    double Rxyz_2;  /* square of XYZ distance betw. wrist & base of link2 */
    double s1, c1, s2, c2, s3, c3, s4, c4, s5, c5, s6, c6; /* sin,cos of thetai */
    double s23, c23, s1c23, c1c23; /* temporary... */
    double d13, d23;
    double temp1, temp2, temp3;   /* ...storage */


    /* Clear singularity bits without changing configuration code: */
    p_d->status &= ~( SING_BITS );

    /* Coordinates of last link wrt base: */
    P6x = p_d->t6.R[ 0 ][ 2 ] * p_d->l6;
    P6y = p_d->t6.R[ 1 ][ 2 ] * p_d->l6;
    P6z = p_d->t6.R[ 2 ][ 2 ] * p_d->l6;

    /* Wrist position: (Low & Dubey eq.(34)) */
    p_d->Pwx = p_d->t6.p[ 0 ] - P6x;
    p_d->Pwy = p_d->t6.p[ 1 ] - P6y;
    p_d->Pwz = p_d->t6.p[ 2 ] - P6z;

    /* Calculation of relevant lengths: */
    p_d->dWv = p_d->Pwz - p_d->l1;
    Rxy_2 = p_d->Pwx * p_d->Pwx + p_d->Pwy * p_d->Pwy;
    Rxyz_2 = Rxy_2 + p_d->dWv * p_d->dWv;

    if ( p_d->status & BACKWARD )
        p_d->dWh = -sqrt( Rxy_2 );
    else
        p_d->dWh = sqrt( Rxy_2 );

    /*  Arm extended? */
    if ( Rxyz_2 > ( p_d->max_wrist_dist - EPS_ARM_EXTENDED * EPS_ARM_EXTENDED ) )
    {
        p_d->status |= SING_ARM_EXTENDED;
        /* Out of reach? */

        if ( Rxyz_2 > p_d->max_wrist_dist )
            p_d->status |= SING_OUT_OF_REACH;

        return ( 1 );
    }

    /* Wrist is above shoulder? */
    if ( fabs( p_d->dWh ) < EPS_WRIST_ABOVE_BASE )
    {
        p_d->status |= SING_WRIST_ABOVE_BASE;
        return ( 1 );
    }


    /* Calculation of q[0] by geometry of triangle formed by dWh and Rxy_2: */
    s1 = -p_d->Pwx * p_d->dWh / Rxy_2;

    c1 = p_d->Pwy * p_d->dWh / Rxy_2;

    p_d->q[ 0 ] = atan2( s1, c1 );

    /* Calculation of q[2]: (formulas (B6) and (36), Low and Dubey) */
    c3 = ( Rxyz_2 - p_d->const1 ) / p_d->const2;

    if ( p_d->status & ELBOW_DOWN )
        s3 = -sqrt( 1.0 - c3 * c3 );
    else
        s3 = sqrt( 1.0 - c3 * c3 );

    p_d->q[ 2 ] = atan2( s3, c3 );

    /* Calculation of q[1]: (formulas (37), Low and Dubey) */
    temp1 = p_d->l3 * c3 + p_d->l2;

    temp2 = p_d->l3 * s3;

    c2 = ( p_d->dWv * temp1 + p_d->dWh * temp2 ) / Rxyz_2;

    s2 = ( p_d->dWh - c2 * temp2 ) / temp1;

    p_d->q[ 1 ] = atan2( s2, c2 );

    /* Orientation of wrist wrt arm: Dij (formulas (42), Low and Dubey) */
    s23 = s2 * c3 + c2 * s3;

    c23 = c2 * c3 - s2 * s3;

    s1c23 = s1 * c23;

    c1c23 = c1 * c23;

    d13 = p_d->t6.R[ 0 ][ 2 ] * c1 + p_d->t6.R[ 1 ][ 2 ] * s1;

    d23 = -p_d->t6.R[ 0 ][ 2 ] * s1c23 + p_d->t6.R[ 1 ][ 2 ] * c1c23 - p_d->t6.R[ 2 ][ 2 ] * s23;

    /* Calculation of q[3], q[4] and q[5], eqs.(39)-(41) Low & Dubey: */
    if ( ( fabs( d13 ) < KINEMATICS_EPS ) && ( fabs( d23 ) < KINEMATICS_EPS ) )
    {
        /* Z-axes of links 4 and 6 are parallel! q[3] keeps its previous value! */
        s4 = sin( p_d->q[ 3 ] );
        c4 = cos( p_d->q[ 3 ] );
        s5 = 0.0;
        p_d->q[ 4 ] = 0.0;
    }

    else
    {
        if ( p_d->status & FLIP )
            p_d->q[ 3 ] = atan2( d13, -d23 );
        else
            p_d->q[ 3 ] = atan2( -d13, d23 );

        c4 = cos( p_d->q[ 3 ] );

        if ( fabs( d23 ) > KINEMATICS_EPS )
            s4 = - c4 * d13 / d23;
        else
            s4 = sin( p_d->q[ 3 ] );

        c5 = -p_d->t6.R[ 0 ][ 2 ] * s1 * s23 + p_d->t6.R[ 1 ][ 2 ] * c1 * s23 + p_d->t6.R[ 2 ][ 2 ] * c23;

        s5 = d23 * c4 - d13 * s4;

        p_d->q[ 4 ] = atan2( s5, c5 );
    }

    if ( fabs( s5 ) < EPS_PARALLEL_WRIST )
        p_d->status |= SING_PARALLEL_WRIST;

    s6 = -( p_d->t6.R[ 0 ][ 1 ] * c1 + p_d->t6.R[ 1 ][ 1 ] * s1 ) * c4 + ( p_d->t6.R[ 0 ][ 1 ] * s1c23 - p_d->t6.R[ 1 ][ 1 ] * c1c23 + p_d->t6.R[ 2 ][ 1 ] * s23 ) * s4;

    c6 = ( p_d->t6.R[ 0 ][ 0 ] * c1 + p_d->t6.R[ 1 ][ 0 ] * s1 ) * c4 + ( -p_d->t6.R[ 0 ][ 0 ] * s1c23 + p_d->t6.R[ 1 ][ 0 ] * c1c23 - p_d->t6.R[ 2 ][ 0 ] * s23 ) * s4;

    p_d->q[ 5 ] = atan2( s6, c6 );

    /* If singularities are detected, it is useless to calculate joint velocities
       or accelerations, because some of them will go to infinity... */
    if ( p_d->status & SING_BITS )
        return ( 1 );


    /* Inverse velocity calculations: */

    /* Calculation of wrist velocity: Low & Dubey, eq.(47) */
    Vwx = p_d->vel[ 0 ] - p_d->vel[ 4 ] * P6z + p_d->vel[ 5 ] * P6y;

    Vwy = p_d->vel[ 1 ] - p_d->vel[ 5 ] * P6x + p_d->vel[ 3 ] * P6z;

    Vwz = p_d->vel[ 2 ] - p_d->vel[ 3 ] * P6y + p_d->vel[ 4 ] * P6x;

    /* First, second and third joint velocities: eqs.(44)-(45), Low & Dubey,
       eqs.(6.24),(6.25) Brochier & Haverhals */
    p_d->qdot[ 0 ] = -( c1 * Vwx + s1 * Vwy ) / p_d->dWh;

    temp3 = c1 * Vwy - s1 * Vwx;

    p_d->qdot[ 1 ] = ( s23 * temp3 + c23 * Vwz ) / p_d->l2 / s3;

    p_d->qdot[ 2 ] = -( p_d->dWh * temp3 + p_d->dWv * Vwz ) / p_d->l2 / p_d->l3 / s3;

    /* Velocity of end effector wrt the wrist: */
    Wwx = c1 * p_d->vel[ 3 ] + s1 * p_d->vel[ 4 ] + p_d->qdot[ 1 ] + p_d->qdot[ 2 ];

    Wwy = c1 * p_d->vel[ 4 ] - s1 * p_d->vel[ 3 ];

    Wwz = c23 * Wwy - s23 * ( p_d->vel[ 5 ] - p_d->qdot[ 0 ] );

    /* Fourth, fifth and sixth joint velocities: eqs.(48)-(50) Low & Dubey */
    p_d->qdot[ 4 ] = -c4 * Wwx - s4 * Wwz;

    p_d->qdot[ 5 ] = ( c4 * Wwz - s4 * Wwx ) / s5;

    p_d->qdot[ 3 ] = c23 * ( p_d->vel[ 5 ] - p_d->qdot[ 0 ] ) + s23 * Wwy - c5 * p_d->qdot[ 5 ];

    return ( 0 );
}
