/**   
// ============================================================================
// \file
// \warning
//       Efficienty can be improved by writing p2 = A*(B*(C*p1))) instead of
//          p2=A*B*C*p1
//          
// \par PROPOSED NAMING CONVENTION FOR FRAME-like OBJECTS
 *
// \verbatim
//      A naming convention of objects of the type defined in this file :
//          (1) Frame : F...
//              Rotation : R ...
//          (2) Twist    : T ...
//              Wrench   : W ...
//              Vector   : V ...
//      This prefix is followed by :
//      for category (1) :
//          F_A_B : w.r.t. frame A, frame B expressed 
//          ( each collumn of F_A_B corresponds to an axis of B, 
//            expressed w.r.t. frame A )
//          in mathematical convention :
//                   A
//         F_A_B ==    F
//                   B
//      
//      for category (2) :
//          V_B   : a vector expressed w.r.t. frame B
//      
//      This can also be prepended by a name :
//          e.g. : temporaryV_B
//      
//      With this convention one can write :
//      
//      F_A_B = F_B_A.Inverse();
//      F_A_C = F_A_B * F_B_C;
//      V_B   = F_B_C * V_C;    // both translation and rotation
//      V_B   = R_B_C * V_C;    // only rotation
// \endverbatim
 *
// \par CONVENTIONS FOR WHEN USED WITH ROBOTS :
 *
// \verbatim
//       world : represents the frame ([1 0 0,0 1 0,0 0 1],[0 0 0]')
//       mp    : represents mounting plate of a robot
//               (i.e. everything before MP is constructed by robot manufacturer
//                    everything after MP is tool )
//       tf    : represents task frame of a robot
//               (i.e. frame in which motion and force control is expressed)
//       sf    : represents sensor frame of a robot
//               (i.e. frame at which the forces measured by the force sensor 
//               are expressed )
//      
//          Frame F_world_mp=...;
//          Frame F_mp_sf(..)
//          Frame F_mp_tf(,.)
//      
//          Wrench are measured in sensor frame SF, so one could write :
//                Wrench_tf = F_mp_tf.Inverse()* ( F_mp_sf * Wrench_sf );
// \endverbatim
//
// \par CONVENTIONS REGARDING UNITS :
//      Any consistent series of units can be used, e.g. N,mm,Nmm,..mm/sec
 *
// \par Twist and Wrench transformations
// 3 different types of transformations do exist for the twists 
// and wrenches.
 *
// \verbatim
//      1) Frame * Twist or Frame * Wrench :
//              this transforms both the velocity/force reference point
//             and the basis to which the twist/wrench are expressed.
//      2) Rotation * Twist or Rotation * Wrench :
//              this transforms the basis to which the twist/wrench are
//              expressed, but leaves the reference point intact.
//      3) Twist.RefPoint(v_base_AB) or Wrench.RefPoint(v_base_AB)
//              this transforms only the reference point. v is expressed
//              in the same base as the twist/wrench and points from the
//              old reference point to the new reference point.
// \endverbatim
 *
// \par Complexity
//  Sometimes the amount of work is given in the documentation
//  e.g. 6M+3A means 6 multiplications and 3 additions.
 *
 *  \author 
 *      Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *      ORO_Geometry V0.2
 *
 *  \par History
 *      * BUG FIXED : Vector::Norm() : case of v = [0 0 0]
 *					                   fabs of data[2]
 *		* BUG FIXED : Frame::DH()    : sin and cos interchanged
 *		* REDEFINITION OF Vector::Normalize interf and impl to something more useful
 *		* july 2002 : various bugs fixed
 *      * changed some syntax so that Visual C++ classview better parses
 *  \par Release
 *      $Id: frames.h,v 1.1.1.1.2.4 2003/04/04 15:39:43 pissaris Exp $
 *      $Name:  $ 
 ****************************************************************************/
#ifndef FRAMES_H
#define FRAMES_H





/////////////////////////////////////////////////////////////


#include "utility.h"
#include "pkgconf/geometry.h"

#ifdef FRAMES_CHECK
    #include <cassert>
#endif


#ifdef FRAMES_CHECK
    #define FRAMES_CHECKI(a) assert(a)
#else
    #define FRAMES_CHECKI(a)
#endif

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif



class Vector;
class Rotation;
class Frame;
class Wrench;
class Twist;
class Vector2;
class Rotation2;
class Frame2;



