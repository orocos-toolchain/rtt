/***************************************************************************
  tag: Erwin Aertbelien  Mon May 10 19:10:36 CEST 2004  rotation.cxx 

                        rotation.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Erwin Aertbelien
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

#include "geometry/frames.h"
 
#ifdef USE_NAMESPACE
    namespace ORO_Geometry {
#endif

#ifndef FRAMES_INLINE
    #include "geometry/rotation.inl"
#endif

bool Equal(const Rotation& a,const Rotation& b,double eps) {
    return (Equal(a.data[0],b.data[0],eps) &&
            Equal(a.data[1],b.data[1],eps) &&
            Equal(a.data[2],b.data[2],eps) &&
            Equal(a.data[3],b.data[3],eps) &&
            Equal(a.data[4],b.data[4],eps) &&
            Equal(a.data[5],b.data[5],eps) &&
            Equal(a.data[6],b.data[6],eps) &&
            Equal(a.data[7],b.data[7],eps) &&
            Equal(a.data[8],b.data[8],eps)    );
}



Rotation operator *(const Rotation& lhs,const Rotation& rhs)
// Complexity : 27M+27A
{
    return Rotation(
        lhs.data[0]*rhs.data[0]+lhs.data[1]*rhs.data[3]+lhs.data[2]*rhs.data[6],   
        lhs.data[0]*rhs.data[1]+lhs.data[1]*rhs.data[4]+lhs.data[2]*rhs.data[7],   
        lhs.data[0]*rhs.data[2]+lhs.data[1]*rhs.data[5]+lhs.data[2]*rhs.data[8],
        lhs.data[3]*rhs.data[0]+lhs.data[4]*rhs.data[3]+lhs.data[5]*rhs.data[6],   
        lhs.data[3]*rhs.data[1]+lhs.data[4]*rhs.data[4]+lhs.data[5]*rhs.data[7],   
        lhs.data[3]*rhs.data[2]+lhs.data[4]*rhs.data[5]+lhs.data[5]*rhs.data[8],
        lhs.data[6]*rhs.data[0]+lhs.data[7]*rhs.data[3]+lhs.data[8]*rhs.data[6],   
        lhs.data[6]*rhs.data[1]+lhs.data[7]*rhs.data[4]+lhs.data[8]*rhs.data[7],   
        lhs.data[6]*rhs.data[2]+lhs.data[7]*rhs.data[5]+lhs.data[8]*rhs.data[8]
    );

}


Rotation Rotation::RPY(double roll,double pitch,double yaw) 
    {
        double ca1,cb1,cc1,sa1,sb1,sc1;
        ca1 = cos(yaw); sa1 = sin(yaw); 
        cb1 = cos(pitch);sb1 = sin(pitch); 
        cc1 = cos(roll);sc1 = sin(roll);
        return Rotation(ca1*cb1,ca1*sb1*sc1 - sa1*cc1,ca1*sb1*cc1 + sa1*sc1,
                   sa1*cb1,sa1*sb1*sc1 + ca1*cc1,sa1*sb1*cc1 - ca1*sc1,
                   -sb1,cb1*sc1,cb1*cc1);
    }

// Gives back a rotation matrix specified with RPY convention
void Rotation::GetRPY(double& roll,double& pitch,double& yaw) const
    {
        if (fabs(data[6]) > 1.0 - epsilon ) {
            roll = -sign(data[6]) * atan2(data[1], data[4]);
            pitch= -sign(data[6]) * PI / 2;
            yaw  = 0.0 ;
        } else {
            roll  = atan2(data[7], data[8]);
            pitch = atan2(-data[6], sqrt( sqr(data[0]) +sqr(data[3]) )  );
            yaw   = atan2(data[3], data[0]);
        }
    }

Rotation Rotation::EulerZYZ(double Alfa,double Beta,double Gamma) {
        double sa,ca,sb,cb,sg,cg;
        sa  = sin(Alfa);ca = cos(Alfa);
        sb  = sin(Beta);cb = cos(Beta);
        sg  = sin(Gamma);cg = cos(Gamma);
        return Rotation( ca*cb*cg-sa*sg,     -ca*cb*sg-sa*cg,        ca*sb,
                 sa*cb*cg+ca*sg,     -sa*cb*sg+ca*cg,        sa*sb,
                 -sb*cg ,                sb*sg,              cb
                );
                 
     }


void Rotation::GetEulerZYZ(double& alfa,double& beta,double& gamma) const {
        if (fabs(data[6]) < epsilon ) {
            alfa=0.0;
            if (data[8]>0) {
                beta = 0.0;
                gamma= atan2(-data[1],data[0]);
            } else {
                beta = PI;
                gamma= atan2(data[1],-data[0]);
            }       
        } else {
            alfa=atan2(data[5], data[2]);
            beta=atan2(sqrt( sqr(data[6]) +sqr(data[7]) ),data[8]);
            gamma=atan2(data[7], -data[6]);
        }
 }

Rotation Rotation::Rot(const Vector& rotaxis,double angle) {
    // The formula is 
    // V.(V.tr) + st*[V x] + ct*(I-V.(V.tr))
    // can be found by multiplying it with an arbitrary vector p
    // and noting that this vector is rotated.
    double ct = cos(angle);
    double st = sin(angle);
    double vt = 1-ct;
    Vector rotvec = rotaxis;
	rotvec.Normalize();
    return Rotation(
        ct            +  vt*rotvec(0)*rotvec(0), 
        -rotvec(2)*st +  vt*rotvec(0)*rotvec(1), 
        rotvec(1)*st  +  vt*rotvec(0)*rotvec(2),
        rotvec(2)*st  +  vt*rotvec(1)*rotvec(0),
        ct            +  vt*rotvec(1)*rotvec(1),
        -rotvec(0)*st +  vt*rotvec(1)*rotvec(2),
        -rotvec(1)*st +  vt*rotvec(2)*rotvec(0),
        rotvec(0)*st  +  vt*rotvec(2)*rotvec(1),
        ct            +  vt*rotvec(2)*rotvec(2)
        );
    }

Rotation Rotation::Rot2(const Vector& rotvec,double angle) {
    // rotvec should be normalized !
    // The formula is 
    // V.(V.tr) + st*[V x] + ct*(I-V.(V.tr))
    // can be found by multiplying it with an arbitrary vector p
    // and noting that this vector is rotated.
    double ct = cos(angle);
    double st = sin(angle);
    double vt = 1-ct;
    return Rotation(
        ct            +  vt*rotvec(0)*rotvec(0), 
        -rotvec(2)*st +  vt*rotvec(0)*rotvec(1), 
        rotvec(1)*st  +  vt*rotvec(0)*rotvec(2),
        rotvec(2)*st  +  vt*rotvec(1)*rotvec(0),
        ct            +  vt*rotvec(1)*rotvec(1),
        -rotvec(0)*st +  vt*rotvec(1)*rotvec(2),
        -rotvec(1)*st +  vt*rotvec(2)*rotvec(0),
        rotvec(0)*st  +  vt*rotvec(2)*rotvec(1),
        ct            +  vt*rotvec(2)*rotvec(2)
        );
}



Vector Rotation::GetRot() const 
         // Returns a vector with the direction of the equiv. axis
         // and its norm is angle
     {
       Vector axis  = Vector((data[7]-data[5]),
			     (data[2]-data[6]),
			     (data[3]-data[1]) )/2;
       
       double sa    = axis.Norm();
       double ca    = (data[0]+data[4]+data[8]-1)/2.0;
       double alfa;
       if (sa > epsilon)
	 alfa = ::atan2(sa,ca)/sa;
       else
	 alfa = 1;
       return axis * alfa;
     }



/** Returns the rotation angle around the equiv. axis
 * @param axis the rotation axis is returned in this variable
 * @param eps :  in the case of angle == 0 : rot axis is undefined and choosen 
 *                                         to be +/- Z-axis
 *               in the case of angle == PI : 2 solutions, positive Z-component
 *                                            of the axis is choosen.
 * @result returns the rotation angle (between [0..PI] )
 * /todo :
 *   Check corresponding routines in rframes and rrframes 
 */
double Rotation::GetRotAngle(Vector& axis,double eps) const {
	double ca    = (data[0]+data[4]+data[8]-1)/2.0;
	if (ca>1-eps) {
		// undefined choose the Z-axis, and angle 0
		axis = Vector(0,0,1);
		return 0;
	}
	if (ca < -1+eps) {
		// two solutions, choose a positive Z-component of the axis
		double z = sqrt( (data[8]+1)/2 );
		double x = (data[2])/2/z;
		double y = (data[5])/2/z;
		axis = Vector( x,y,z  );
		return PI;
	}
	double angle = acos(ca);
	double sa    = sin(angle);
	axis  = Vector((data[7]-data[5])/2/sa,
                       (data[2]-data[6])/2/sa,
                       (data[3]-data[1])/2/sa  ); 
	return angle;
}

#ifdef USE_NAMESPACE
}
#endif
