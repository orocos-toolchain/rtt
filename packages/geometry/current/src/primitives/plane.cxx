
#include <geometry/plane.h>

namespace ORO_Geometry
{

    Plane::Plane()
        : n(Vector(0,0,1)), d(0)
    {
    }

    Plane::Plane(double a, double b, double c, double d) : n(Vector(a,b,c)), d(d) 
    {
        double length=n.Norm();
        d/=length;
        n.Normalize();
    }

    Plane::Plane(const Vector &v1, const Vector &v2, const Vector &v3) 
    {
        Vector v_1(v2-v1);
        Vector v_2(v2-v3);
        n = v_2 * v_1;
        n.Normalize();
        d=-dot(v1, n);
    }

    Plane::~Plane() 
    {
    }

    void Plane::flip() 
    {
        n.x(-n.x());
        n.y(-n.y());
        n.z(-n.z());
        d=-d;
    }

    Vector Plane::project(const Vector& v) const
    {
        return v-n*dot(v,n);
    }

    Vector Plane::normal() const
    {
        return n;
    }

    void
    Plane::reset(const Vector &v1, const Vector &v2, const Vector &v3) 
    {
        n = v2-v3 * v2-v1;
        n.Normalize();
        d = -dot(v1, n);
    }
}