/**
 * A concrete implementation of a 3 dimensional vector class
 */
class Vector 
{
public:
    double data[3];
#ifdef OROSEM_GEOMETRY_DEFAULT_INIT
     IMETHOD Vector()  { *this = Vector::Zero(); }
#else
     //! Does not initialise the Vector to zero. use Vector::Zero() or SetToZero for that
     IMETHOD Vector() {}
#endif
     //! Constructs a vector out of the three values x, y and z
     IMETHOD Vector(double x,double y, double z);

     //! Assignment operator. The normal copy by value semantics.
     IMETHOD Vector(const Vector& arg);

     //! Assignment operator. The normal copy by value semantics.
     IMETHOD Vector& operator = ( const Vector& arg);

     //! Access to elements, range checked when NDEBUG is not set, from 0..2
     IMETHOD double operator()(int index) const;

     //! Access to elements, range checked when NDEBUG is not set, from 0..2
     IMETHOD double& operator() (int index); 

     double operator[] ( int index ) const
       {
	 return this->operator() ( index );
       }
	    
     double& operator[] ( int index )
       {
	 return this->operator() ( index );
       }

     IMETHOD double x() const;
     IMETHOD double y() const;
     IMETHOD double z() const;
     IMETHOD void x(double);
     IMETHOD void y(double);
     IMETHOD void z(double);

     //! Reverses the sign of the Vector object itself
     IMETHOD void ReverseSign();


     //! subtracts a vector from the Vector object itself
     IMETHOD Vector& operator-=(const Vector& arg);
     

     //! Adds a vector from the Vector object itself
     IMETHOD Vector& operator +=(const Vector& arg);
     
     //! Scalar multiplication is defined
     IMETHOD friend Vector operator*(const Vector& lhs,double rhs);
     //! Scalar multiplication is defined
     IMETHOD friend Vector operator*(double lhs,const Vector& rhs);
     //! Scalar division is defined

     IMETHOD friend Vector operator/(const Vector& lhs,double rhs);
     IMETHOD friend Vector operator+(const Vector& lhs,const Vector& rhs);
     IMETHOD friend Vector operator-(const Vector& lhs,const Vector& rhs);
     IMETHOD friend Vector operator*(const Vector& lhs,const Vector& rhs);
     IMETHOD friend Vector operator-(const Vector& arg);
     IMETHOD friend double dot(const Vector& lhs,const Vector& rhs);

     //! To have a uniform operator to put an element to zero, for scalar values
     //! and for objects.
     IMETHOD friend void SetToZero(Vector& v);

     //! @return a zero vector
     IMETHOD static Vector Zero();
     
   /** Normalizes this vector and returns it norm
	* makes v a unitvector and returns the norm of v.
	* if v is smaller than eps, Vector(1,0,0) is returned with norm 0.
	* if this is not good, check the return value of this method.
	*/
     double Normalize(double eps=epsilon);

     //! OBSOLETE
     Vector Normalize2() const; // OBSOLETE: TODO : remove this method
    
     //!    @return the norm of the vector  
     double Norm() const;
    
    

     //! a 3D vector where the 2D vector v is put in the XY plane
     IMETHOD void Set2DXY(const Vector2& v);
     //! a 3D vector where the 2D vector v is put in the YZ plane
     IMETHOD void Set2DYZ(const Vector2& v);
     //! a 3D vector where the 2D vector v is put in the ZX plane
     IMETHOD void Set2DZX(const Vector2& v);
     //! a 3D vector where the 2D vector v_XY is put in the XY plane of the frame F_someframe_XY.    
     IMETHOD void Set2DPlane(const Frame& F_someframe_XY,const Vector2& v_XY);


     //! do not use operator == because the definition of Equal(.,.) is slightly
     //! different.  It compares whether the 2 arguments are equal in an eps-interval    
     IMETHOD friend bool Equal(const Vector& a,const Vector& b,double eps=epsilon);

     friend class Rotation;
     friend class Frame;
};


