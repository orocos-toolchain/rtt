/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:33 CEST 2004  SchunkCommunicationFile.cxx 

                        SchunkCommunicationFile.cxx -  description
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
#include "corelib/SchunkCommunicationFile.hpp"

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/poll.h>
}

// TODO -> check flushInput

namespace ORO_DeviceDriver
{

    SchunkCommunicationFile::SchunkCommunicationFile()
    {
        // TODO constructor argument for device
        _file = ::fopen( "/dev/ttyS0", "a+" );

        if ( ! _file )
        {
            // fout ... tja
        }
    }

    SchunkCommunicationFile::~SchunkCommunicationFile()
    {
        if ( ! ::fclose( _file ) )
        {
            // fout ... tjah
        }
    }

    int SchunkCommunicationFile::send( char * buf, int count )
    {
        if ( _file )
        {
            return ::fwrite( buf, 1, count, _file );
        }
    }

    int SchunkCommunicationFile::read( char * buf, int count )
    {
        int ret;

        if ( _file )
        {
            return ::fread( buf, 1, count, _file );
        }
    }

    int SchunkCommunicationFile::flushInput()
    {
        if ( _file )
        {
            // XXX
            usleep( 1 );
            _file = ::freopen( "/dev/ttyS0", "a+", _file );
        }
    }
}
