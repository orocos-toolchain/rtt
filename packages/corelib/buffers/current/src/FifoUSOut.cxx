/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:30 CEST 2004  FifoUSOut.cxx 

                        FifoUSOut.cxx -  description
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
#include "corelib/FifoUSIn.hpp"
#include "os/rtconversions.hpp"
#include <string>
#include <stdarg.h>
#include <fcntl.h>

namespace ORO_CoreLib
{

    FifoUSOut::FifoUSOut( unsigned int fNr)
        :fifoNr(fNr)
    {
        std::string thefifo ("/dev/rtf");
        thefifo += inttostring(fNr);

        fifo = fopen( thefifo.c_str() , "w" );

        if ( fifo == 0 )
            ::printf( "Error opening fifo %s !", thefifo.c_str() );
    }

    FifoUSOut::~FifoUSOut()
    {
        if ( fifo != 0 )
            fclose( fifo );
    }

    int FifoUSOut::write( const char* buf, size_t write_length)
    {
        if ( fifo != 0 )
            {
                // write size
                fwrite( &write_length, sizeof( int ), 1, fifo );

                // buffer.length()+1 would write the \0 too to the fifo
                int rv = fwrite( buf, sizeof( char ), write_length, fifo ) + sizeof(int);
                ::printf( "Tried to write %d bytes, wrote %d to the fifo\n", write_length+sizeof(int), rv );
                fflush( fifo );
                return rv + sizeof(int);
            }

        return 0;
    }
}