/**
  A class to represent rotations in 3 dimensional space
  This class represents a rotation matrix with the following
  conventions :
 \verbatim
     Suppose V2 = R*V,                                    (1)
     V is expressed in frame B
     V2 is expressed in frame A
     This matrix R consists of 3 collumns [ X,Y,Z ],
     X,Y, and Z contain the axes of frame B, expressed in frame A
     Because of linearity expr(1) is valid.
 \endverbatim
   This class only represents rotational_interpolation, not translation
 Two interpretations are possible for rotation angles.
 * if you rotate with angle around X frame A to have frame B, 
   then the result of SetRotX is equal to frame B expressed wrt A.
     In code:
 \verbatim
      Rotation R;
      F_A_B = R.SetRotX(angle);
 \endverbatim
 * Secondly, if you take the following code :
 \verbatim
      Vector p,p2; Rotation R;
      R.SetRotX(angle);
      p2 = R*p;
 \endverbatim
   then the frame p2 is rotated around X axis with (-angle).
   Analogue reasonings can be applyd to SetRotY,SetRotZ,SetRot
 \par type
  Concrete implementation
*/
class Rotation 
{
public:
    double data[9];

#ifdef OROSEM_GEOMETRY_DEFAULT_INIT
    IMETHOD Rotation() {
		*this = Rotation::Identity();
	}
#else
    IMETHOD Rotation() {}
#endif
    IMETHOD Rotation(double Xx,double Yx,double Zx,
                double Xy,double Yy,double Zy,
                double Xz,double Yz,double Zz);
    IMETHOD Rotation(const Vector& x,const Vector& y,const Vector& z);
    // default copy constructor is sufficient


     IMETHOD Rotation& operator=(const Rotation& arg);

     //!  Defines a multiplication R*V between a Rotation R and a Vector V.
     //! Complexity : 9M+6A
     IMETHOD Vector operator*(const Vector& v) const;

     //!    Access to elements 0..2,0..2, bounds are checked when NDEBUG is not set
     IMETHOD double& operator()(int i,int j);

     //!    Access to elements 0..2,0..2, bounds are checked when NDEBUG is not set
     IMETHOD double operator() (int i,int j) const;
     
     friend Rotation operator *(const Rotation& lhs,const Rotation& rhs);

     //! Sets the value of *this to its inverse.
     IMETHOD void SetInverse();

     //! Gives back the inverse rotation matrix of *this.
     IMETHOD Rotation Inverse() const;
    
     //! The same as R.Inverse()*v but more efficient.
     IMETHOD Vector Inverse(const Vector& v) const;

     //! The same as R.Inverse()*arg but more efficient.    
     IMETHOD Wrench Inverse(const Wrench& arg) const;
    
     //! The same as R.Inverse()*arg but more efficient.
     IMETHOD Twist Inverse(const Twist& arg) const;
    
     //! Gives back an identity rotaton matrix 
     IMETHOD static Rotation Identity();
    

// = Rotations
    //! The Rot... static functions give the value of the appropriate rotation matrix back.
    IMETHOD static Rotation RotX(double angle);
    //! The Rot... static functions give the value of the appropriate rotation matrix back.
    IMETHOD static Rotation RotY(double angle);
    //! The Rot... static functions give the value of the appropriate rotation matrix back.
    IMETHOD static Rotation RotZ(double angle);
    //! The DoRot... functions apply a rotation R to *this,such that *this = *this * Rot..
    //! DoRot... functions are only defined when they can be executed more efficiently
    IMETHOD void DoRotX(double angle);
    //! The DoRot... functions apply a rotation R to *this,such that *this = *this * Rot..
    //! DoRot... functions are only defined when they can be executed more efficiently
    IMETHOD void DoRotY(double angle);
    //! The DoRot... functions apply a rotation R to *this,such that *this = *this * Rot..
    //! DoRot... functions are only defined when they can be executed more efficiently
    IMETHOD void DoRotZ(double angle);

    //! Along an arbitrary axes.  It is not necessary to normalize rotvec. 
    //! returns identity rotation matrix in the case that the norm of rotvec
    //! is to small to be used.  
    // @see Rot2 if you want to handle this error in another way.
    static Rotation Rot(const Vector& rotvec,double angle);

    //! Along an arbitrary axes.  rotvec should be normalized.
    static Rotation Rot2(const Vector& rotvec,double angle);
    
    //! Returns a vector with the direction of the equiv. axis
    //! and its norm is angle
    Vector GetRot() const; 

