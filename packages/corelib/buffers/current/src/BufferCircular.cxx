/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:30 CEST 2004  BufferCircular.cxx 

                        BufferCircular.cxx -  description
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
#include <corelib/BufferCircular.hpp>
#include <os/Mutex.hpp>
#include <os/MutexLock.hpp>

namespace ORO_CoreLib
{
    BufferCircular::BufferCircular( size_t size ) : _size( size )
    {
        _buffer = new char[ _size ];
        _rdptr = 0;
        _wrptr = 0;
    }

    BufferCircular::~BufferCircular()
    {
        delete[] _buffer;
    }

    size_t BufferCircular::capacityGet()
    {
        return _size;
    }

    int BufferCircular::write( const char * buffer, size_t size )
    {
        ORO_OS::MutexLock locker( bufLock );
        size_t hulp = 0; // to store the number of characters actually written

        while ( ( ( _wrptr + hulp ) % _size != _rdptr ) && ( hulp <= size ) )
        {
            _buffer[ ( _wrptr + hulp ) % size ] = buffer[ hulp ];
            hulp++;
        }

        _wrptr += hulp;
        return hulp;
    }

    int BufferCircular::read( char * buffer, size_t size )
    {
        ORO_OS::MutexLock locker( bufLock );
        size_t hulp = 0;

        while ( ( ( _rdptr + hulp ) % size != _wrptr ) && ( hulp <= size ) )
        {
            buffer[ hulp ] = _buffer[ ( _rdptr + hulp ) % size ];
            hulp++;
        }

        _rdptr += hulp;
        return hulp;
    }
}
