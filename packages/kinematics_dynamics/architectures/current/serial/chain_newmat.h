/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 15:59:17 CET 2005  chain_newmat.h 

                        chain_newmat.h -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Erwin Aertbelien
    email                : erwin.aertbelien@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
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
