
#include "corelib/FifoRTCommon.hpp"

namespace ORO_CoreLib
{
    
    /**
     * This is the place where we keep information about the Event
     * object (dataReady) of a fifo. 
    */
    fifo_info_t rt_fifos[ MAX_NR_FIFOS ];

    int getFreeFifoNr()
    {
        for ( int i = 0; i < MAX_NR_FIFOS; i++ )
        {
            if ( !rt_fifos[ i ].inUse )
            {
                return i;
            }
        }
        return -1;
    }

}

