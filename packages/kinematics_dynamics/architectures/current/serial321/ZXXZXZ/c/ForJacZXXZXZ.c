/*  N O T  F U L L Y  I M P L E M E N T E D !!!
** FILE: ForJacZXXZXZ.c
** CREATED: 6 Dec 1992
** AUTHOR: Herman Bruyninckx, PMA, KU Leuven, Belgium.
**
** FUNCTION: Calculates the jacobian matrix of a robot with 6 revolute
**  joints, no offsets, and no eccentricities.
**  The Jacobian maps joint velocities to end effector velocities,
**  expressed with respect to the base frame:
**
**   |v_ee(x)|  |qdot[0]|
**   |   .   | =   J  |   .   |
**   |w_ee(z)|  |qdot[5]|
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
#include "Status6R.h" /* Status6R,
BACKWARD, FORWARD, ELBOW_UP, ELBOW_DOWN, FLIP, NO_FLIP,
SING_WRIST_ABOVE_BASE, SING_ARM_EXTENDED,
SING_PARALLEL_WRIST,
EPS_WRIST_ABOVE_BASE, EPS_ARM_EXTENDED, EPS_PARALLEL_WRIST,
KINEMATICS_EPS * /
#include "Kine6R.h" /* Kine6RData */


int ForJacZXXZXZ ( Kine6RData *p_d, double J[ 6 ][ 6 ] )

{
    double c1, c23, c4, c5, s1, s23, s4, s5, s4s5, c4s5;
    double c1c23, c1s23, s1c23, s1s23;
    double Pwx, Pwy, Pwz; /* Coordinates of the wrist */

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
    c4 = cos( p_d->q[ 3 ] );
    s4 = sin( p_d->q[ 3 ] );
    c5 = cos( p_d->q[ 4 ] );
    s5 = sin( p_d->q[ 4 ] );
    c4s5 = c4 * s5;
    s4s5 = s4 * s5;
    c1c23 = c1 * c23;
    c1s23 = c1 * s23;
    s1c23 = s1 * c23;
    s1s23 = s1 * s23;

    p_d->dWv = cos( p_d->q[ 1 ] ) * p_d->l2 + c23 * p_d->l3;
    p_d->dWh = sin( p_d->q[ 1 ] ) * p_d->l2 + s23 * p_d->l3;

    Pwx = -s1 * p_d->dWh;
    Pwy = c1 * p_d->dWh;
    Pwz = p_d->l1 + p_d->dWv;

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
    J[ 0 ][ 0 ] = - c1 * p_d->dWh + Pwy;
    J[ 0 ][ 1 ] = J[ 0 ][ 2 ] = s1 * Pwz;
    J[ 0 ][ 1 ] -= s1 * p_d->dWv;
    J[ 0 ][ 2 ] -= s1c23 * p_d->l3;
    J[ 0 ][ 3 ] = -J[ 5 ][ 3 ] * Pwy + J[ 4 ][ 3 ] * Pwz;
    J[ 0 ][ 4 ] = -J[ 5 ][ 4 ] * Pwy + J[ 4 ][ 4 ] * Pwz;
    J[ 0 ][ 5 ] = -J[ 5 ][ 5 ] * Pwy + J[ 4 ][ 5 ] * Pwz;

    J[ 1 ][ 0 ] = -s1 * p_d->dWh - Pwx;
    J[ 1 ][ 1 ] = J[ 1 ][ 2 ] = -c1 * Pwz;
    J[ 1 ][ 1 ] += c1 * p_d->dWv;
    J[ 1 ][ 2 ] += c1c23 * p_d->l3;
    J[ 1 ][ 3 ] = -J[ 3 ][ 3 ] * Pwz + J[ 5 ][ 3 ] * Pwx;
    J[ 1 ][ 4 ] = -J[ 3 ][ 4 ] * Pwz + J[ 5 ][ 4 ] * Pwx;
    J[ 1 ][ 5 ] = -J[ 3 ][ 5 ] * Pwz + J[ 5 ][ 5 ] * Pwx;

    J[ 2 ][ 0 ] = 0.;
    J[ 2 ][ 1 ] = J[ 2 ][ 2 ] = c1 * Pwy - s1 * Pwx;
    J[ 2 ][ 1 ] -= p_d->dWh;
    J[ 2 ][ 2 ] -= s23 * p_d->l3;
    J[ 2 ][ 3 ] = -J[ 4 ][ 3 ] * Pwx + J[ 3 ][ 3 ] * Pwy;
    J[ 2 ][ 4 ] = -J[ 4 ][ 4 ] * Pwx + J[ 3 ][ 4 ] * Pwy;
    J[ 2 ][ 5 ] = -J[ 4 ][ 5 ] * Pwx + J[ 3 ][ 5 ] * Pwy;

    /* Calculation of configuration: */

    if ( p_d->dWh >= 0. )
        p_d->status = FORWARD;
    else
        p_d->status = BACKWARD;

    if ( p_d->q[ 4 ] < 0. )
        p_d->status |= FLIP;

    if ( p_d->q[ 2 ] < 0. )
        p_d->status |= ELBOW_DOWN;

    /* Calculation of singularities: */
    if ( fabs( p_d->dWh ) < EPS_WRIST_ABOVE_BASE )
        p_d->status |= SING_WRIST_ABOVE_BASE;

    if ( ( p_d->dWv * p_d->dWv + p_d->dWh * p_d->dWh ) >
            ( p_d->max_wrist_dist - EPS_ARM_EXTENDED * EPS_ARM_EXTENDED )
       )
        p_d->status |= SING_ARM_EXTENDED;

    if ( fabs( s5 ) < EPS_PARALLEL_WRIST )
        p_d->status |= SING_PARALLEL_WRIST;

    if ( p_d->status & SING_BITS )
        return ( 1 );

    return ( 0 );
}
