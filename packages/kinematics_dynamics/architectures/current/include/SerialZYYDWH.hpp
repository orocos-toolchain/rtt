/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:21:19 CEST 2002  SerialZYYDWH.hpp 

                        SerialZYYDWH.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

#ifndef SerialZYYDWH_HPP
#define SerialZYYDWH_HPP

#include "SerialZXXDWH.hpp"
#include <math.h>    /* cos,sin,fabs */
#include <corelib/MultiVector.hpp>

namespace ORO_KinDyn
{

    using ORO_CoreLib::Double6D;

    /**
     * This class provides the kinematics for an ZYYDWH robot.
     * It uses the ZXXDWH implementation for that.
     * 
     * @todo do this with angular offsets in kinematics component
     */
    class SerialZYYDWH : public Serial321
    {

    public:
        virtual ~SerialZYYDWH();

        virtual bool jacobianForward( const Double6D& q, double J[ 6 ][ 6 ], Singularity& s ) const;

        virtual bool jacobianInverse( const Double6D& q, double J[ 6 ][ 6 ], Singularity& s ) const;

        virtual bool positionForward( const Double6D& q, ORO_Geometry::Frame& mp_base, Singularity& s ) const;

        virtual bool positionInverse( const ORO_Geometry::Frame& mp_base, Configuration c, Double6D& q, Singularity& s ) const;

        virtual bool velocityInverse( const ORO_Geometry::Frame& pos_base, const ORO_Geometry::Twist& vel_base, Configuration c,  Double6D& q, Double6D& qdot, Singularity& s ) const;

        virtual bool velocityInverse( const Double6D& q, const ORO_Geometry::Twist& vel_base, Double6D& qdot, Singularity& s ) const;

        virtual bool velocityForward( const Double6D& q, const Double6D& qdot, ORO_Geometry::Frame& pos_base, ORO_Geometry::Twist& vel_base, Singularity& s ) const;

        virtual void configurationGet(const Double6D& q, Configuration& c) const;

    virtual void geometrySet( const double len1, const double len2, const double len3,
                      const double len4, const double len5, const double len6 )
    {
        zxxdwh.geometrySet(len1,len2,len3,len4,len5,len6);
    }

    virtual void offsetSet( const double off )
    {
        zxxdwh.offsetSet(off);
    }

    virtual void eccentricitySet( const double ecc )
    {
        zxxdwh.eccentricitySet(ecc);
    }

    private:
        SerialZXXDWH zxxdwh;
    };
}

#endif
