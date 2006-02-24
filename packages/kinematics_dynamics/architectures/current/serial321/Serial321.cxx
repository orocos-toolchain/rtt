/***************************************************************************
  tag: Peter Soetens  Fri Feb 24 17:02:58 CET 2006  Serial321.cxx 

                        Serial321.cxx -  description
                           -------------------
    begin                : Fri February 24 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#include "kindyn/Serial321.hpp"

namespace ORO_KinDyn
{
    const double Serial321::M_PI_T2;

#ifndef SQRT3d2
    const double Serial321::SQRT3d2;
#endif
#ifndef SQRT3t2
    const double Serial321::SQRT3t2;
#endif

    const double Serial321::EPS_WRIST_ABOVE_BASE;

    const double Serial321::EPS_ARM_EXTENDED;

    const double Serial321::EPS_PARALLEL_WRIST;
    
    const double Serial321::KINEMATICS_EPS;

        Serial321::Serial321()
            : offset(0), eccent( 0 )
        {
            geometrySet( 1, 1, 1, 1, 1, 1);
        }

        Serial321::~Serial321() {}

}
