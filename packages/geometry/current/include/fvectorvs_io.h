/*****************************************************************************
 * \file  
 *      Implements I/O operation on FVectorVSIZE<T,N>
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
 *      $Id: fvectorvs_io.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/
#ifndef FVectorVSIZE_IO_H
#define FVectorVSIZE_IO_H

#include <utilities/utility_io.h>
#include <algebra/fvectorvs.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

template <class T,int N>
inline ostream& operator << (ostream& os,const FVectorVSIZE<T,N>& v)
            {
            os << "FVectorVSIZE(";
            for (int i=0;i<VSIZE-1;i++) {
                os << v[i] << ", ";
            }
            os << v[VSIZE-1] << ")";
            return os;
            }



#ifdef USE_NAMESPACE
}
#endif
#endif
