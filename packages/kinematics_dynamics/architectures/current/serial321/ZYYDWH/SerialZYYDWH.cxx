#include "kindyn/SerialZYYDWH.hpp"
#include <geometry/frames.h>

namespace ORO_KinDyn
{

    bool SerialZYYDWH::positionForward( const Double6D& original_q, ORO_Geometry::Frame& mp_base, Singularity& s ) const
    {
        Double6D q = original_q;

        q[ 0 ] = - q[ 0 ] - M_PI / 2.0;
        q[ 3 ] = - q[ 3 ];
        q[ 5 ] = - q[ 5 ] + M_PI / 2.0;

        return zxxdwh.positionForward(q, mp_base, s);
    }

    bool SerialZYYDWH::positionInverse ( const ORO_Geometry::Frame& mp_base, Configuration c,  Double6D& q, Singularity& s ) const
    {
        bool result = zxxdwh.positionInverse( mp_base, c, q, s );

        if (result)
            {
                q[ 0 ] = - q[ 0 ] - M_PI / 2.0;
                q[ 3 ] = - q[ 3 ];
                q[ 5 ] = - q[ 5 ] + M_PI / 2.0;
            }
        return result;
    }


    bool SerialZYYDWH::velocityForward ( const Double6D& original_q, const Double6D& original_qdot, ORO_Geometry::Frame& mp_base, ORO_Geometry::Twist& vel_base, Singularity& s ) const
    {
        Double6D q = original_q;
        q[0] = - q[0] - M_PI/2.0;
        q[3] = - q[3];
        q[5] = - q[5] + M_PI/2.0;
       
        Double6D qdot = original_qdot;
        qdot[ 0 ] = - qdot[ 0 ];
        qdot[ 3 ] = - qdot[ 3 ];
        qdot[ 5 ] = - qdot[ 5 ];
	
        return zxxdwh.velocityForward( q, qdot, mp_base, vel_base, s);
    }

    bool SerialZYYDWH::velocityInverse ( const ORO_Geometry::Frame& vel_base, const ORO_Geometry::Twist& vel, Configuration c, Double6D& q, Double6D& qdot, Singularity& s ) const
    {

        bool result = zxxdwh.velocityInverse(vel_base, vel, c, q, qdot, s);

        if (result)
            {
                q[0] = - q[0] - M_PI/2.0;
                q[3] = - q[3];
                q[5] = - q[5] + M_PI/2.0;

                qdot[ 0 ] = - qdot[ 0 ] ;
                qdot[ 3 ] = - qdot[ 3 ];
                qdot[ 5 ] = - qdot[ 5 ] ;
            }

        return result;
    }

    bool SerialZYYDWH::velocityInverse ( const Double6D& q_or, const ORO_Geometry::Twist& vel, Double6D& qdot, Singularity& s ) const
    {
        Double6D q(q_or);
        q[0] = - q[0] - M_PI/2.0;
        q[3] = - q[3];
        q[5] = - q[5] + M_PI/2.0;

        bool result = zxxdwh.velocityInverse(q, vel, qdot, s);

        if (result)
            {
                qdot[ 0 ] = - qdot[ 0 ] ;
                qdot[ 3 ] = - qdot[ 3 ];
                qdot[ 5 ] = - qdot[ 5 ] ;
            }

        return result;
    }

    bool SerialZYYDWH::jacobianInverse ( const Double6D& q_or, double JInv[ 6 ][ 6 ], Singularity& s ) const
    {
        Double6D q(q_or);

        q[0] = - q[0] - M_PI/2.0;
        q[3] = - q[3];
        q[5] = - q[5] + M_PI/2.0;

        return zxxdwh.jacobianInverse(q, JInv, s);
    }

    bool SerialZYYDWH::jacobianForward( const Double6D& q_or, double J[ 6 ][ 6 ], Singularity& s ) const
    {
        Double6D q(q_or);

        q[0] = - q[0] - M_PI/2.0;
        q[3] = - q[3];
        q[5] = - q[5] + M_PI/2.0;

        return zxxdwh.jacobianForward(q, J, s);
    }

    void SerialZYYDWH::configurationGet(const Double6D& q_or, Configuration& c) const
    {
        Double6D q(q_or);

        q[0] = - q[0] - M_PI/2.0;
        q[3] = - q[3];
        q[5] = - q[5] + M_PI/2.0;

        zxxdwh.configurationGet(q,c);
    }


}