	/** Returns the rotation angle around the equiv. axis
	 * @param axis the rotation axis is returned in this variable
	 * @param eps :  in the case of angle == 0 : rot axis is undefined and choosen 
	 *                                         to be +/- Z-axis
	 *               in the case of angle == PI : 2 solutions, positive Z-component
	 *                                            of the axis is choosen.
	 * @result returns the rotation angle (between [0..PI] )
	 */
	double GetRotAngle(Vector& axis,double eps=epsilon) const;


    //! Gives back a rotation matrix specified with EulerZYZ convention :
    //!  First rotate around Z with alfa, 
    //!  then around the new Y with beta, then around 
    //!  new Z with gamma.
    static Rotation EulerZYZ(double Alfa,double Beta,double Gamma);

    //! Gives back the EulerZYZ convention description of the rotation matrix :
    //!  First rotate around Z with alfa, 
    //!  then around the new Y with beta, then around 
    //!  new Z with gamma.
    //!
    //! Variables are bound by
    //!  (-PI <= alfa <= PI),
    //! (0 <= beta <= PI),
    //!  (-PI <= alfa <= PI)
    void GetEulerZYZ(double& alfa,double& beta,double& gamma) const;


    //! Sets the value of this object to a rotation specified with RPY convention:
    //! first rotate around X with roll, then around the
    //!               old Y with pitch, then around old Z with alfa 
    static Rotation RPY(double roll,double pitch,double yaw);

    //! Gives back a vector in RPY coordinates, variables are bound by
    //!   -PI <= roll <= PI
    //!    -PI <= Yaw  <= PI
    //!   -PI/2 <= PITCH <= PI/2
    //!
    //!  convention : first rotate around X with roll, then around the
    //!               old Y with pitch, then around old Z with alfa 
    void GetRPY(double& roll,double& pitch,double& yaw) const;

    
    //! Gives back a rotation matrix specified with EulerZYX convention :
    //!  First rotate around Z with alfa, 
    //!  then around the new Y with beta, then around 
    //!  new X with gamma.
    //!
    //! closely related to RPY-convention
    IMETHOD static Rotation EulerZYX(double Alfa,double Beta,double Gamma) {
        return RPY(Gamma,Beta,Alfa);
    }

    //! GetEulerZYX gets the euler ZYX parameters of a rotation :
    //!  First rotate around Z with alfa, 
    //!  then around the new Y with beta, then around 
    //!  new X with gamma.
    //!
    //! Range of the results of GetEulerZYX :
    //!   -PI <= alfa <= PI
    //!    -PI <= gamma <= PI
    //!   -PI/2 <= beta <= PI/2
    //!
    //! Closely related to RPY-convention.
    IMETHOD void GetEulerZYX(double& Alfa,double& Beta,double& Gamma) const {
        GetRPY(Gamma,Beta,Alfa);
    }

     //! Transformation of the base to which the twist is expressed.
     //! Complexity : 18M+12A
     //! @see Frame*Twist for a transformation that also transforms
     //! the velocity reference point.
     IMETHOD Twist operator * (const Twist& arg) const;

     //! Transformation of the base to which the wrench is expressed.
     //! Complexity : 18M+12A
     //! @see Frame*Wrench for a transformation that also transforms
     //! the force reference point.
     IMETHOD Wrench operator * (const Wrench& arg) const;

     //! Access to the underlying unitvectors of the rotation matrix
     IMETHOD Vector UnitX() const {
         return Vector(data[0],data[3],data[6]);
     }

     //! Access to the underlying unitvectors of the rotation matrix
     IMETHOD void UnitX(const Vector& X) {
        data[0] = X(0);
        data[3] = X(1);
        data[6] = X(2);
     }

     //! Access to the underlying unitvectors of the rotation matrix
     IMETHOD Vector UnitY() const {
         return Vector(data[1],data[4],data[7]);
     }

     //! Access to the underlying unitvectors of the rotation matrix
     IMETHOD void UnitY(const Vector& X) {
        data[1] = X(0);
        data[4] = X(1);
        data[7] = X(2);
     }

     //! Access to the underlying unitvectors of the rotation matrix
     IMETHOD Vector UnitZ() const {
         return Vector(data[2],data[5],data[8]);
     }

     //! Access to the underlying unitvectors of the rotation matrix
     IMETHOD void UnitZ(const Vector& X) {
        data[2] = X(0);
        data[5] = X(1);
        data[8] = X(2);
     }

     //! do not use operator == because the definition of Equal(.,.) is slightly
     //! different.  It compares whether the 2 arguments are equal in an eps-interval    
     friend bool Equal(const Rotation& a,const Rotation& b,double eps=epsilon);

