/*****************************************************************************
 * \file  
 *      Implements I/O operation on FVectorVSIZE2<T,N>
 * 
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
 *      $Id: fvectorvs2_io.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/
#ifndef FVECTORVSIZE2_IO_H
#define FVECTORVSIZE2_IO_H

#include <algebra/fvectorvs2.h>

#ifdef WANT_STD_IOSTREAM
    #include <iostream>
    using namespace std;
#else
    #include <iostream.h>
#endif

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

template <class T,int N,class S>
inline ostream& operator << (ostream& os,const FVectorVSIZE2<T,N,S>& v)
            {
            os << "FVectorVSIZE2(";
            for (int i=0;i<VSIZE-1;i++) {
                os << v[i] << ", ";
            }
            os << v[N-1] << ")";
            return os;
            }



#ifdef USE_NAMESPACE
}
#endif
#endif
