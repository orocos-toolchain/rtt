#ifndef ORO_FRAMES2_H
#define ORO_FRAMES2_H


#include "utility.h"


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

class Vector2;
class Rotation2;
class Frame2;



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



#ifdef USE_NAMESPACE
}
#endif


#ifdef FRAMES_INLINE
#include "vector2.inl"
#include "rotation2.inl"
#include "frame2.inl"
#endif




#endif // ifndef FRAMES
