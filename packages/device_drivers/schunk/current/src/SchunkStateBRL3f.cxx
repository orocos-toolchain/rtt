/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:33 CEST 2004  SchunkStateBRL3f.cxx 

                        SchunkStateBRL3f.cxx -  description
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

#include "corelib/SchunkStateBRL3f.hpp"

namespace ORO_DeviceDriver
{

    using namespace ORO_CoreLib;

    SchunkStateBRL3f::SchunkStateBRL3f( SchunkDevice *device ) :
            SchunkState( device )
    {
        dev = device;
    }

    void SchunkStateBRL3f::getLine( SchunkData* data )
    {
        dev->flushInput();
        sendCommand();
        processResult( data );
    }


    void SchunkStateBRL3f::sendCommand()
    {
        dev->_comm->send( "", 1 );
    }

    void SchunkStateBRL3f::processResult( SchunkData* data )
    {
        int i, val, idx = 0;
        char buf[ 13 ];
        dev->_comm->read( buf, 13 );
        data->setOverloaded( ( bool ) ( buf[ idx++ ] ) );

        for ( i = 0; i < 6; i++ )
        {
            val = ( int ) ( buf[ idx++ ] << 8 );
            data->setValue( i, val | buf[ idx++ ] );
        }
    }

    void SchunkStateBRL3f::imposeState()
    {
        char cmd[ 200 ];
        strcpy( cmd, "\r\n" );
        dev->_comm->send( cmd, strlen( cmd ) );
        dev->_comm->flushInput();
        strcpy( cmd, "CD B\r\n" );
        dev->_comm->send( cmd, strlen( cmd ) );
        dev->_comm->flushInput();
        strcpy( cmd, "CD R\r\n" );
        dev->_comm->send( cmd, strlen( cmd ) );
        dev->_comm->flushInput();
        strcpy( cmd, "CL 1\r\n" );
        dev->_comm->send( cmd, strlen( cmd ) );
        dev->_comm->flushInput();
        strcpy( cmd, "CV 3f\r\n" );
        dev->_comm->send( cmd, strlen( cmd ) );
        dev->_comm->flushInput();
    }
};


