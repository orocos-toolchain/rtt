/*
** NAME: ForVelZXXZXZ.c
** CREATED: 6 Dec 1992
** AUTHOR: Herman Bruyninckx, PMA, KU Leuven, Belgium.
**
** EXPORTS: ForVelZXXZXZ();
**
** FUNCTION: calculation of the forward POSITION and VELOCITY kinematics of
**  a 6 rotational dof robot with no shoulder offset or
**  eccentricity.
**  The kinematical definitions are as described in the references.
**
** INPUT: `p_d': pointer to ``Kine6RData'' struct. Used fields are:
**  `q': the current joint angles.
**  `qdot': the current joint velocities.
** OUTPUT: `p_d': used fields:
**  `t6': pointer to the homogeneous transform expressing
**   the location of the end effector wrt the base;
**  `status': pointer to coded configuration and singularities;
**  `vel': end effector velocity twist, expressed wrt the base.
**
** CAVEATS: joint angles are THEORETICAL, i.e., NO range checking is done!!
**
**  Eq.(37) of Featherstone is wrong for the Y-component. The
**  correct equation is given in (32)!
**
** REFERENCE: - Featherstone, Int.J.Rob.Research, Vol. 2, 1983.
**  - Low and Dubey, Int.J.Rob.Research,Vol. 5, No. 4, 1986
**  - Brochier en Haverhals, Studie voor on-line toepassing van
**    inverse kinematika en dynamica, Thesis 1988.
**  - "feathZXXZXZ.doc"
**  - "Kine6R.h"
*/

#include <math.h> /* cos,sin,fabs */
#include "kindyn/SerialZXXZXZ.hpp"

namespace ORO_KinDyn
{

    bool SerialZXXZXZ::velocityForward ( const Double6D& q, const Double6D& qdot, ORO_Geometry::Frame& mp_base, ORO_Geometry::Twist& vel_base, Singularity& s ) const
    {
        double c1, c23, c4, c5, c6, s1, s23, s4, s5, s6;
        double c1c23, c1s23, c4s5, s4s5, s1c23, s1s23, s5c6, s5s6;
        double P6x, P6y, P6z; /* Coordinates of the last link */
        double Wwy, Wwz; /* Angular velocity components of the wrist */
        double Wx, Wy, Vx, Vy; /* Angular and linear velocity components of end eff. */
        double R; /* distance (squared) shoulder-wrist */
        double dWv, dWh;
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
            set( Serial321Configuration::Forward );
        else
            set( Serial321Configuration::Backward );

        if ( q[ 4 ] < 0. )
            set( Serial321Configuration::Flip );

        if ( q[ 2 ] < 0. )
            set( Serial321Configuration::ElbowDown );
#endif

        /* CALCULATION OF SINGULARITIES: */
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

        /* Forward velocity transformation according to Featherstone eqs.(37)-(43) */
        Wwy = -s4 * qdot[ 4 ] + c4s5 * qdot[ 5 ];

        Wwz = qdot[ 3 ] + c5 * qdot[ 5 ];

        Wx = -c4 * qdot[ 4 ] - s4s5 * qdot[ 5 ] - qdot[ 1 ] - qdot[ 2 ];

        Wy = c23 * Wwy + s23 * Wwz;

        vel_base( 3 ) = c1 * Wx - s1 * Wy;

        vel_base( 4 ) = c1 * Wy + s1 * Wx;

        vel_base( 5 ) = qdot[ 0 ] + c23 * Wwz - s23 * Wwy;

        Vx = -dWh * qdot[ 0 ];

        Vy = dWv * qdot[ 1 ] + l3 * c23 * qdot[ 2 ];

	//29/03/2005 changed velocity_base from ee-frame to base_frame

        vel_base( 0 ) = c1 * Vx - s1 * Vy - vel_base( 4 ) * Pwz + vel_base( 5 ) * Pwy;

        vel_base( 1 ) = c1 * Vy + s1 * Vx - vel_base( 5 ) * Pwx + vel_base( 3 ) * Pwz;

        vel_base( 2 ) = -dWh * qdot[ 1 ] - l3 * s23 * qdot[ 2 ] - vel_base( 3 ) * Pwy + vel_base( 4 ) * Pwx;

        if ( s.isSingular( Singularity::AnySingularity ) )
            return false;
        else
            return true;
    }
}
