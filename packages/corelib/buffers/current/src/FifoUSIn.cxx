#include "corelib/FifoUSIn.hpp"

#include "os/rtconversions.hpp"

namespace ORO_CoreLib
{

    FifoUSIn::FifoUSIn( unsigned int f_nr )
        : okr_len(0), fNr( f_nr )
    {
        std::string thefifo("/dev/rtf");
        thefifo += int_to_string(fNr);

        fifo = fopen( thefifo.c_str(), "r" );

        if ( fifo == 0 )
            ::printf( "Error opening fifo %s !", thefifo.c_str() );

    }

    FifoUSIn::~FifoUSIn()
    {
        if ( fifo != 0 )
            fclose( fifo );
    }

    int FifoUSIn::read( char* buf, size_t length )
    {
        buf[0] = 0; // eases debugging
        if (fifo != 0)
            {
                unsigned int r_len, readBytes=0;
                // bytes of data ready in buffer
                fread(&r_len, sizeof(int), 1, fifo);
                if (r_len <= length)
                    {
                        okr_len = r_len;
                        while ( readBytes !=  r_len)
                            {
                                void* addr = &buf[readBytes];
                                readBytes += fread(addr, sizeof(char), r_len-readBytes, fifo);
                            }
                        return readBytes + sizeof(int);
                    }
                else
                    {
                        // something went very wrong... try to recover !
                        if (feof(fifo)) printf("End of File!\n");
                        if (ferror(fifo)) printf("Error in File!\n");
                        clearerr(fifo);
                        // todo : seek new start point
                        return 0;
                    }
            }
        return 0;
    }

    unsigned int FifoUSIn::fifoNr() const
    {
        return fNr;
    }


}