     friend class Frame;
};




/**
    This class represents a complete Frame transformation (rotational_interpolation + rotation)
    if V2 = Frame*V1 (V2 expressed in frame A, V1 expressed in frame B)
    then V2 = Frame.M*V1+Frame.p

    Frame.M contains columns that represent the axes of frame B wrt frame A
    Frame.p contains the origin of frame B expressed in frame A.
*/


class Frame {
public:
    Vector p;       //!< origine of the Frame
    Rotation M;     //!< Orientation of the Frame

public:

     IMETHOD Frame(const Rotation& R,const Vector& V);

     //! The rotation matrix defaults to identity
     EXPLICIT IMETHOD Frame(const Vector& V);
     //! The position matrix defaults to zero 
     EXPLICIT IMETHOD Frame(const Rotation& R);
    
     //! An empty constructor does NOT initialise to Identity/Zero to avoid
     //! unneccessary calculations
     IMETHOD Frame(){}
     //! The copy constructor. Normal copy by value semantics.
     IMETHOD Frame(const Frame& arg);

     //! Reads data from an double array    
     //\TODO should be formulated as a constructor
     void Make4x4(double* d);

     //!  Treats a frame as a 4x4 matrix and returns element i,j
     //!  Access to elements 0..3,0..3, bounds are checked when NDEBUG is not set
     IMETHOD double operator()(int i,int j);

     //!  Treats a frame as a 4x4 matrix and returns element i,j
     //!    Access to elements 0..3,0..3, bounds are checked when NDEBUG is not set
     IMETHOD double operator() (int i,int j) const;

// = Inverse
     //! Gives back inverse transformation of a Frame
     IMETHOD Frame Inverse() const;
    
     //! The same as p2=R.Inverse()*p but more efficient.
     IMETHOD Vector Inverse(const Vector& arg) const;   
    
     //! The same as p2=R.Inverse()*p but more efficient.
     IMETHOD Wrench Inverse(const Wrench& arg) const;
    
     //! The same as p2=R.Inverse()*p but more efficient.
     IMETHOD Twist  Inverse(const Twist& arg) const;

     //! Normal copy-by-value semantics.
     IMETHOD Frame& operator = (const Frame& arg);

     //! Transformation of the base to which the vector 
     //! is expressed.
     IMETHOD Vector operator * (const Vector& arg) const;

     //! Transformation of both the force reference point
     //! and of the base to which the wrench is expressed.
     //! look at Rotation*Wrench operator for a transformation 
     //! of only the base to which the twist is expressed.
     //!
     //! Complexity : 24M+18A
     IMETHOD Wrench operator * (const Wrench& arg) const;

     //! Transformation of both the velocity reference point
     //! and of the base to which the twist is expressed.
     //! look at Rotation*Twist for a transformation of only the
     //! base to which the twist is expressed.
     //!
     //! Complexity : 24M+18A
     IMETHOD Twist operator * (const Twist& arg) const;

     //! Composition of two frames.
     IMETHOD friend Frame operator *(const Frame& lhs,const Frame& rhs);

     //! @return the identity transformation Frame(Rotation::Identity(),Vector::Zero()).
     IMETHOD static Frame Identity();

     //! The twist <t_this> is expressed wrt the current
     //! frame.  This frame is integrated into an updated frame with
     //! <samplefrequency>.  Very simple first order integration rule.
     IMETHOD void Integrate(const Twist& t_this,double frequency);

