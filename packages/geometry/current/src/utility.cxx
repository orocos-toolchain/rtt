/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:35 CEST 2004  utility.cxx 

                        utility.cxx -  description
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
/** @file   utility.cpp
 *  @author Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *  @version 
 *      ORO_Geometry V0.2
 *   
 *  @par history
 *   - changed layout of the comments to accomodate doxygen
 */
 
#include "geometry/utility.h"

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

int STREAMBUFFERSIZE=10000;
int MAXLENFILENAME = 255;
const double PI=       3.1415926535897932384626433832795;
const double deg2rad = 0.01745329251994329576923690768488;
const double rad2deg = 57.2957795130823208767981548141052;
double epsilon = 0.0000001;
int VSIZE = VSIZEMAX;

#ifdef USE_NAMESPACE
}
#endif
