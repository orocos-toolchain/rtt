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