     /**
     // SetDH : constructs a transformationmatrix T_link(i-1)_link(i)
     //         with the Denevit-Hartenberg convention.
     // Note that the frame is a redundant way to express the information
     // in the DH-convention.
     // \verbatim
     // Parameters in full : a(i-1),alpha(i-1),d(i),theta(i)        
     //
     // axis i-1   is connected by link i-1   to  axis i
     //  numbering axis 1 to axis n
     // link 0 (immobile base) to link n 
     //
     //     link length a(i-1) 
     //         length of the mutual perpendicular line (normal) between the 2 axes.
     //         This normal runs from (i-1) to (i) axis.
     //     link twist alpha(i-1):
     //         construct plane perpendicular to the normal
     //         project axis(i-1) and axis(i) into plane
     //         angle from (i-1) to (i) measured in the direction of the normal
     //     link offset d(i)
     //         signed distance between normal (i-1) to (i) and normal (i) to (i+1)
     //         along axis i
     //     joint angle theta(i)
     //         signed angle between normal (i-1) to (i) and normal (i) to (i+1)
     //         along axis i
     // First and last joints :
     //     a(0)= a(n) = 0
     //     alpha(0) = alpha(n) = 0
     //     PRISMATIC : theta(1) = 0
     //                 d(1) arbitrarily
     //     REVOLUTE  : theta(1) arbitrarily
     //                 d(1) = 0
     // Not unique :
     //     if intersecting joint axis 2 choices for normal
     // Frame assignment of the DH convention :
     //     Z(i-1) follows axis (i-1)
     //     X(i-1) is the normal between axis(i-1) and axis(i)
     //     Y(i-1) follows out of Z(i-1) and X(i-1)
     //     
     //     a(i-1)     = distance from Z(i-1) to Z(i) along X(i-1)
     //     alpha(i-1) = angle between Z(i-1) to Z(i) along X(i-1)
     //     d(i)       = distance from X(i-1) to X(i) along Z(i)
     //     theta(i)   = angle between X(i-1) to X(i) along X(i)
     // \endverbatim
     */
     static Frame DH(double a,double alpha,double d,double theta);



     //! do not use operator == because the definition of Equal(.,.) is slightly
     //! different.  It compares whether the 2 arguments are equal in an eps-interval    
     IMETHOD friend bool Equal(const Frame& a,const Frame& b,double eps=epsilon);

};

//! This class represents a twist.  A twist is the combination of translational
//! velocity and rotational velocity applied at one point.
class Twist {
public:
    Vector vel; //!< The velocity of that point
    Vector rot; //!< The rotational velocity of that point.
public:

    //! The default constructor does NOT initialise to Zero.
    Twist():vel(),rot() {};

    Twist(const Vector& _vel,const Vector& _rot):vel(_vel),rot(_rot) {};

    IMETHOD Twist& operator-=(const Twist& arg);
    IMETHOD Twist& operator+=(const Twist& arg);
    //! index-based access to components, first vel(0..2), then rot(3..5)
    IMETHOD double& operator()(int i);

    //! index-based access to components, first vel(0..2), then rot(3..5)
    //! For use with a const Twist
    IMETHOD double operator()(int i) const;

     IMETHOD friend Twist operator*(const Twist& lhs,double rhs);
     IMETHOD friend Twist operator*(double lhs,const Twist& rhs);
     IMETHOD friend Twist operator/(const Twist& lhs,double rhs);
     IMETHOD friend Twist operator+(const Twist& lhs,const Twist& rhs);
     IMETHOD friend Twist operator-(const Twist& lhs,const Twist& rhs);
     IMETHOD friend Twist operator-(const Twist& arg);
     IMETHOD friend double dot(const Twist& lhs,const Wrench& rhs);
     IMETHOD friend double dot(const Wrench& rhs,const Twist& lhs);
     IMETHOD friend void SetToZero(Twist& v);


     //! @return a zero Twist : Twist(Vector::Zero(),Vector::Zero())
     static IMETHOD Twist Zero();

     //! Reverses the sign of the twist
     IMETHOD void ReverseSign();

     //! Changes the reference point of the twist.
     //! The vector v_base_AB is expressed in the same base as the twist
     //! The vector v_base_AB is a vector from the old point to
     //! the new point.
     //!
     //! Complexity : 6M+6A
     IMETHOD Twist RefPoint(const Vector& v_base_AB);


     //! do not use operator == because the definition of Equal(.,.) is slightly
     //! different.  It compares whether the 2 arguments are equal in an eps-interval    
     IMETHOD friend bool Equal(const Twist& a,const Twist& b,double eps=epsilon);

// = Friends
    friend class Rotation;
    friend class Frame;
  
};

 

//! This class represents a Wrench.  A Wrench is the force and torque applied at a point
class Wrench 
{
public:
    Vector force;       //!< Force that is applied at the origin of the current ref frame
    Vector torque;      //!< Torque that is applied at the origin of the current ref frame
public:

    //! Does NOT initialise force and torque to zero !
    Wrench():force(),torque() {};
    Wrench(const Vector& _force,const Vector& _torque):force(_force),torque(_torque) {};

// = Operators
     IMETHOD Wrench& operator-=(const Wrench& arg);
     IMETHOD Wrench& operator+=(const Wrench& arg);

