/*****************************************************************************
 * \file  
 *      provides I/O operations on RNFrames classes
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
 *      $Id: rnframes_io.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/
#ifndef RNFRAMES_IO
#define RNFRAMES_IO

#include <utilities/utility_io.h>
#include <frames/frames_io.h>
#include <algebra/rall1d_io.h>
#include <rframes/rnframes.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


inline ostream& operator << (ostream& os,const RNVector& arg) {
    os << "[" << arg.p;
    int i;
    for (i=0;i<VSIZE;i++) {
        os << "," << endl<< arg.v[i];
    }
    os << "]";
    return os;
}
inline ostream& operator << (ostream& os,const RNRotation& arg) {
    os << "[" << arg.R;
    int i;
    for (i=0;i<VSIZE;i++) {
        os << "," << endl << arg.w[i];
    }
    os << "]";
    return os;
}
inline ostream& operator << (ostream& os,const RNFrame& arg) {
    os << "[" << arg.M;
    os << "," << arg.p;
    os << "]";
    return os;
}
inline ostream& operator << (ostream& os,const RNTwist& arg) {
    os << "[" << arg.vel;
    os << "," << arg.rot;
    os << "]";
    return os;
}






#ifdef USE_NAMESPACE
} // namespace Frame
#endif



#endif
