/***************************************************************************
 tag: Peter Soetens  Mon Jun 10 14:33:16 CEST 2002  EventBottomHalfInterface.hpp 

                       EventBottomHalfInterface.hpp -  description
                          -------------------
   begin                : Mon June 10 2002
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


#ifndef EVENT_BOTTOM_HALF_INTERFACE
#define EVENT_BOTTOM_HALF_INTERFACE

namespace ORO_OS
{

    /**
     * This class provides functionality for the Deferred Service Routine
     * (DSR) which normally follows the ISR
     * @see ISRInterface.hpp
     */
    class DSRInterface
    {

        public:
            virtual ~DSRInterface()
            {}

            /**
             * Complete an Event caused by an interrupt
             */
            virtual void completeInterrupt() = 0;

    };
}

#endif
