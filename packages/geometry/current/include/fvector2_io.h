/*****************************************************************************
 * \file  
 *      Implements I/O operation on FVector2<T,N>
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
 *      $Id: fvector2_io.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/

#ifndef FVECTOR2_IO_H
#define FVECTOR2_IO_H

#include "FVector2.h"

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
inline ostream& operator << (ostream& os,const FVector2<T,N,S>& v)
            {
            os << "FVector2(";
            for (int i=0;i<N-1;i++) {
                os << v[i] << ", ";
            }
            os << v[N-1] << ")";
            return os;
            }



#ifdef USE_NAMESPACE
}
#endif
#endif
