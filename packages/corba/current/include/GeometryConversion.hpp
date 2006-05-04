#ifndef ORO_GEOMETRY_ANYCONVERSION_HPP
#define ORO_GEOMETRY_ANYCONVERSION_HPP

#include <corba/CorbaConversion.hpp>
#include "GeometryC.h"

namespace ORO_CoreLib
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
