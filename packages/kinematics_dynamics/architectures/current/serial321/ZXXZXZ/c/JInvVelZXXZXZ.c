/*
** NAME: JInvVelZXXZXZ.c
** CREATED: 27 Jan 1993
** AUTHOR: Herman Bruyninckx, Dept. Mechanical Engineering,
**  Division PMA, Katholieke Universiteit Leuven, Belgium
**
** FUNCTION: Calculation of the inverse VELOCITY kinematics
**  of a 6 rotational dof robot with no shoulder offset, and no
**  eccentricities. The kinematical definitions are as
**  described in the references. 
**  ******************************************************
**  ***   Only the input configuration is calculated.  ***
**  ***                                                ***
**  ***         The joint positions are known          ***
**  ******************************************************
**
** REFERENCES: - Low and Dubey, Int.J.Rob.Research,Vol. 5, No. 4, 1986
**  - Brochier en Haverhals, Studie voor on-line toepassing van
**    inverse kinematika en dynamica, Thesis 1988.
**  - "InvVelZXXZXZ.c"
**  - "feathZXXZXZ.doc"
**  - "Kine6R.h"
**
** EXPORTS: int JInvVelZXXZXZ();
**
** INPUT: `p_d': pointer to ``Kine6RData'' struct; used fields:
**  `status':desired configuration.
**  `q': joint angles.
**  `vel': end effector twist, expressed wrt the base.
** RETURNS: `0': if desired output can be calculated;
**  `1': otherwise.
** OUTPUT: `status':possible singularities.
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


int JInvVelZXXZXZ ( Kine6RData *p_d )

{
    double P6x, P6y, P6z; /* Coordinates (in the base frame) of the 6th link */
    double Vwx, Vwy, Vwz; /* Translational velocity components of wrist */
    double Wwx, Wwy, Wwz /* Rotational velocity components of wrist */;
    double R;  /* square of XYZ distance betw. wrist & base of link2 */
    double s1, c1, s2, c2, s3, c3, s4, c4, s5, c5, s6, c6; /* sin,cos of thetai */
    double s23, c23, s1c23, c1c23, s5s6, s5c6; /* temporary... */
    double c4s5, s4s5, s1s23, c1s23;
    double d13, d23;
    double temp1, temp2, temp3;   /* ...storage */


    /*---  VRIJWILLIG INGEZETTE FOUT ------------------------------------------*/
    a =

        /*
          Vooraleer deze fout te verwijderen, ga na of de kinenatica van de robot
          goed geimplementeerd is. Lees in het intern rapport 93R20, Wim Persoons,
          hoe je de waarden van de Jacobiaan, de inverse en voorwaartse snelheids
          transformatie algorithmes moet interpreteren en hoe je de kinematica bij
          moet schaven.
         
          Voor vragen kan je terecht bij Herman of bij WimP
         
          19 juli 1993
        ----------------------------------------------------------------------------*/



        c1 = cos( p_d->q[ 0 ] );
    s1 = sin( p_d->q[ 0 ] );
    c23 = cos( p_d->q[ 1 ] + p_d->q[ 2 ] );
    s23 = sin( p_d->q[ 1 ] + p_d->q[ 2 ] );
    c2 = cos( p_d->q[ 1 ] );
    s2 = sin( p_d->q[ 1 ] );
    c3 = cos( p_d->q[ 2 ] );
    s3 = sin( p_d->q[ 2 ] );
    /* c2,s2,s3,s3 waren in oorspronkelijke versie niet gedeclareerd
     s3 wordt in ieder geval verder gebruikt SDE */
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
    /* hierboven stond s23  = c1 * s23 */
    /* veranderd door SDE , 7/5/93 */
    s1c23 = s1 * c23;
    s1s23 = s1 * s23;

    p_d->t6.R[ 0 ][ 2 ] = -s1c23 * c4s5 - c1 * s4s5 - s1s23 * c5;
    p_d->t6.R[ 1 ][ 2 ] = c1c23 * c4s5 - s1 * s4s5 + c1s23 * c5;
    p_d->t6.R[ 2 ][ 2 ] = - s23 * c4s5 + c23 * c5;

    /* Coordinates of last link wrt base: */
    P6x = p_d->t6.R[ 0 ][ 2 ] * p_d->l6;
    P6y = p_d->t6.R[ 1 ][ 2 ] * p_d->l6;
    P6z = p_d->t6.R[ 2 ][ 2 ] * p_d->l6;

    /* vertical distance of wrist from shoulder: */
    p_d->dWv = cos( p_d->q[ 1 ] ) * p_d->l2 + c23 * p_d->l3;
    /* horizontal distance of wrist from shoulder: */
    p_d->dWh = sin( p_d->q[ 1 ] ) * p_d->l2 + s23 * p_d->l3;

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

    /* If singularities are detected, it is useless to calculate joint velocities
       or accelerations, because some of them will go to infinity... */
    if ( p_d->status & SING_BITS )
        return ( 1 );


    /* Inverse velocity calculations: */

    /* Calculation of wrist velocity: Low & Dubey, eq.(47) */
    Vwx = p_d->vel[ 0 ] + p_d->vel[ 4 ] * p_d->Pwz - p_d->vel[ 5 ] * p_d->Pwy;

    Vwy = p_d->vel[ 1 ] + p_d->vel[ 5 ] * p_d->Pwx - p_d->vel[ 3 ] * p_d->Pwz;

    Vwz = p_d->vel[ 2 ] + p_d->vel[ 3 ] * p_d->Pwy - p_d->vel[ 4 ] * p_d->Pwx;

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
