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

namespace CBDeviceDriver
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
