/*****************************************************************************
 * \file  
 *      provides I/O operations on Rall1d
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
 *      $Id: rall1d_io.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/
#ifndef Rall_IO_H
#define Rall_IO_H

#include <utilities/utility_io.h>
#include <algebra/rall1d.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

template <class T,class V,class S>
inline ostream& operator << (ostream& os,const Rall1d<T,V,S>& r)
            {
            os << "Rall1d(" << r.t <<"," << r.grad <<")";
            return os;
            }


#ifdef USE_NAMESPACE
}
#endif


#endif
