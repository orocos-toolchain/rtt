/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 16:38:39 CET 2005  rrframes_io.h 

                        rrframes_io.h -  description
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
 *      Defines I/O related routines to the RRFrames classes defined in 
 *      RRFrames.h
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
 *      $Id: rrframes_io.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $ 
 ****************************************************************************/
#ifndef RRFRAMES_IO
#define RRFRAMES_IO

#include "utility_io.h"
#include "frames_io.h"
#include "rall2d_io.h"

#include "rrframes.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif


// Output...
inline ostream& operator << (ostream& os,const RRVector& r) {
    os << "{" << r.p << "," << r.v << "," << r.dv << "}" << endl;
    return os;
}

inline ostream& operator << (ostream& os,const RRRotation& r) {
    os << "{" << endl << r.R << "," << endl << r.w << 
          "," << endl << r.dw << endl << "}" << endl;
    return os;
}


inline ostream& operator << (ostream& os,const RRFrame& r) {
    os << "{" << endl << r.M << "," << endl << r.p << "}" << endl;
    return os;
}
inline ostream& operator << (ostream& os,const RRTwist& r) {
    os << "{" << endl << r.vel << "," << endl << r.rot << endl << "}" << endl;
    return os;
}


#ifdef USE_NAMESPACE
} // namespace Frame
#endif



#endif
