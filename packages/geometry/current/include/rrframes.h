/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 16:38:39 CET 2005  rrframes.h 

                        rrframes.h -  description
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
 * \file  
 *      This file contains the definition of classes for a 
 *      Rall Algebra of (subset of) the classes defined in frames,
 *      i.e. classes that contain a set (value,derivative,2nd derivative) 
 *      and define operations on that set
 *      this classes are usefull for automatic differentiation ( <-> symbolic diff , 
 *      <-> numeric diff).
 *      Defines RRVector, RRRotation, RRFrame, RRDouble.
 *      Look at the corresponding classes Vector Rotation Frame Twist and 
 *      Wrench for the semantics of the methods.
 *
 *      It also contains the 2nd derivative <-> RFrames.h
 *       
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
 *      $Id: rrframes.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/

#ifndef RRFRAMES_H
#define RRFRAMES_H


#include "frames.h"
#include "rall2d.h"



#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

class RRTwist;
typedef Rall2d<double,double,double> RRDouble;


class RRVector 
{
public:
    Vector p;   //!< position vector
    Vector v;   //!< velocity vector
    Vector dv;  //!< acceleration vector
public:
    RRVector():p(),v(),dv() {}
    explicit RRVector(const Vector& _p):p(_p),v(Vector::Zero()),dv(Vector::Zero()) {}
    RRVector(const Vector& _p,const Vector& _v):p(_p),v(_v),dv(Vector::Zero()) {}
    RRVector(const Vector& _p,const Vector& _v,const Vector& _dv):
        p(_p),v(_v),dv(_dv) {}
    IMETHOD RRVector& operator = (const RRVector& arg);
    IMETHOD RRVector& operator = (const Vector& arg);
    IMETHOD RRVector& operator += (const RRVector& arg);
    IMETHOD RRVector& operator -= (const RRVector& arg);
    IMETHOD static RRVector Zero();
    IMETHOD void ReverseSign();
    IMETHOD RRDouble Norm();
    IMETHOD friend RRVector operator + (const RRVector& r1,const RRVector& r2);
    IMETHOD friend RRVector operator - (const RRVector& r1,const RRVector& r2);
    IMETHOD friend RRVector operator + (const Vector& r1,const RRVector& r2);
    IMETHOD friend RRVector operator - (const Vector& r1,const RRVector& r2);
    IMETHOD friend RRVector operator + (const RRVector& r1,const Vector& r2);
    IMETHOD friend RRVector operator - (const RRVector& r1,const Vector& r2);
    IMETHOD friend RRVector operator * (const RRVector& r1,const RRVector& r2);
    IMETHOD friend RRVector operator * (const RRVector& r1,const Vector& r2);
    IMETHOD friend RRVector operator * (const Vector& r1,const RRVector& r2);
    IMETHOD friend RRVector operator * (const RRVector& r1,double r2);
    IMETHOD friend RRVector operator * (double r1,const RRVector& r2);
    IMETHOD friend RRVector operator * (const RRDouble& r1,const RRVector& r2);
    IMETHOD friend RRVector operator * (const RRVector& r2,const RRDouble& r1);
    IMETHOD friend RRVector operator*(const Rotation& R,const RRVector& x);

    IMETHOD friend RRVector operator / (const RRVector& r1,double r2);
    IMETHOD friend RRVector operator / (const RRVector& r2,const RRDouble& r1);


    IMETHOD friend bool Equal(const RRVector& r1,const RRVector& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const Vector& r1,const RRVector& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const RRVector& r1,const Vector& r2,double eps=epsilon);
    IMETHOD friend RRVector operator - (const RRVector& r);
    IMETHOD friend RRDouble dot(const RRVector& lhs,const RRVector& rhs);
    IMETHOD friend RRDouble dot(const RRVector& lhs,const Vector& rhs);
    IMETHOD friend RRDouble dot(const Vector& lhs,const RRVector& rhs);
};



class RRRotation 
{
public:
    Rotation R;     //!< rotation matrix
    Vector   w;     //!< angular velocity vector
    Vector   dw;    //!< angular acceration vector
public:
    RRRotation():R(),w() {}
    explicit RRRotation(const Rotation& _R):R(_R),w(Vector::Zero()){}
    RRRotation(const Rotation& _R,const Vector& _w,const Vector& _dw):
        R(_R),w(_w),dw(_dw) {}
    IMETHOD RRRotation& operator = (const RRRotation& arg);
    IMETHOD RRRotation& operator = (const Rotation& arg);
    IMETHOD static RRRotation Identity();
    IMETHOD RRRotation Inverse() const;
    IMETHOD RRVector Inverse(const RRVector& arg) const;
    IMETHOD RRVector Inverse(const Vector& arg) const;
    IMETHOD RRVector operator*(const RRVector& arg) const;
    IMETHOD RRVector operator*(const Vector& arg) const;

    //  Rotations
    // The SetRot.. functions set the value of *this to the appropriate rotation matrix.
    // The Rot... static functions give the value of the appropriate rotation matrix back.
    // The DoRot... functions apply a rotation R to *this,such that *this = *this * R.
    // IMETHOD void DoRotX(const RRDouble& angle);
    // IMETHOD void DoRotY(const RRDouble& angle);
    // IMETHOD void DoRotZ(const RRDouble& angle);
    // IMETHOD static RRotation RotX(const RRDouble& angle);
    // IMETHOD static RRotation RotY(const RRDouble& angle);    
    // IMETHOD static RRotation RotZ(const RRDouble& angle);

