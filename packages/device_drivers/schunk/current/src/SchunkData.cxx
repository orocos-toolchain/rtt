/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:33 CEST 2004  SchunkData.cxx 

                        SchunkData.cxx -  description
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
// Jan Veldeman
// 2-8-2002

#include "corelib/SchunkData.hpp"

namespace ORO_DeviceDriver
{

    inline void SchunkData::setOverloaded( bool overload )
    {
        _overload = overload;
    }

    inline bool SchunkData::getOverloaded()
    {
        return _overload;
    }

    inline void SchunkData::setValue( int idx, int value )
    {
        data[ idx ] = value;
    }

    inline int SchunkData::getXForce()
    {
        return data[ 0 ];
    }

    inline int SchunkData::getYForce()
    {
        return data[ 1 ];
    }

    inline int SchunkData::getZForce()
    {
        return data[ 2 ];
    }

    inline int SchunkData::getXTorque()
    {
        return data[ 3 ];
    }

    inline int SchunkData::getYTorque()
    {
        return data[ 4 ];
    }

    inline int SchunkData::getZTorque()
    {
        return data[ 5 ];
    }

    SchunkData::~SchunkData()
    {}

} // namespace

