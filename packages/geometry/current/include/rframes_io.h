/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 16:38:38 CET 2005  rframes_io.h 

                        rframes_io.h -  description
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
 *      provides I/O operations on RFrames classes
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
 *      $Id: rframes_io.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/
#ifndef RFRAMES_IO
#define RFRAMES_IO

#include <utilities/utility_io.h>
#include <rframes/rframes.h>
#include <frames/frames_io.h>
#include <algebra/rall1d_io.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

// Output...
inline ostream& operator << (ostream& os,const RVector& r) {
    os << "{" << r.p << "," << r.v << "}" << endl;
    return os;
}

inline ostream& operator << (ostream& os,const RRotation& r) {
    os << "{" << endl << r.R << "," << endl << r.w << endl << "}" << endl;
    return os;
}


inline ostream& operator << (ostream& os,const RFrame& r) {
    os << "{" << endl << r.M << "," << endl << r.p << endl << "}" << endl;
    return os;
}

inline ostream& operator << (ostream& os,const RTwist& r) {
    os << "{" << endl << r.vel << "," << endl << r.rot << endl << "}" << endl;
    return os;
}


#ifdef USE_NAMESPACE
} // namespace Frame
#endif



#endif
