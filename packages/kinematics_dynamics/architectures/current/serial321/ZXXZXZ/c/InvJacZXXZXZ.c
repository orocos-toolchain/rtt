/* N O T  F U L L Y  I M P L E M E N T E D !!!
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
**   |qdot[0]|  |v_ee(x)|
**   |   .   | = JInv |   .   |
**   |qdot[5]|  |w_ee(z)|
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
#include "Status6R.h" /* KINEMATICS_EPS */
#include "Kine6R.h" /* Kine6RData */

int InvJacZXXZXZ ( Kine6RData *p_d, double JInv[ 6 ][ 6 ] )

{
    double Pwx, Pwy, Pwz;    /* Coordinates (in the base frame) of the wrist */
    double s1, c1, s4, c4, s5, c5; /* sin,cos of thetai */
    double s23, c23, s1s23, s1c23, c1s23, c1c23;
    double s23s4, s23c4;
    double CONST1;

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


        s1 = sin( p_d->q[ 0 ] );
    c1 = cos( p_d->q[ 0 ] );
    s23 = sin( p_d->q[ 1 ] + p_d->q[ 2 ] );
    c23 = cos( p_d->q[ 1 ] + p_d->q[ 2 ] );
    s4 = sin( p_d->q[ 3 ] );
    c4 = cos( p_d->q[ 3 ] );
    s5 = sin( p_d->q[ 4 ] );
    c5 = cos( p_d->q[ 4 ] );
    s1s23 = s1 * s23;
    s1c23 = s1 * c23;
    c1s23 = c1 * s23;
    c1c23 = c1 * c23;

    p_d->dWh = sin( p_d->q[ 1 ] ) * p_d->l2 + s23 * p_d->l3;
    p_d->dWv = cos( p_d->q[ 1 ] ) * p_d->l2 + c23 * p_d->l3;

    if ( fabs( p_d->dWh ) < KINEMATICS_EPS )
        return ( 1 );

    if ( fabs( s5 ) < KINEMATICS_EPS )
        return ( 1 );

    CONST1 = p_d->l2 * sin( p_d->q[ 2 ] );

    if ( fabs( CONST1 ) < KINEMATICS_EPS )
        return ( 1 );

    /* Position of wrist: */
    Pwx = - s1 * p_d->dWh;

    Pwy = + c1 * p_d->dWh;

    Pwz = p_d->l1 + p_d->dWv;

    /* Eqs. (44)-(50), Low & Dubey: */
    JInv[ 0 ][ 0 ] = -c1 / p_d->dWh;

    JInv[ 0 ][ 1 ] = -s1 / p_d->dWh;

    JInv[ 0 ][ 2 ] = 0.0;

    JInv[ 0 ][ 3 ] = s1 * Pwz / p_d->dWh;

    JInv[ 0 ][ 4 ] = -c1 * Pwz / p_d->dWh;

    JInv[ 0 ][ 5 ] = ( c1 * Pwy - s1 * Pwx ) / p_d->dWh;

    JInv[ 1 ][ 0 ] = -s1s23 / CONST1;

    JInv[ 1 ][ 1 ] = c1s23 / CONST1;

    JInv[ 1 ][ 2 ] = c23 / CONST1;

    JInv[ 1 ][ 3 ] = ( -c1s23 * Pwz + c23 * Pwy ) / CONST1;

    JInv[ 1 ][ 4 ] = -( s1s23 * Pwz + c23 * Pwx ) / CONST1;

    JInv[ 1 ][ 5 ] = ( c1s23 * Pwx + s1s23 * Pwy ) / CONST1;

    p_d->dWv = p_d->dWv / p_d->dWh;

    p_d->dWh = p_d->dWh / CONST1 / p_d->l3;

    JInv[ 2 ][ 0 ] = s1 * p_d->dWh;

    JInv[ 2 ][ 1 ] = -c1 * p_d->dWh;

    JInv[ 2 ][ 2 ] = -p_d->dWv * p_d->dWh;

    JInv[ 2 ][ 3 ] = p_d->dWh * ( -p_d->dWv * Pwy + c1 * Pwz );

    JInv[ 2 ][ 4 ] = p_d->dWh * ( p_d->dWv * Pwx + s1 * Pwz );

    JInv[ 2 ][ 5 ] = -p_d->dWh * ( s1 * Pwy + c1 * Pwx );

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

    return ( 0 );
}
