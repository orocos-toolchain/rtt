/*****************************************************************************
 * \file  
 *		 Utility routines used by chain_xxx.h files in connection with newmat
 *  \author 
 *  	Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *		ORO_Geometry V0.2
 *
 *	\par History
 *		- $log$
 *
 *	\par Release
 *		$Id: chain_newmat.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/

#ifndef __CHAIN_NEWMAT_H
#define __CHAIN_NEWMAT_H

#include <newmat/newmat.h>
#include <frames/frames.h>
#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

/**
 * some newmat related code
 */
 

//! Converts a twist into a ColumnVector
inline NEWMAT::ColumnVector CV(const Twist& t) {
	NEWMAT::ColumnVector c(6);
	for (int i=0;i<6;i++) c(i+1)=t(i);
	return c;
}

//! Converts a ColumnVector in a twist
inline Twist TW(const NEWMAT::ColumnVector & c) {
	Twist t;
	for (int i=0;i<6;i++) t(i)=c(i+1);
	return t;
}


#ifdef USE_NAMESPACE
}
#endif

#endif 
