/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:36 CEST 2004  frame.cxx 

                        frame.cxx -  description
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
 *      Implementation of frames.h
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
    #include <frames/frame.inl>
#endif


void Frame::Make4x4(double * d)
{
    int i;
    int j;
    for (i=0;i<3;i++) {
        for (j=0;j<3;j++)
            d[i*4+j]=M(i,j);
        d[i*4+3] = p(i)/1000;
    }
    for (j=0;j<3;j++) 
        d[12+j] = 0.;
    d[15] = 1;
}

Frame Frame::DH(double a,double alpha,double d,double theta)
// returns Denavit-Hartenberg parameters
{
    double ct,st,ca,sa;
    ct = cos(theta);
    st = sin(theta);
    sa = sin(alpha);
    ca = cos(alpha);
    return Frame(Rotation(
                    ct,       -st,     0,
                    st*ca,  ct*ca,   -sa,
                    st*sa,  ct*sa,    ca   ),
                 Vector(
                    a,      -sa*d,  ca*d   )
            );
}

#ifdef USE_NAMESPACE
}
#endif
