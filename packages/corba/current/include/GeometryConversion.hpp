/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:58 CEST 2006  GeometryConversion.hpp 

                        GeometryConversion.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_GEOMETRY_ANYCONVERSION_HPP
#define ORO_GEOMETRY_ANYCONVERSION_HPP

#include "CorbaConversion.hpp"
#include "GeometryC.h"

namespace RTT
{

    template<>
    struct AnyConversion<ORO_Geometry::Vector>
    {
        typedef ORO_Geometry::Vector& StdType;
        typedef Orocos::Vector CorbaType;

        static bool update(const CORBA::Any& any, StdType tp) {
            CorbaType* at;
            // at is shallow copy of 'any', so do not delete it.
            if (any >>= at ) {
                for(size_t i =0; i != 3; ++i)
                    tp[i] = (*at)[i];
                return true;
            }
            return false;
        }

        static CORBA::Any_ptr createAny( const ORO_Geometry::Vector& tp ) {
            CorbaType ta;
            for(size_t i =0; i != 3; ++i)
                ta[i] = tp[i];
            CORBA::Any_ptr any = new CORBA::Any();
            *any <<= ta;
            return any;
        }
    };

    template<>
    struct AnyConversion<ORO_Geometry::Rotation>
    {
        typedef ORO_Geometry::Rotation& StdType;
        typedef Orocos::Rotation CorbaType;

        static bool update(const CORBA::Any& any, StdType tp) {
            CorbaType* at;
            // at is shallow copy of 'any', so do not delete it.
            if (any >>= at ) {
                for(size_t i =0; i != 3; ++i)
                    for (size_t j=0; j != 3; ++j )
                        tp(i,j) = (*at)[i][j];
                return true;
            }
            return false;
        }

        static CORBA::Any_ptr createAny( const ORO_Geometry::Rotation& tp ) {
            CorbaType ta;
            for(size_t i =0; i != 3; ++i)
                for (size_t j=0; j != 3; ++j )
                    ta[i][j] = tp(i,j);
            CORBA::Any_ptr any = new CORBA::Any();
            *any <<= ta;
            return any;
        }
    };

    template<>
    struct AnyConversion<ORO_Geometry::Frame>
    {
        typedef ORO_Geometry::Frame& StdType;
        typedef Orocos::Frame CorbaType;

        static bool update(const CORBA::Any& any, StdType tp) {
            CorbaType* at;
            // at is shallow copy of 'any', so do not delete it.
            if (any >>= at ) {
                for(size_t i =0; i != 3; ++i) {
                    tp.p[i] = at->p[i];
                    for (size_t j=0; j != 3; ++j )
                        tp.M(i,j) = at->m[i][j];
                }
                return true;
            }
            return false;
        }

        static CORBA::Any_ptr createAny( const ORO_Geometry::Frame& tp ) {
            CorbaType ta;
            for(size_t i =0; i != 3; ++i) {
                ta.p[i] = tp.p[i];
                for (size_t j=0; j != 3; ++j )
                    ta.m[i][j] = tp.M(i,j);
            }
            CORBA::Any_ptr any = new CORBA::Any();
            *any <<= ta;
            return any;
        }
    };

    template<>
    struct AnyConversion<ORO_Geometry::Twist>
    {
        typedef ORO_Geometry::Twist& StdType;
        typedef Orocos::Twist CorbaType;

        static bool update(const CORBA::Any& any, StdType tp) {
            CorbaType* at;
            // at is shallow copy of 'any', so do not delete it.
            if (any >>= at ) {
                for(size_t i =0; i != 3; ++i) {
                    tp.vel[i] = at->vel[i];
                    tp.rot[i] = at->rot[i];
                }
                return true;
            }
            return false;
        }

        static CORBA::Any_ptr createAny( const ORO_Geometry::Twist& tp ) {
            CorbaType ta;
            for(size_t i =0; i != 3; ++i) {
                ta.vel[i] = tp.vel[i];
                ta.rot[i] = tp.rot[i];
            }
            CORBA::Any_ptr any = new CORBA::Any();
            *any <<= ta;
            return any;
        }
    };

    template<>
    struct AnyConversion<ORO_Geometry::Wrench>
    {
        typedef ORO_Geometry::Wrench& StdType;
        typedef Orocos::Wrench CorbaType;

        static bool update(const CORBA::Any& any, StdType tp) {
            CorbaType* at;
            // at is shallow copy of 'any', so do not delete it.
            if (any >>= at ) {
                for(size_t i =0; i != 3; ++i) {
                    tp.force[i] = at->force[i];
                    tp.torque[i] = at->torque[i];
                }
                return true;
            }
            return false;
        }

        static CORBA::Any_ptr createAny( const ORO_Geometry::Wrench& tp ) {
            CorbaType ta;
            for(size_t i =0; i != 3; ++i) {
                ta.force[i] = tp.force[i];
                ta.torque[i] = tp.torque[i];
            }
            CORBA::Any_ptr any = new CORBA::Any();
            *any <<= ta;
            return any;
        }
    };

}

#endif
