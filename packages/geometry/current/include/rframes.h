/*****************************************************************************
 * \file  
 *      This file contains the definition of classes for a 
 *      Rall Algebra of (subset of) the classes defined in frames,
 *      i.e. classes that contain a pair (value,derivative) and define operations on that pair
 *      this classes are usefull for automatic differentiation ( <-> symbolic diff , <-> numeric diff)
 *      Defines RVector, RRotation, RFrame.  Look at Frames.h for details on how to work
 *      with Frame objects. 
 *  \author 
 *      Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *      ORO_Geometry V0.2
 *
 *  \par History
 *      - $log$
 *
 *  \par Release
 *      $Id: rframes.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/
 
#ifndef RFRAMES_H
#define RFRAMES_H

#include "utility.h"
#include "rall1d.h"
#include "frames.h"




#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

typedef Rall1d<double> RDouble;
class RTwist;
class RVector;
class RFrame;
class RRotation;

class RVector 
// = TITLE
//     An RVector is a Vector and its first derivative
// = CLASS TYPE
//     Concrete
{
public:
    Vector p;       // position vector
    Vector v;       // velocity vector
public:
    RVector():p(),v(){}
    RVector(const Vector& _p,const Vector& _v):p(_p),v(_v) {}
    explicit RVector(const Vector& _p):p(_p),v(Vector::Zero()) {}
    IMETHOD RVector& operator = (const RVector& arg);
    IMETHOD RVector& operator = (const Vector& arg);
    IMETHOD RVector& operator += (const RVector& arg);
    IMETHOD RVector& operator -= (const RVector& arg);
    IMETHOD static RVector Zero();
    IMETHOD void ReverseSign();
    IMETHOD RDouble Norm();
    IMETHOD friend RVector operator + (const RVector& r1,const RVector& r2);
    IMETHOD friend RVector operator - (const RVector& r1,const RVector& r2);
    IMETHOD friend RVector operator + (const Vector& r1,const RVector& r2);
    IMETHOD friend RVector operator - (const Vector& r1,const RVector& r2);
    IMETHOD friend RVector operator + (const RVector& r1,const Vector& r2);
    IMETHOD friend RVector operator - (const RVector& r1,const Vector& r2);
    IMETHOD friend RVector operator * (const RVector& r1,const RVector& r2);
    IMETHOD friend RVector operator * (const RVector& r1,const Vector& r2);
    IMETHOD friend RVector operator * (const Vector& r1,const RVector& r2);
    IMETHOD friend RVector operator * (const RVector& r1,double r2);
    IMETHOD friend RVector operator * (double r1,const RVector& r2);
    IMETHOD friend RVector operator * (const RDouble& r1,const RVector& r2);
    IMETHOD friend RVector operator * (const RVector& r2,const RDouble& r1);
    IMETHOD friend RVector operator*(const Rotation& R,const RVector& x);

    IMETHOD friend RVector operator / (const RVector& r1,double r2);
    IMETHOD friend RVector operator / (const RVector& r2,const RDouble& r1);

    IMETHOD friend bool Equal(const RVector& r1,const RVector& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const Vector& r1,const RVector& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const RVector& r1,const Vector& r2,double eps=epsilon);
    IMETHOD friend RVector operator - (const RVector& r);
    IMETHOD friend RDouble dot(const RVector& lhs,const RVector& rhs);
    IMETHOD friend RDouble dot(const RVector& lhs,const Vector& rhs);
    IMETHOD friend RDouble dot(const Vector& lhs,const RVector& rhs);
};



class RRotation 
// = TITLE
//     An RRotation is a Rotation and its first derivative, a rotation vector
// = CLASS TYPE
//     Concrete
{
public:
    Rotation R; // Rotation matrix
    Vector   w; // rotation vector
public:
    RRotation():R(),w() {}
    explicit RRotation(const Rotation& _R):R(_R),w(Vector::Zero()){}
    RRotation(const Rotation& _R,const Vector& _w):R(_R),w(_w){}
    IMETHOD RRotation& operator = (const RRotation& arg);
    IMETHOD RRotation& operator = (const Rotation& arg);
    IMETHOD static RRotation Identity();
    IMETHOD RRotation Inverse() const;
    IMETHOD RVector Inverse(const RVector& arg) const;
    IMETHOD RVector Inverse(const Vector& arg) const;
    IMETHOD RVector operator*(const RVector& arg) const;
    IMETHOD RVector operator*(const Vector& arg) const;
    IMETHOD void DoRotX(const RDouble& angle);
    IMETHOD void DoRotY(const RDouble& angle);
    IMETHOD void DoRotZ(const RDouble& angle);
    IMETHOD static RRotation RotX(const RDouble& angle);
    IMETHOD static RRotation RotY(const RDouble& angle);    
    IMETHOD static RRotation RotZ(const RDouble& angle);
    IMETHOD static RRotation Rot(const Vector& rotvec,const RDouble& angle);
    // rotvec has arbitrary norm
    // rotation around a constant vector !
    IMETHOD static RRotation Rot2(const Vector& rotvec,const RDouble& angle);
    // rotvec is normalized.
    // rotation around a constant vector !
    IMETHOD friend RRotation operator* (const RRotation& r1,const RRotation& r2);
    IMETHOD friend RRotation operator* (const Rotation& r1,const RRotation& r2);
    IMETHOD friend RRotation operator* (const RRotation& r1,const Rotation& r2);
    IMETHOD friend bool Equal(const RRotation& r1,const RRotation& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const Rotation& r1,const RRotation& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const RRotation& r1,const Rotation& r2,double eps=epsilon);

    IMETHOD RTwist Inverse(const RTwist& arg) const;
    IMETHOD RTwist Inverse(const Twist& arg) const;
    IMETHOD RTwist operator * (const RTwist& arg) const;
    IMETHOD RTwist operator * (const Twist& arg) const;
};




class RFrame 
// = TITLE
//     An RFrame is a Frame and its first derivative, a Twist vector
// = CLASS TYPE
//     Concrete
// = CAVEATS
//
{
public:
    RRotation M;
    RVector   p;
public:
    RFrame(){}

    explicit RFrame(const Frame& _T):
        M(_T.M),p(_T.p) {}

    RFrame(const Frame& _T,const Twist& _t):
        M(_T.M,_t.rot),p(_T.p,_t.vel) {}

    RFrame(const RRotation& _M,const RVector& _p):
        M(_M),p(_p) {}

    IMETHOD RFrame& operator = (const Frame& arg);
    IMETHOD RFrame& operator = (const RFrame& arg);
    IMETHOD static RFrame Identity();
    IMETHOD RFrame Inverse() const;
    IMETHOD RVector Inverse(const RVector& arg) const;
    IMETHOD RVector operator*(const RVector& arg) const;
    IMETHOD RVector operator*(const Vector& arg) const;
    IMETHOD RVector Inverse(const Vector& arg) const;
    IMETHOD Frame GetFrame() const;
    IMETHOD Twist GetTwist() const;
    IMETHOD friend RFrame operator * (const RFrame& f1,const RFrame& f2);
    IMETHOD friend RFrame operator * (const Frame& f1,const RFrame& f2);
    IMETHOD friend RFrame operator * (const RFrame& f1,const Frame& f2);
    IMETHOD friend bool Equal(const RFrame& r1,const RFrame& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const Frame& r1,const RFrame& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const RFrame& r1,const Frame& r2,double eps=epsilon);

    IMETHOD RTwist  Inverse(const RTwist& arg) const;
    IMETHOD RTwist  Inverse(const Twist& arg) const;
    IMETHOD RTwist operator * (const RTwist& arg) const;
    IMETHOD RTwist operator * (const Twist& arg) const;
};





//very similar to Wrench class.
class RTwist 
// = TITLE
// This class represents a RTwist. This is a velocity and rotational velocity together
{
public:
    RVector vel;
    RVector rot;
public:

// = Constructors
    RTwist():vel(),rot() {};
    RTwist(const RVector& _vel,const RVector& _rot):vel(_vel),rot(_rot) {};

// = Operators
     IMETHOD RTwist& operator-=(const RTwist& arg);
     IMETHOD RTwist& operator+=(const RTwist& arg);

// = External operators
     IMETHOD friend RTwist operator*(const RTwist& lhs,double rhs);
     IMETHOD friend RTwist operator*(double lhs,const RTwist& rhs);
     IMETHOD friend RTwist operator/(const RTwist& lhs,double rhs);

     IMETHOD friend RTwist operator*(const RTwist& lhs,const RDouble& rhs);
     IMETHOD friend RTwist operator*(const RDouble& lhs,const RTwist& rhs);
     IMETHOD friend RTwist operator/(const RTwist& lhs,const RDouble& rhs);

     IMETHOD friend RTwist operator+(const RTwist& lhs,const RTwist& rhs);
     IMETHOD friend RTwist operator-(const RTwist& lhs,const RTwist& rhs);
     IMETHOD friend RTwist operator-(const RTwist& arg);
     IMETHOD friend void SetToZero(RTwist& v);


// = Zero
     static IMETHOD RTwist Zero();

// = Reverse Sign
     IMETHOD void ReverseSign();

// = Change Reference point
     IMETHOD RTwist RefPoint(const RVector& v_base_AB);
     // Changes the reference point of the RTwist.
     // The RVector v_base_AB is expressed in the same base as the RTwist
     // The RVector v_base_AB is a RVector from the old point to
     // the new point.
     // Complexity : 6M+6A

     // = Equality operators
     // do not use operator == because the definition of Equal(.,.) is slightly
     // different.  It compares whether the 2 arguments are equal in an eps-interval     
     IMETHOD friend bool Equal(const RTwist& a,const RTwist& b,double eps=epsilon);
     IMETHOD friend bool Equal(const Twist& a,const RTwist& b,double eps=epsilon);
     IMETHOD friend bool Equal(const RTwist& a,const Twist& b,double eps=epsilon);

// = Conversion to other entities
     IMETHOD Twist GetTwist() const;
     IMETHOD Twist GetTwistDot() const;
// = Friends
    friend class RRotation;
    friend class RFrame;
    
};





#ifdef FRAMES_INLINE
    #include "rframes.inl"
#endif

#ifdef USE_NAMESPACE
}
#endif





#endif