    // IMETHOD void SetRot(const Vector& rotaxis,const RRDouble& angle);
    // Along an arbitrary axes.  The norm of rotvec is neglected.
    // IMETHOD static RRRotation Rot(const Vector& rotvec,const RRDouble& angle);
    // rotvec has arbitrary norm
    // rotation around a constant vector !
    // IMETHOD static RRRotation Rot2(const Vector& rotvec,const RRDouble& angle);
    // rotvec is normalized.
    // rotation around a constant vector !

    IMETHOD friend RRRotation operator* (const RRRotation& r1,const RRRotation& r2);
    IMETHOD friend RRRotation operator* (const Rotation& r1,const RRRotation& r2);
    IMETHOD friend RRRotation operator* (const RRRotation& r1,const Rotation& r2);
    IMETHOD friend bool Equal(const RRRotation& r1,const RRRotation& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const Rotation& r1,const RRRotation& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const RRRotation& r1,const Rotation& r2,double eps=epsilon);
    IMETHOD RRTwist Inverse(const RRTwist& arg) const;
    IMETHOD RRTwist Inverse(const Twist& arg) const;
    IMETHOD RRTwist operator * (const RRTwist& arg) const;
    IMETHOD RRTwist operator * (const Twist& arg) const;
};




class RRFrame
{
public:
    RRRotation M;   //!< Rotation,angular velocity, and angular acceleration of frame.
    RRVector   p;   //!< Translation, velocity and acceleration of origin.
public:
    RRFrame(){}
    explicit RRFrame(const Frame& _T):M(_T.M),p(_T.p) {}
    RRFrame(const Frame& _T,const Twist& _t,const Twist& _dt):
        M(_T.M,_t.rot,_dt.rot),p(_T.p,_t.vel,_dt.vel) {}
    RRFrame(const RRRotation& _M,const RRVector& _p):M(_M),p(_p) {}

    IMETHOD RRFrame& operator = (const RRFrame& arg);
    IMETHOD RRFrame& operator = (const Frame& arg);
    IMETHOD static RRFrame Identity();
    IMETHOD RRFrame Inverse() const;
    IMETHOD RRVector Inverse(const RRVector& arg) const;
    IMETHOD RRVector operator*(const RRVector& arg) const;
    IMETHOD RRVector operator*(const Vector& arg) const;
    IMETHOD RRVector Inverse(const Vector& arg) const;
    IMETHOD Frame GetFrame() const;
    IMETHOD Twist GetTwist() const;
    IMETHOD Twist GetAccTwist() const;
    IMETHOD friend RRFrame operator * (const RRFrame& f1,const RRFrame& f2);
    IMETHOD friend RRFrame operator * (const Frame& f1,const RRFrame& f2);
    IMETHOD friend RRFrame operator * (const RRFrame& f1,const Frame& f2);
    IMETHOD friend bool Equal(const RRFrame& r1,const RRFrame& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const Frame& r1,const RRFrame& r2,double eps=epsilon);
    IMETHOD friend bool Equal(const RRFrame& r1,const Frame& r2,double eps=epsilon);

    IMETHOD RRTwist  Inverse(const RRTwist& arg) const;
    IMETHOD RRTwist  Inverse(const Twist& arg) const;
    IMETHOD RRTwist operator * (const RRTwist& arg) const;
    IMETHOD RRTwist operator * (const Twist& arg) const;
};








//very similar to Wrench class.
class RRTwist 
{
public:
    RRVector vel;       //!< translational velocity and its 1st and 2nd derivative
    RRVector rot;       //!< rotational velocity and its 1st and 2nd derivative
public:

     RRTwist():vel(),rot() {};
     RRTwist(const RRVector& _vel,const RRVector& _rot):vel(_vel),rot(_rot) {};

     IMETHOD RRTwist& operator-=(const RRTwist& arg);
     IMETHOD RRTwist& operator+=(const RRTwist& arg);

     IMETHOD friend RRTwist operator*(const RRTwist& lhs,double rhs);
     IMETHOD friend RRTwist operator*(double lhs,const RRTwist& rhs);
     IMETHOD friend RRTwist operator/(const RRTwist& lhs,double rhs);

     IMETHOD friend RRTwist operator*(const RRTwist& lhs,const RRDouble& rhs);
     IMETHOD friend RRTwist operator*(const RRDouble& lhs,const RRTwist& rhs);
     IMETHOD friend RRTwist operator/(const RRTwist& lhs,const RRDouble& rhs);

     IMETHOD friend RRTwist operator+(const RRTwist& lhs,const RRTwist& rhs);
     IMETHOD friend RRTwist operator-(const RRTwist& lhs,const RRTwist& rhs);
     IMETHOD friend RRTwist operator-(const RRTwist& arg);

     IMETHOD friend void SetToZero(RRTwist& v);

     static IMETHOD RRTwist Zero();

     IMETHOD void ReverseSign();

     IMETHOD RRTwist RefPoint(const RRVector& v_base_AB);
     // Changes the reference point of the RTwist.
     // The RVector v_base_AB is expressed in the same base as the RTwist
     // The RVector v_base_AB is a RVector from the old point to
     // the new point.
     // Complexity : 6M+6A
 
     IMETHOD friend bool Equal(const RRTwist& a,const RRTwist& b,double eps=epsilon);
     IMETHOD friend bool Equal(const Twist& a,const RRTwist& b,double eps=epsilon);
     IMETHOD friend bool Equal(const RRTwist& a,const Twist& b,double eps=epsilon);


     IMETHOD Twist GetTwist() const;
     IMETHOD Twist GetTwistDot() const;

    friend class RRRotation;
    friend class RRFrame;
    
};







#ifdef FRAMES_INLINE
    #include "rrframes.inl"
#endif

#ifdef USE_NAMESPACE
}
#endif





#endif
