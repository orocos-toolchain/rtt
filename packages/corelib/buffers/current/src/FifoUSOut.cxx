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

