/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:35 CEST 2004  debug_macros.cxx 

                        debug_macros.cxx -  description
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
 *		$Id: debug_macros.cpp,v 1.1.1.1.2.1 2003/01/06 16:14:02 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#include "geometry/debug_macros.h"
#ifdef WANT_STD_IOSTREAM
    using namespace std;
#endif

ofstream& logger() {
  static ofstream log("Verify.log",ios::app);
  return log;
}
ofstream& errorlogger() {
  static ofstream errorlog("Error.log",ios::app);
  return errorlog;
}
int DEBUG_RETURNVALUE = 0;
