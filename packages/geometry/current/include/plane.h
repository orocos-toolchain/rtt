#ifndef ORO_PLANE_H
#define ORO_PLANE_H

#include "frames.h"

namespace ORO_Geometry 
{
    /**
     * This class represents a plane in cartesian space.
     */
    class Plane 
    {
        /**
         * The norm of the plane.
         */
        Vector n;
        /**
         * The distance on the norm of the plane from the origin.
         */
        double d;
    public:
        /**
         * Create a plane defined by three points.
         */
        Plane(const Vector &v1, const Vector &v2, const Vector &v3);
        /**
         * Create a plane equal to the XY plane.
         */
        Plane();

        /**
         * Create a plane using the homogenous numbers.
         */
        Plane(double a, double b, double c, double d);

        /**
         * Destructor.
         */
        ~Plane();
        /**
         * Flip the plane's norm, but not its position. (Norm and distance change
         * their sign)
         */
        void flip();
        /**
         * Project a Vector on this plane.
         */
        Vector project(const Vector& v) const;
        /**
         * The normal of this Plane.
         */
        Vector normal() const;
        /**
         * Redefine the plane.
         */
        void reset( const Vector &v1, const Vector &v2, const Vector &v3 );
    };

}

#endif
