/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:30 CEST 2004  FifoRTIn.cxx 

                        FifoRTIn.cxx -  description
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
#include "corelib/FifoRTIn.hpp"
#include "os/fosi.h"


namespace ORO_CoreLib
{

    extern fifo_info_t rt_fifos[ MAX_NR_FIFOS ];
    extern int getFreeFifoNr();

    /**
     * @class FifoRTIn
     * @todo TODO: test handling of listeners.
     *
     * @warning
     * it is possible that when a fifo is created in user space,
     * no event is registered in rt_fifos. This must be fixed eventually.
     * For now: always create a fifo with a FifoRT... create call (not just
     * a wrapper).
     */
    FifoRTIn::FifoRTIn( unsigned int fifoNr, size_t bytes )
        : fifo( fifoNr ), created( true ), bytesReady(0)
    {
        if ( !rt_fifos[ fifo ].inUse )
        {
            rtosf_create( fifo, bytes );
            rt_fifos[ fifo ].dataReady = new Event( Event::SYNASYN );
            rt_fifos[ fifo ].inUse = true;
            rtosf_set_handler( fifo, fifoHandler );
        }
    }

    FifoRTIn::FifoRTIn( unsigned int fifoNr )
        : fifo( fifoNr ), created( false ), bytesReady(0)
    {
        // all handlers are allready in place
    }

    FifoRTIn::~FifoRTIn()
    {
        // ??? remove handler

        if ( created )
        {
            rtosf_destroy( fifo );
            delete rt_fifos[ fifo ].dataReady;
            rt_fifos[ fifo ].inUse = false;
            //            rt_fifos[fifo].dataReady = NULL;
        }
    }

    int FifoRTIn::read( char* buf, size_t length )
    {
        /**
         * The read algorithm extracts protocol info from the input stream
         * The protocol is : 1 message = int(n) + char*n
         */
        if ( bytesReady == 0 )
            {
                // read size of incomming message
                char c[sizeof(int)];
                int res = rtosf_get(fifo,c,sizeof(int));

                if ( res == sizeof(int) )
                    memcpy (&bytesReady,c,sizeof(int));
                else 
                    return 0;
            }
        unsigned int done;
        if (length <= bytesReady)
            done = rtosf_get( fifo, buf, length );
        else
            done = rtosf_get( fifo, buf, bytesReady );
        bytesReady -= done;

        return done;
    }

    int FifoRTIn::get( char& c)
    {
        unsigned int done = rtosf_get( fifo, &c, 1);
        return (done == 1 ? 0 : -1);
    }
    
    int FifoRTIn::fifoHandler( unsigned int fifo, int rw )
    {
        if ( char( rw ) == 'w' )  // 'w' means data is available
        {
            rt_fifos[ fifo ].dataReady->fire();
        }

        return 0;
    }

    unsigned int FifoRTIn::fifoNr() const
    {
        return fifo;
    }

    void FifoRTIn::addHandler( EventListenerInterface *eli, EventCompleterInterface *eci )
    {
        rt_fifos[ fifo ].dataReady->addHandler( eli, eci );
    }

    void FifoRTIn::removeHandler( EventListenerInterface *eli, EventCompleterInterface* eci )
    {
        rt_fifos[ fifo ].dataReady->removeHandler( eli, eci );
    }

    void FifoRTIn::complete(EventListenerInterface* eli)
    {
        rt_fifos[ fifo ].dataReady->complete( eli );
    }

}


