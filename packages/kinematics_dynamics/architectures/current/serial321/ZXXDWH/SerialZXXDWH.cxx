
#include "kindyn/SerialZXXDWH.hpp"

namespace ORO_KinDyn
{

    void SerialZXXDWH::configurationGet(const Double6D& q, Configuration& c) const
    {
        double s23;
        double dWh;

        c.set( Serial321Configuration::Normal );

        if ( q[ 4 ] < -KINEMATICS_EPS )
        {
            c.set( Serial321Configuration::Flip );
        }

        else
        {
            c.set( Serial321Configuration::NoFlip);
        }

        s23 = sin( q[ 1 ] + q[ 2 ] );

        /* horizontal distance of wrist from shoulder: */
        dWh = sin( q[ 1 ] ) * l2 + s23 * l3;


        /* CALCULATION OF CONFIGURATION: */

        if ( dWh >= 0.0 )
            c.set( Serial321Configuration::Forward );
        else
            c.set( Serial321Configuration::Backward );

        if ( q[ 4 ] < 0. )
            c.set( Serial321Configuration::Flip );

        if ( q[ 2 ] < 0. )
            c.set( Serial321Configuration::ElbowDown );

    }

}

