/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:43:33 CEST 2002  cpp.cpp 

                       cpp.cpp -  description
                          -------------------
   begin                : Mon June 10 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


//////////////////////////////////////////////////////////////////////////////
//
//      Copyright (C) 2000 Pierre Cloutier (Poseidon Controls Inc.),
//                         All rights reserved
//
// Authors:             Pierre Cloutier (pcloutier@poseidoncontrols.com)
//
// Original date:       21 Oct 2000
//
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
//
// Dynamic Memory Management for Real Time Linux.
//
///////////////////////////////////////////////////////////////////////////////

#include "corelib/cpp.hpp"

void *
operator new( size_t size )
{
    return rt_malloc( size );
}

void *
operator new [] ( size_t size )
{
    return rt_malloc( size );
}

void
operator delete( void* p )
{
    if ( p == 0 )
    {
        rt_printk( "Tried to free null pointer !" );
        return ;
    }

    rt_free( p );
}

void
operator delete [] ( void* p )
{
    if ( p == 0 )
    {
        rt_printk( "Tried to free null Array pointer !" );
        return ;
    }

    rt_free( p );
}