     //! index-based access to components, first force(0..2), then torque(3..5)
     IMETHOD double& operator()(int i);

     //! index-based access to components, first force(0..2), then torque(3..5)
     //! for use with a const Wrench
     IMETHOD double operator()(int i) const;
     
     
     //! Scalar multiplication
     IMETHOD friend Wrench operator*(const Wrench& lhs,double rhs);
     //! Scalar multiplication
     IMETHOD friend Wrench operator*(double lhs,const Wrench& rhs);
     //! Scalar division
     IMETHOD friend Wrench operator/(const Wrench& lhs,double rhs);

     IMETHOD friend Wrench operator+(const Wrench& lhs,const Wrench& rhs);
     IMETHOD friend Wrench operator-(const Wrench& lhs,const Wrench& rhs);

     //! An unary - operator
     IMETHOD friend Wrench operator-(const Wrench& arg);

     //! Sets the Wrench to Zero, to have a uniform function that sets an object or
     //! double to zero.
     IMETHOD friend void SetToZero(Wrench& v);

     //! @return a zero Wrench
     static IMETHOD Wrench Zero();

     //! Reverses the sign of the current Wrench
     IMETHOD void ReverseSign();

     //! Changes the reference point of the wrench.
     //! The vector v_base_AB is expressed in the same base as the twist
     //! The vector v_base_AB is a vector from the old point to
     //! the new point.
     //!
     //! Complexity : 6M+6A
     IMETHOD Wrench RefPoint(const Vector& v_base_AB);


     //! do not use operator == because the definition of Equal(.,.) is slightly
     //! different.  It compares whether the 2 arguments are equal in an eps-interval    
     IMETHOD friend bool Equal(const Wrench& a,const Wrench& b,double eps=epsilon);

    friend class Rotation;
    friend class Frame;
	 

};


//! 2D version of Vector
class Vector2
{
    double data[2];
public:
     //! Does not initialise to Zero().
     Vector2() {}
     IMETHOD Vector2(double x,double y);
     IMETHOD Vector2(const Vector2& arg);

     IMETHOD Vector2& operator = ( const Vector2& arg);

     //! Access to elements, range checked when NDEBUG is not set, from 0..1
     IMETHOD double operator()(int index) const;

     //! Access to elements, range checked when NDEBUG is not set, from 0..1
     IMETHOD double& operator() (int index); 
    
     IMETHOD void ReverseSign();
     IMETHOD Vector2& operator-=(const Vector2& arg);
     IMETHOD Vector2& operator +=(const Vector2& arg);


     IMETHOD friend Vector2 operator*(const Vector2& lhs,double rhs);
     IMETHOD friend Vector2 operator*(double lhs,const Vector2& rhs);
     IMETHOD friend Vector2 operator/(const Vector2& lhs,double rhs);
     IMETHOD friend Vector2 operator+(const Vector2& lhs,const Vector2& rhs);
     IMETHOD friend Vector2 operator-(const Vector2& lhs,const Vector2& rhs);
     IMETHOD friend Vector2 operator*(const Vector2& lhs,const Vector2& rhs);
     IMETHOD friend Vector2 operator-(const Vector2& arg);
     IMETHOD friend void SetToZero(Vector2& v);
        
     //! @return a zero 2D vector.
     IMETHOD static Vector2 Zero();
     
   /** Normalizes this vector and returns it norm
	* makes v a unitvector and returns the norm of v.
	* if v is smaller than eps, Vector(1,0,0) is returned with norm 0.
	* if this is not good, check the return value of this method.
	*/
     double Normalize(double eps=epsilon);

	 //! OBSOLETE : TODO : REMOVE THIS METHOD
	 Vector2 Normalize2();
     //!  @return the norm of the vector    
     IMETHOD double Norm() const;
    
     //! projects v in its XY plane, and sets *this to these values
     IMETHOD void Set3DXY(const Vector& v);
     
     //! projects v in its YZ plane, and sets *this to these values
     IMETHOD void Set3DYZ(const Vector& v);
     
     //! projects v in its ZX plane, and sets *this to these values
     IMETHOD void Set3DZX(const Vector& v);

