/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 16:38:38 CET 2005  rnframes.h 

                        rnframes.h -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Erwin Aertbelien
    email                : erwin.aertbelien@mech.kuleuven.ac.be
 
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
 
 
/*****************************************************************************
 *		 
 *  \author 
 *  	Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *		ORO_Geometry V0.2
 *
 *	\par History
 *		- $log$
 *
 *	\par Release
 *		$Id: rnframes.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 *
 *  \file
 *
 *      This file contains the definition of classes for a 
 *      Rall Algebra of (subset of) the classes defined in frames,
 *      i.e. classes that contain a pair (value,derivative) and define operations on that pair
 *      this classes are usefull for automatic differentiation ( <-> symbolic diff , <-> numeric diff)
 *      Defines RNVector, RRotation, RFrame.  Look at Frames.h for details on how to work
 *      with Frame objects.
 *      This set of classes that allow more then one derivative of the objects.
 *
 *  \todo 
 *      The RNFrames classes work with a fixed length determined by global variable VSIZE, 
 *        should find a way to avoid this and still use the stack to allocate the
 *        objects. (to avoid new/delete overhead)
 *       (these objects are constructed and destructed many times). 
 *       Or use some fast memory manager (fixed pool of objects)...
 *       This method is also NOT THREAD-SAFE
 */

#ifndef RNFRAMES_H
#define RNFRAMES_H


#include "frames.h"
#include "rall1d.h"
#include "fvectorvs2.h"
//#include <algebra/fvectorvs.h>


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif




typedef FVectorVSIZE2<Vector,VSIZEMAX,double> VectorList;
typedef FVectorVSIZE2<double,VSIZEMAX,double> DoubleList;
typedef Rall1d<double,DoubleList,double> RNDouble;


class RNVector 
// = TITLE
//     An RNVector is a Vector and its first derivative
// = CLASS TYPE
//     Concrete
{
public:
    Vector p;           //!< position vector
    VectorList v;       //!< velocity vector
public:
    RNVector():p(),v(){
    }
    RNVector(const Vector& _p,const VectorList& _v):
        p(_p),v(_v) {}
    explicit RNVector(const Vector& _p):p(_p),v() {
            int i;
            for (i=0;i<VSIZE;i++) v[i] = Vector::Zero();
    }
    RNVector(const Vector& _p,int unitnr,const Vector& _v) :
        p(_p) {
            int i;
            for (i=0;i<VSIZE;i++) 
                if (i!=unitnr)
                    v[i] = Vector::Zero();
                else
                    v[i] = _v;
    }

    IMETHOD RNVector& operator = (const RNVector& arg);
    IMETHOD RNVector& operator = (const Vector& arg);
    IMETHOD RNVector& operator += (const RNVector& arg);
    IMETHOD RNVector& operator -= (const RNVector& arg);
    IMETHOD static RNVector Zero();
    IMETHOD void ReverseSign();
    IMETHOD RNDouble Norm();
    IMETHOD friend RNVector operator + (const RNVector& r1,const RNVector& r2);
    IMETHOD friend RNVector operator - (const RNVector& r1,const RNVector& r2);
    IMETHOD friend RNVector operator + (const Vector& r1,const RNVector& r2);
    IMETHOD friend RNVector operator - (const Vector& r1,const RNVector& r2);
    IMETHOD friend RNVector operator + (const RNVector& r1,const Vector& r2);
    IMETHOD friend RNVector operator - (const RNVector& r1,const Vector& r2);

    IMETHOD friend RNVector operator * (const RNVector& r1,const RNVector& r2);
    IMETHOD friend RNVector operator * (const RNVector& r1,const Vector& r2);
    IMETHOD friend RNVector operator * (const Vector& r1,const RNVector& r2);

    IMETHOD friend RNVector operator * (const RNVector& r1,double r2);
    IMETHOD friend RNVector operator / (const RNVector& r1,double r2);
    IMETHOD friend RNVector operator * (double r1,const RNVector& r2);

    IMETHOD friend RNVector operator * (const RNDouble& r1,const RNVector& r2);
    IMETHOD friend RNVector operator * (const RNVector& r2,const RNDouble& r1);
    IMETHOD friend RNVector operator*(const Rotation& R,const RNVector& x);
    
    IMETHOD friend bool Equal(const RNVector& r1,const RNVector& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const Vector& r1,const RNVector& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const RNVector& r1,const Vector& r2,double eps=epsilon);
    IMETHOD friend RNVector operator - (const RNVector& r);

    IMETHOD friend RNDouble dot(const RNVector& lhs,const RNVector& rhs);
    IMETHOD friend RNDouble dot(const RNVector& lhs,const Vector& rhs);
    IMETHOD friend RNDouble dot(const Vector& lhs,const RNVector& rhs);
    IMETHOD friend void SetToZero(RNVector& v);
};


