
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


