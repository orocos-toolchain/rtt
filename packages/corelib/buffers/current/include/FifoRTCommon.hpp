/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  FifoRTCommon.hpp 

                        FifoRTCommon.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 

#ifndef FIFORTCOMMON_HPP
#define FIFORTCOMMON_HPP

#include "Event.hpp"

namespace ORO_CoreLib
{

    /**
     * Data structure used for signaling when a fifo has new data.
     * @see FifoRTIn
     * @see FifoRTOut
     */
    typedef struct
    {
        Event* dataReady;
        bool inUse;
    } fifo_info_t;

    const int MAX_NR_FIFOS = 64;

    /**
     * Returns the number of a free Fifo
     *
     * @return the number of a free Fifo
     * @return -1 when there are no free Fifo's left
     */
    int getFreeFifoNr();

}

#endif

