/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:30 CEST 2004  BufferSimple.cxx 

                        BufferSimple.cxx -  description
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
#include "corelib/BufferSimple.hpp"

namespace ORO_CoreLib
{

    BufferSimple::BufferSimple( size_t size )
    {
        for ( int i = 0; i < MAX_BUFFERS; i++ )
        {
            buffers[ i ] = new char[ size ];
            lengths[ i ] = 0;
        }

        buffersize = size;
        readBuffer = 0;
        writeBuffer = 0;
        nextReadBuffer = 0;
    }

    BufferSimple::~BufferSimple()
    {
        for ( int i = 0; i < MAX_BUFFERS; i++ )
            delete[] buffers[ i ];
    }

    int BufferSimple::write( const char* buffer, size_t length )
    {
        size_t writeLength;

        if ( length > buffersize )
        {
            writeLength = buffersize;
        }

        else
        {
            writeLength = length;
        }

        memcpy( buffers[ writeBuffer ], buffer, writeLength );
        lengths[ writeBuffer ] = writeLength;
        updatePointerWrite();
        return writeLength;
    }

    int BufferSimple::read( char *buffer, size_t length )
    {
        size_t readLength;
        updatePointerRead();
        readLength = lengths[ readBuffer ];

        if ( readLength > length )
        {
            readLength = length;
        }

        memcpy( buffer, buffers[ readBuffer ], readLength );
        lengths[ readBuffer ] = 0; // we've read this buffer, so clear it
        return readLength;
    }

    size_t BufferSimple::capacityGet()
    {
        return buffersize;
    }


    void BufferSimple::updatePointerWrite()
    {
        ORO_OS::MutexLock Locker( pointerLock );
        unsigned int nextWrite = 0;
        /*
         * make sure that the next write buffer isn't the
         * current write buffer or the read buffer.
         * These 2 equals if's _are_ necessary!
         */

        if ( nextWrite == writeBuffer || nextWrite == readBuffer )
        {
            nextWrite++;
        }

        if ( nextWrite == writeBuffer || nextWrite == readBuffer )
        {
            nextWrite++;
        }

        nextReadBuffer = writeBuffer;
        writeBuffer = nextWrite;
    }

    void BufferSimple::updatePointerRead()
    {
        // quick lock to change read pointer
        // to nextRead, so we can read the most recent data
        ORO_OS::MutexLock Locker( pointerLock );
        readBuffer = nextReadBuffer;
    }

}