class RNTwist;

class RNRotation 
// = TITLE
//     An RRotation is a Rotation and its first derivative, a rotation vector
// = CLASS TYPE
//     Concrete
{
public:
    Rotation R;         //!< Rotation matrix
    VectorList   w;     //!< rotation vector
public:
    RNRotation():R(),w() {}
    explicit RNRotation(const Rotation& _R):R(_R),w(VectorList::Zero()){}
    RNRotation(const Rotation& _R,const VectorList& _w):R(_R),w(_w){}
    RNRotation(const Rotation& _R,int unitnr,const Vector& _w):
        R(_R) 
    {
        for (int i=0;i<VSIZE;i++) 
            if (i!=unitnr) 
                w[i] = Vector::Zero();
            else
                w[i] = _w;
    }
    IMETHOD RNRotation& operator = (const RNRotation& arg);
    IMETHOD RNRotation& operator = (const Rotation& arg);
    IMETHOD static RNRotation Identity();
    IMETHOD RNRotation Inverse() const;
    IMETHOD RNVector Inverse(const RNVector& arg) const;
    IMETHOD RNVector Inverse(const Vector& arg) const;
    IMETHOD RNVector operator*(const RNVector& arg) const;
    IMETHOD RNVector operator*(const Vector& arg) const;
    IMETHOD RNTwist Inverse(const RNTwist& arg) const;
    IMETHOD void DoRotX(const RNDouble& angle);
    IMETHOD void DoRotY(const RNDouble& angle);
    IMETHOD void DoRotZ(const RNDouble& angle);
    IMETHOD static RNRotation RotX(const RNDouble& angle);
    IMETHOD static RNRotation RotY(const RNDouble& angle);  
    IMETHOD static RNRotation RotZ(const RNDouble& angle);
    IMETHOD static RNRotation Rot(const Vector& rotvec,const RNDouble& angle);
    // rotvec has arbitrary norm
    // rotation around a constant vector !
    IMETHOD static RNRotation Rot2(const Vector& rotvec,const RNDouble& angle);
    // rotvec is normalized.
    // rotation around a constant vector !
    IMETHOD friend RNRotation operator* (const RNRotation& r1,const RNRotation& r2);
    IMETHOD friend RNRotation operator* (const Rotation& r1,const RNRotation& r2);
    IMETHOD friend RNRotation operator* (const RNRotation& r1,const Rotation& r2);

    IMETHOD RNTwist operator* (const RNTwist& arg) const;
    IMETHOD RNTwist operator* (const Twist& arg) const;

    IMETHOD friend bool Equal(const RNRotation& r1,const RNRotation& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const Rotation& r1,const RNRotation& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const RNRotation& r1,const Rotation& r2,double eps=epsilon);
};




