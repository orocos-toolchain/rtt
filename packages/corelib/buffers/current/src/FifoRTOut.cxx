
#include "corelib/FifoRTOut.hpp"
#include "os/fosi.h"

namespace ORO_CoreLib
{

    extern fifo_info_t rt_fifos[ MAX_NR_FIFOS ];

    FifoRTOut::FifoRTOut( unsigned int fifoNr, size_t bytes )
            : fifo( fifoNr ), created( true )
    {
        rtosf_create( fifo, bytes );
    }

    FifoRTOut::FifoRTOut( unsigned int fifoNr )
            : fifo( fifoNr ), created( false )
    {}

    FifoRTOut::~FifoRTOut()
    {
        if ( created )
        {
            rtosf_destroy( fifo );
        }
    }

    int FifoRTOut::write( const char* msg, size_t bytes )
    {
        rtosf_put(fifo, (char*) &bytes, sizeof(size_t) );
        return rtosf_put( fifo, msg, bytes );
    }

    int FifoRTOut::put( char c )
    {
        const size_t size=1;
        rtosf_put(fifo, (char*)&size, sizeof(size_t) );
        return (rtosf_put( fifo, &c, 1) == 1 ? 0 : -1 );
    }

    unsigned int FifoRTOut::fifoNr() const
    {
        return fifo;
    }

} // namespace
