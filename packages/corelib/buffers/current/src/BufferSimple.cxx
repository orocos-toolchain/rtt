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

