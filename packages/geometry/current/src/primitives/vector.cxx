/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:36 CEST 2004  vector.cxx 

                        vector.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 *      Implementation of vector.h
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
 *      $Id: frames.cpp,v 1.1.1.1.2.1 2003/01/06 16:14:02 psoetens Exp $
 *      $Name:  $ 
 ****************************************************************************/
#include "geometry/frames.h"
 
#ifdef USE_NAMESPACE
    namespace ORO_Geometry {
#endif

#ifndef FRAMES_INLINE
    #include "geometry/vector.inl"
#endif


// do some effort not to lose precision
double Vector::Norm() const
{
    double tmp1;
    double tmp2;
    tmp1 = fabs(data[0]);
    tmp2 = fabs(data[1]);
    if (tmp1 >= tmp2) {
        tmp2=fabs(data[2]);
        if (tmp1 >= tmp2) {
        	if (tmp1 == 0) {
        		// only to everything exactly zero case, all other are handled correctly
        		return 0;
        	}
            return tmp1*sqrt(1+sqr(data[1]/data[0])+sqr(data[2]/data[0]));
        } else {
            return tmp2*sqrt(1+sqr(data[0]/data[2])+sqr(data[1]/data[2]));
        }
    } else {
        tmp1=fabs(data[2]);
        if (tmp2 > tmp1) {
            return tmp2*sqrt(1+sqr(data[0]/data[1])+sqr(data[2]/data[1]));
        } else {
            return tmp1*sqrt(1+sqr(data[0]/data[2])+sqr(data[1]/data[2]));
        }
    }
}

// makes v a unitvector and returns the norm of v.
// if v is smaller than eps, Vector(1,0,0) is returned with norm 0.
// if this is not good, check the return value of this method.
double Vector::Normalize(double eps) {
	double v = this->Norm();
	if (v < eps) {
		*this = Vector(1,0,0);
		return v;
	} else {
		*this = (*this)/v;
		return v;
	}
}

Vector Vector::Normalize2() const
{
    double N;
    N = Norm();
    return Vector(data[0]/N,data[1]/N,data[2]/N);
}


#ifdef USE_NAMESPACE
}
#endif
