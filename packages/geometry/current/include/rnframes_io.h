/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 16:38:39 CET 2005  rnframes_io.h 

                        rnframes_io.h -  description
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
