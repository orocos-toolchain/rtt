/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:43:35 CEST 2002  cpp.hpp 

                       cpp.hpp -  description
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

#ifndef _RT_NEW_HH_
#define _RT_NEW_HH_

// just include the fosi
#include "../fosi.h"

extern "C" void *rt_malloc( unsigned int size );
extern "C" void rt_free( void *p );

void* operator new( size_t );
void* operator new [] ( size_t );
void operator delete( void* );
void operator delete [] ( void* );

inline void* operator new( size_t __size, void* __p )
{
    return __p;
}

inline void* operator new[] ( size_t, void* __p )
{
    return __p;
}

/**
 * @see crt.c for these functions
 */
extern "C"
{
    void __do_global_ctors( void );
    void __do_global_dtors( void );
}

extern "C"
{
    int init_module( void );
    void cleanup_module( void );
}

#endif // _RT_NEW_HH_

