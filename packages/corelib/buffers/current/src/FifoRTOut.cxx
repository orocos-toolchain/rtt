/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:30 CEST 2004  FifoRTOut.cxx 

                        FifoRTOut.cxx -  description
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

#include "corelib/FifoRTOut.hpp"
#include "os/fosi.h"

namespace ORO_CoreLib
{

    extern fifo_info_t rt_fifos[ MAX_NR_FIFOS ];

    FifoRTOut::FifoRTOut( unsigned int fifoNr, size_t bytes )
            : fifo( fifoNr ), created( true )
    {
        rtosf_create( fifo, bytes );
    }

    FifoRTOut::FifoRTOut( unsigned int fifoNr )
            : fifo( fifoNr ), created( false )
    {}

    FifoRTOut::~FifoRTOut()
    {
        if ( created )
        {
            rtosf_destroy( fifo );
        }
    }

    int FifoRTOut::write( const char* msg, size_t bytes )
    {
        rtosf_put(fifo, (char*) &bytes, sizeof(size_t) );
        return rtosf_put( fifo, msg, bytes );
    }

    int FifoRTOut::put( char c )
    {
        const size_t size=1;
        rtosf_put(fifo, (char*)&size, sizeof(size_t) );
        return (rtosf_put( fifo, &c, 1) == 1 ? 0 : -1 );
    }

    unsigned int FifoRTOut::fifoNr() const
    {
        return fifo;
    }

} // namespace
