 
#ifndef FIFOUSOUT_HPP
#define FIFOUSOUT_HPP


#include "WriteInterface.hpp"
#include <cstdio>

namespace ORO_CoreLib
{

    /**
     * A FifoUSOut is meant for writing to a realtime fifo from
     * (not realtime) userspace. 
     *
     * @see FifoUSIn
     * @see FifoRTIn
     */
    class FifoUSOut
        : public WriteInterface
    {

    public:
        /**
         * Open a fifo with number <fNr> for writing.
         * 
         * @param fNr
         *        The fifo number
         */
        FifoUSOut( unsigned int fNr);

        /**
         * Close the fifo.
         */
        virtual ~FifoUSOut();

        int write( const char* buf, size_t write_length);

    private:
        FILE *fifo;
        unsigned int fifoNr;
    };
}

#endif
