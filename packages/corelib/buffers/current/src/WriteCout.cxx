/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:30 CEST 2004  WriteCout.cxx 

                        WriteCout.cxx -  description
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
#include "corelib/WriteCout.hpp"

#include <os/rtstreams.hpp>

namespace ORO_CoreLib
{
    
    int WriteCout::write( const char* buf, size_t length )
    {
        rt_std::cout << buf;
        // TODO: Check if cout DID write buf, and if not, return -1
        return length;
    }

    int WriteCout::put( char c)
    {
        rt_std::cout << c;
        return 0;
    }

    WriteCout::WriteCout()
    {
    }

    WriteCout::~WriteCout()
    {
    }

}

