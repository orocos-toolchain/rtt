/*****************************************************************************
 * \file  
 *      provides I/O operations on Rall1d
 *       
 *  \author 
 *      Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *      ORO_Geometry V2
 *
 *  \par History
 *      - $log$
 *
 *  \par Release
 *      $Id: frames_io.cpp,v 1.1.1.1.2.3 2003/06/26 15:23:59 psoetens Exp $
 *      $Name:  $ 
 *
 ****************************************************************************/

#include "geometry/utility.h"

#include "geometry/error.h"
#include "geometry/error_stack.h"
#include "geometry/frames.h"
#include "geometry/frames_io.h"

#if !(defined HAVE_FOSI) || (defined HAVE_IOSTREAM)
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#endif


#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif
int FRAMEWIDTH = 12;

    using namespace std;

ostream& operator << (ostream& os,const Vector& v) {
    os << "[" << setw(FRAMEWIDTH) << v(0) << "," << setw(FRAMEWIDTH)<<v(1) 
       << "," << setw(FRAMEWIDTH) << v(2) << "]";
    return os;
}

ostream& operator << (ostream& os,const Twist& v) {
    os << "[" << setw(FRAMEWIDTH) << v.vel(0) 
       << "," << setw(FRAMEWIDTH) << v.vel(1) 
       << "," << setw(FRAMEWIDTH) << v.vel(2) 
       << "," << setw(FRAMEWIDTH) << v.rot(0) 
       << "," << setw(FRAMEWIDTH) << v.rot(1) 
       << "," << setw(FRAMEWIDTH) << v.rot(2) 
       << "]";
    return os;
}

ostream& operator << (ostream& os,const Wrench& v) {
    os << "[" << setw(FRAMEWIDTH) << v.force(0) 
       << "," << setw(FRAMEWIDTH) << v.force(1) 
       << "," << setw(FRAMEWIDTH) << v.force(2) 
       << "," << setw(FRAMEWIDTH) << v.torque(0) 
       << "," << setw(FRAMEWIDTH) << v.torque(1) 
       << "," << setw(FRAMEWIDTH) << v.torque(2) 
       << "]";
    return os;
}


ostream& operator << (ostream& os,const Rotation& R) {
    os << "[";
    for (int i=0;i<=2;i++) {
        os << setw(FRAMEWIDTH) << R(i,0) << "," <<
                       setw(FRAMEWIDTH) << R(i,1) << "," <<
                       setw(FRAMEWIDTH) << R(i,2);
        if (i<2) 
            os << ";"<< endl << " ";
        else
            os << "]";
    }

    return os;
}

ostream& operator << (ostream& os, const Frame& T) 
{
    os << "[" << T.M << endl<< T.p << "]";
    return os;
}

ostream& operator << (ostream& os,const Vector2& v) {
    os << "[" << setw(FRAMEWIDTH) << v(0) << "," << setw(FRAMEWIDTH)<<v(1) 
       << "]";
    return os;
}

// Rotation2 gives back an angle in degrees with the << and >> operators.
ostream& operator << (ostream& os,const Rotation2& R) {
    os << "[" << R.GetRot()*rad2deg << "]";
    return os;
}

ostream& operator << (ostream& os, const Frame2& T) 
{
    os << T.M << T.p;
    return os;
}


istream& operator >> (istream& is,Vector& v) 
{   IOTrace("Stream input Vector (vector or ZERO)");
    char storage[10];
    EatWord(is,"[]",storage,10);
    if (strlen(storage)==0) {
        Eat(is,'['); 
        is >> v(0);
        Eat(is,',');
        is >> v(1);
        Eat(is,','); 
        is >> v(2);
        EatEnd(is,']');
        IOTracePop();
        return is;
    }
    if (strcmp(storage,"ZERO")==0) {
        v = Vector::Zero();
        IOTracePop();
        return is;
    }
#ifdef HAVE_RTSTL
                Error_Frame_Vector_Unexpected_id err;
                cout << err.Description();
                abort();
#else
    throw Error_Frame_Vector_Unexpected_id();
#endif
}

istream& operator >> (istream& is,Twist& v) 
{   IOTrace("Stream input Twist");
    Eat(is,'['); 
    is >> v.vel(0);
    Eat(is,',');
    is >> v.vel(1);
    Eat(is,','); 
    is >> v.vel(2);
    Eat(is,','); 
    is >> v.rot(0);
    Eat(is,','); 
    is >> v.rot(1);
    Eat(is,','); 
    is >> v.rot(2);
    EatEnd(is,']');
    IOTracePop();
    return is;
}