     //! projects v_someframe in the XY plane of F_someframe_XY, 
     //! and sets *this to these values
     //! expressed wrt someframe.
     IMETHOD void Set3DPlane(const Frame& F_someframe_XY,const Vector& v_someframe);


     //! do not use operator == because the definition of Equal(.,.) is slightly
     //! different.  It compares whether the 2 arguments are equal in an eps-interval    
     IMETHOD friend bool Equal(const Vector2& a,const Vector2& b,double eps=epsilon);

    friend class Rotation2;
};


//! A 2D Rotation class, for conventions see Rotation. For further documentation
//! of the methods see Rotation class.
class Rotation2
{
    double s,c;
    //! c,s represent  cos(angle), sin(angle), this also represents first col. of rot matrix
    //! from outside, this class behaves as if it would store the complete 2x2 matrix.
public:
    //! Default constructor does NOT initialise to Zero().
    Rotation2() {}

    explicit Rotation2(double angle_rad):s(sin(angle_rad)),c(cos(angle_rad)) {}

    Rotation2(double ca,double sa):s(sa),c(ca){}

     IMETHOD Rotation2& operator=(const Rotation2& arg);
     IMETHOD Vector2 operator*(const Vector2& v) const;
     //!    Access to elements 0..1,0..1, bounds are checked when NDEBUG is not set
     IMETHOD double operator() (int i,int j) const;
     
     IMETHOD friend Rotation2 operator *(const Rotation2& lhs,const Rotation2& rhs);

     IMETHOD void SetInverse();
     IMETHOD Rotation2 Inverse() const;
     IMETHOD Vector2 Inverse(const Vector2& v) const;

     IMETHOD void SetIdentity();
     IMETHOD static Rotation2 Identity();

    
     //! The SetRot.. functions set the value of *this to the appropriate rotation matrix.
     IMETHOD void SetRot(double angle);
     
     //! The Rot... static functions give the value of the appropriate rotation matrix bac
     IMETHOD static Rotation2 Rot(double angle);

     //! Gets the angle (in radians)
     IMETHOD double GetRot() const;
     
     //! do not use operator == because the definition of Equal(.,.) is slightly
     //! different.  It compares whether the 2 arguments are equal in an eps-interval    
     IMETHOD friend bool Equal(const Rotation2& a,const Rotation2& b,double eps=epsilon);
};

//! A 2D frame class, for further documentation see the Frames class
//! for methods with unchanged semantics.
class Frame2
 {
public:
    Vector2 p;          //!< origine of the Frame
    Rotation2 M;        //!< Orientation of the Frame

public:

     IMETHOD Frame2(const Rotation2& R,const Vector2& V);
     EXPLICIT IMETHOD Frame2(const Vector2& V);
     EXPLICIT IMETHOD Frame2(const Rotation2& R);
     IMETHOD Frame2(void);
     IMETHOD Frame2(const Frame2& arg);
     IMETHOD void Make4x4(double* d);
    
     //!  Treats a frame as a 3x3 matrix and returns element i,j
     //!    Access to elements 0..2,0..2, bounds are checked when NDEBUG is not set
     IMETHOD double operator()(int i,int j);

     //!  Treats a frame as a 4x4 matrix and returns element i,j
     //!  Access to elements 0..3,0..3, bounds are checked when NDEBUG is not set
     IMETHOD double operator() (int i,int j) const;

     IMETHOD void SetInverse();
     IMETHOD Frame2 Inverse() const;
     IMETHOD Vector2 Inverse(const Vector2& arg) const; 
     IMETHOD Frame2& operator = (const Frame2& arg);
     IMETHOD Vector2 operator * (const Vector2& arg);   
     IMETHOD friend Frame2 operator *(const Frame2& lhs,const Frame2& rhs);
     IMETHOD void SetIdentity();
     IMETHOD void Integrate(const Twist& t_this,double frequency);
     IMETHOD static Frame2 Identity() {
        Frame2 tmp;
        tmp.SetIdentity();
        return tmp;
     }
     IMETHOD friend bool Equal(const Frame2& a,const Frame2& b,double eps=epsilon);
};



#ifdef FRAMES_INLINE
#include "vector.inl"
#include "wrench.inl"
#include "rotation.inl"
#include "frame.inl"
#include "twist.inl"
#include "vector2.inl"
#include "rotation2.inl"
#include "frame2.inl"
#endif



#ifdef USE_NAMESPACE
}
#endif



#endif // ifndef FRAMES
