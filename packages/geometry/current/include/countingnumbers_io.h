/***************************************************************************
  tag: Erwin Aertbelien  Mon Jan 10 16:38:38 CET 2005  countingnumbers_io.h 

                        countingnumbers_io.h -  description
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
 * This file contains the definition of the I/O operations on classes 
 * for counting the number of operations on a float.
 * 
 *       
 *  \author Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *  \version 
 *      ORO_Geometry V2
 *  \par History
 *      - $log$
 * 
 *  \par Release
 *      $Id: countingnumbers_io.h,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *      $Name:  $   
 ****************************************************************************/
#ifndef COUNTINGNUMBERS_IO_H
#define COUNTINGNUMBERS_IO_H

#include <utilities/utility_io.h>
#include <algebra/countingnumbers.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

inline ostream& operator << (ostream& os,const Counting& v)
            {
            os << "Counting(" << v.value << ")";
            return os;
            }

#ifdef USE_NAMESPACE
}
#endif

#endif
