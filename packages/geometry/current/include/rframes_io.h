/*****************************************************************************
 * \file  
 *      provides I/O operations on RFrames classes
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
 *      $Id: rframes_io.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/
#ifndef RFRAMES_IO
#define RFRAMES_IO

#include <utilities/utility_io.h>
#include <rframes/rframes.h>
#include <frames/frames_io.h>
#include <algebra/rall1d_io.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

// Output...
inline ostream& operator << (ostream& os,const RVector& r) {
    os << "{" << r.p << "," << r.v << "}" << endl;
    return os;
}

inline ostream& operator << (ostream& os,const RRotation& r) {
    os << "{" << endl << r.R << "," << endl << r.w << endl << "}" << endl;
    return os;
}


inline ostream& operator << (ostream& os,const RFrame& r) {
    os << "{" << endl << r.M << "," << endl << r.p << endl << "}" << endl;
    return os;
}

inline ostream& operator << (ostream& os,const RTwist& r) {
    os << "{" << endl << r.vel << "," << endl << r.rot << endl << "}" << endl;
    return os;
}


#ifdef USE_NAMESPACE
} // namespace Frame
#endif



#endif
