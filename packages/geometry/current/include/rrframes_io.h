/*****************************************************************************
 * \file  
 *      Defines I/O related routines to the RRFrames classes defined in 
 *      RRFrames.h
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
 *      $Id: rrframes_io.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/
#ifndef RRFRAMES_IO
#define RRFRAMES_IO

#include <utilities/utility_io.h>
#include <frames/frames_io.h>
#include <algebra/rall2d_io.h>

#include <rframes/rrframes.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


// Output...
inline ostream& operator << (ostream& os,const RRVector& r) {
    os << "{" << r.p << "," << r.v << "," << r.dv << "}" << endl;
    return os;
}

inline ostream& operator << (ostream& os,const RRRotation& r) {
    os << "{" << endl << r.R << "," << endl << r.w << 
          "," << endl << r.dw << endl << "}" << endl;
    return os;
}


inline ostream& operator << (ostream& os,const RRFrame& r) {
    os << "{" << endl << r.M << "," << endl << r.p << "}" << endl;
    return os;
}
inline ostream& operator << (ostream& os,const RRTwist& r) {
    os << "{" << endl << r.vel << "," << endl << r.rot << endl << "}" << endl;
    return os;
}


#ifdef USE_NAMESPACE
} // namespace Frame
#endif



#endif