istream& operator >> (istream& is,Wrench& v) 
{   IOTrace("Stream input Wrench");
    Eat(is,'['); 
    is >> v.force(0);
    Eat(is,',');
    is >> v.force(1);
    Eat(is,','); 
    is >> v.force(2);
    Eat(is,','); 
    is >> v.torque(0);
    Eat(is,','); 
    is >> v.torque(1);
    Eat(is,','); 
    is >> v.torque(2);
    EatEnd(is,']');
    IOTracePop();
    return is;
}

istream& operator >> (istream& is,Rotation& r)
{   IOTrace("Stream input Rotation (Matrix or EULERZYX, EULERZYZ,RPY, ROT, IDENTITY)");
    char storage[10];
    EatWord(is,"[]",storage,10);
    if (strlen(storage)==0) {
        Eat(is,'[');
        for (int i=0;i<3;i++) {
            is >> r(i,0);
            Eat(is,',') ;
            is >> r(i,1);
            Eat(is,',');
            is >> r(i,2);
            if (i<2) 
                Eat(is,';');
            else
                EatEnd(is,']');
        }
        IOTracePop();
        return is;
    }
    Vector v;
    if (strcmp(storage,"EULERZYX")==0) {
        is >> v;
        v=v*deg2rad;
        r = Rotation::EulerZYX(v(0),v(1),v(2));
        IOTracePop();
        return is;
    }
    if (strcmp(storage,"EULERZYZ")==0) {
        is >> v;
        v=v*deg2rad;
        r = Rotation::EulerZYZ(v(0),v(1),v(2));
        IOTracePop();
        return is;
    }
    if (strcmp(storage,"RPY")==0) {
        is >> v;
        v=v*deg2rad;
        r = Rotation::RPY(v(0),v(1),v(2));
        IOTracePop();
        return is;
    }
    if (strcmp(storage,"ROT")==0) {
        is >> v;
        double angle;
        Eat(is,'['); 
        is >> angle;
        EatEnd(is,']');
        r = Rotation::Rot(v,angle*deg2rad);
        IOTracePop();
        return is;
    }
    if (strcmp(storage,"IDENTITY")==0) {
        r = Rotation::Identity();
        IOTracePop();
        return is;
    }
#ifdef HAVE_RTSTL
    Error_Frame_Rotation_Unexpected_id err;
    cout << err.Description();
    abort();
#else
    throw Error_Frame_Rotation_Unexpected_id();
#endif
    return is;
}

istream& operator >> (istream& is,Frame& T)
{   IOTrace("Stream input Frame (Rotation,Vector) or DH[...]");
    char storage[10];
    EatWord(is,"[",storage,10);
    if (strlen(storage)==0) {
        Eat(is,'[');
        is >> T.M;
        is >> T.p;
        EatEnd(is,']');
        IOTracePop();
        return is;
    }
    if (strcmp(storage,"DH")==0) {
        double a,alpha,d,theta;
        Eat(is,'[');
        is >> a;
        Eat(is,',');
        is >> alpha;
        Eat(is,',');
        is >> d;
        Eat(is,',');
        is >> theta;
        EatEnd(is,']');
        T = Frame::DH(a,alpha*deg2rad,d,theta*deg2rad);
        IOTracePop();
        return is;
    }
#ifdef HAVE_RTSTL
    Error_Frame_Frame_Unexpected_id err;
    cout << err.Description();
    abort();
#else
    throw Error_Frame_Frame_Unexpected_id();
#endif
    return is;
}

istream& operator >> (istream& is,Vector2& v) 
{   IOTrace("Stream input Vector2");
    Eat(is,'['); 
    is >> v(0);
    Eat(is,',');
    is >> v(1);
    EatEnd(is,']');
    IOTracePop();
    return is;
}
istream& operator >> (istream& is,Rotation2& r)
{   IOTrace("Stream input Rotation2");
    Eat(is,'[');
    double val;
    is >> val;
    r.Rot(val*deg2rad);
    EatEnd(is,']');
    IOTracePop();
    return is;
}
istream& operator >> (istream& is,Frame2& T)
{   IOTrace("Stream input Frame2");
    is >> T.M;
    is >> T.p;
    IOTracePop();
    return is;
}

#ifdef USE_NAMESPACE
} // namespace Frame
#endif
