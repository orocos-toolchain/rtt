/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:21:19 CEST 2002  SerialZXXDWH.hpp 

                        SerialZXXDWH.hpp -  description
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
 


#ifndef SerialZXXDWH_HPP
#define SerialZXXDWH_HPP

#include "Serial321.hpp"

#include <math.h>    /* cos,sin,fabs */


namespace ORO_KinDyn
{
    /**
     * A class for calculating the kinematics of a type ZXXDWH robot.
     */
    class SerialZXXDWH : public Serial321
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

        ~SerialZXXDWH();

        bool jacobianForward( const ORO_CoreLib::Double6D& q, double J[ 6 ][ 6 ], Singularity& s ) const;

        bool jacobianInverse( const ORO_CoreLib::Double6D& q, double J[ 6 ][ 6 ], Singularity& s ) const;

        bool positionForward( const ORO_CoreLib::Double6D& q, ORO_Geometry::Frame& mp_base, Singularity& s ) const;

        bool positionInverse( const ORO_Geometry::Frame& mp_base, Configuration c,  ORO_CoreLib::Double6D& q, Singularity& s) const;

        bool velocityInverse( const ORO_Geometry::Frame& pos_base, Configuration c, const ORO_Geometry::Twist& vel_base, ORO_CoreLib::Double6D& q, ORO_CoreLib::Double6D& qdot, Singularity& s ) const;

        bool velocityInverse( const ORO_CoreLib::Double6D& q, const ORO_Geometry::Twist& vel_base, ORO_CoreLib::Double6D& qdot, Singularity& s ) const;

        bool velocityForward( const ORO_CoreLib::Double6D& q, const ORO_CoreLib::Double6D& qdot, ORO_Geometry::Frame& pos_base, ORO_Geometry::Twist& vel_base, Singularity& s ) const;

        void configurationGet(const ORO_CoreLib::Double6D& q, Configuration& c) const;
    };

}

#endif
