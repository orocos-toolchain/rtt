/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:21:19 CEST 2002  SerialmZXXmZXmZ.hpp 

                        SerialmZXXmZXmZ.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 

#ifndef SerialmZXXmZXmZ_HPP
#define SerialmZXXmZXmZ_HPP

#include "SerialZXXZXZ.hpp"
#include <math.h>    /* cos,sin,fabs */
#include <rtt/MultiVector.hpp>

namespace ORO_KinDyn
{

    /**
     * This class provides the kinematics for an ZXXZXZ robot with inverse Z-axes.
     * It uses the ZXXZXZ implementation for that.
     * 
     * @todo do this with angular offsets in kinematics component
     */
    class SerialmZXXmZXmZ : public Serial321
    {

    public:
        /**
         * This is only in order to silent -Woverloaded-virtual 
         * compiler warnings.
         */
        using Kinematics6DWrapper::jacobianForward;
        using Kinematics6DWrapper::jacobianInverse;
        using Kinematics6DWrapper::positionForward;
        using Kinematics6DWrapper::positionInverse;
        using Kinematics6DWrapper::velocityForward;
        using Kinematics6DWrapper::velocityInverse;
        using Kinematics6DWrapper::configurationGet;

        virtual SerialmZXXmZXmZ* clone() const { return new SerialmZXXmZXmZ(*this); }

        virtual std::string getKind() const { return "mZXXmZXmZ"; }

        virtual bool jacobianForward( const RTT::Double6D& q, double J[ 6 ][ 6 ], Singularity& s ) const;

        virtual bool jacobianInverse( const RTT::Double6D& q, double J[ 6 ][ 6 ], Singularity& s ) const;

        virtual bool positionForward( const RTT::Double6D& q, ORO_Geometry::Frame& mp_base, Singularity& s ) const;

        virtual bool positionInverse( const ORO_Geometry::Frame& mp_base, Configuration c, RTT::Double6D& q, Singularity& s ) const;

        virtual bool velocityInverse( const ORO_Geometry::Frame& pos_base, Configuration c, const ORO_Geometry::Twist& vel_base,  RTT::Double6D& q, RTT::Double6D& qdot, Singularity& s ) const;

        virtual bool velocityInverse( const RTT::Double6D& q, const ORO_Geometry::Twist& vel_base, RTT::Double6D& qdot, Singularity& s ) const;

        virtual bool velocityForward( const RTT::Double6D& q, const RTT::Double6D& qdot, ORO_Geometry::Frame& pos_base, ORO_Geometry::Twist& vel_base, Singularity& s ) const;

        virtual void configurationGet(const RTT::Double6D& q, Configuration& c) const;

    virtual void geometrySet( const double len1, const double len2, const double len3,
                      const double len4, const double len5, const double len6 )
    {
        zxxzxz.geometrySet(len1,len2,len3,len4,len5,len6);
    }

    virtual void offsetSet( const double off )
    {
        zxxzxz.offsetSet(off);
    }

    virtual void eccentricitySet( const double ecc )
    {
        zxxzxz.eccentricitySet(ecc);
    }

    private:
        SerialZXXZXZ zxxzxz;
    };
}


#endif