class RNFrame 
// = TITLE
//     An RFrame is a Frame and its first derivative, a Twist vector
// = CLASS TYPE
//     Concrete
// = CAVEATS
//
{
public:
    RNRotation M;           //!< Orientation of the frame and its derivatives towards VSIZE variables
    RNVector   p;           //!< Position of the frame and its derivatives towards VSIZE variables
public:
    RNFrame(){}

    explicit RNFrame(const Frame& _T):
        M(_T.M),p(_T.p) {}

    RNFrame(const Frame& _T,const Twist& _t):
        M(_T.M,_t.rot),p(_T.p,_t.vel) {}

    RNFrame(const RNRotation& _M,const RNVector& _p):
        M(_M),p(_p) {}

    IMETHOD RNFrame& operator = (const Frame& arg);
    IMETHOD RNFrame& operator = (const RNFrame& arg);
    IMETHOD static RNFrame Identity();
    IMETHOD RNFrame Inverse() const;
    IMETHOD RNVector Inverse(const RNVector& arg) const;
    IMETHOD RNVector operator*(const RNVector& arg) const;
    IMETHOD RNVector operator*(const Vector& arg) const;
    IMETHOD RNVector Inverse(const Vector& arg) const;
    IMETHOD Frame GetFrame() const;
    IMETHOD Twist GetTwist(int i) const;
    IMETHOD friend RNFrame operator * (const RNFrame& f1,const RNFrame& f2);
    IMETHOD friend RNFrame operator * (const Frame& f1,const RNFrame& f2);
    IMETHOD friend RNFrame operator * (const RNFrame& f1,const Frame& f2);
    IMETHOD friend bool Equal(const RNFrame& r1,const RNFrame& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const Frame& r1,const RNFrame& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const RNFrame& r1,const Frame& r2,double eps=epsilon);

    IMETHOD RNTwist operator* (const Twist& arg) const;

};


class RNTwist 
// = TITLE
// This class represents a twist. This is a velocity and rotational velocity together
{
public:
    RNVector vel;  //!< transl velocity and its derivatives towards VSIZE variables
    RNVector rot;  //!< rot    velocity and its derivatives towards VSIZE variables
public:

// = Constructors
    RNTwist():vel(),rot() {};
    RNTwist(const RNVector& _vel,const RNVector& _rot):
        vel(_vel),rot(_rot) {};
    // twist with derivative to the unitnr th var. _ta
    RNTwist(const Twist& t,int unitnr,const Twist& ta):
            vel(t.vel,unitnr,ta.vel),
            rot(t.rot,unitnr,ta.rot) {}


// = Operators
     IMETHOD RNTwist& operator-=(const RNTwist& arg);
     IMETHOD RNTwist& operator+=(const RNTwist& arg);
     IMETHOD double& operator()(int i);
    // index-based access to components, first vel(0..2), then rot(3..5)
     IMETHOD double operator()(int i) const;
    // index-based access to components, first vel(0..2), then rot(3..5)
    // For use with a const Twist

// = External operators
     IMETHOD friend RNTwist operator*(const RNTwist& lhs,double rhs);
     IMETHOD friend RNTwist operator*(double lhs,const RNTwist& rhs);
     IMETHOD friend RNTwist operator/(const RNTwist& lhs,double rhs);
     IMETHOD friend RNTwist operator+(const RNTwist& lhs,const RNTwist& rhs);
     IMETHOD friend RNTwist operator-(const RNTwist& lhs,const RNTwist& rhs);
     IMETHOD friend RNTwist operator-(const RNTwist& arg);
     //IMETHOD friend double dot(const Twist& lhs,const Wrench& rhs);
     //IMETHOD friend double dot(const Wrench& rhs,const Twist& lhs);
     IMETHOD friend void SetToZero(RNTwist& v);

// = Zero
     static IMETHOD RNTwist Zero();

// = Reverse Sign
     IMETHOD void ReverseSign();

     IMETHOD Twist GetTwist();

// = Change Reference point
     IMETHOD RNTwist RefPoint(const Vector& v_base_AB);
     IMETHOD RNTwist RefPoint(const RNVector& v_base_AB);
     // Changes the reference point of the twist.
     // The vector v_base_AB is expressed in the same base as the twist
     // The vector v_base_AB is a vector from the old point to
     // the new point.
     // Complexity : 6M+6A

     // = Equality operators
     // do not use operator == because the definition of Equal(.,.) is slightly
     // different.  It compares whether the 2 arguments are equal in an eps-interval     
     IMETHOD friend bool Equal(const RNTwist& a,const RNTwist& b,double eps=epsilon);

//  friend class Frame; 
};









#ifdef FRAMES_INLINE
    #include "rnframes.inl"
#endif

#ifdef USE_NAMESPACE
}
#endif





#endif
