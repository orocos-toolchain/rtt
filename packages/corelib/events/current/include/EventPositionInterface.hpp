/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 14:46:38 CEST 2002  EventPositionInterface.h 

                       EventPositionInterface.h -  description
                          -------------------
   begin                : Wed April 17 2002
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


#ifndef EVENT_POSITION_INTERFACE_H
#define EVENT_POSITION_INTERFACE_H

#include "Event.hpp"

namespace ORO_CoreLib
{

    /**
     * This interface describes an event occuring when a certain
     * position is reached ("met") within a certain tolerance
     */

    class EventPositionInterface : public Event
    {

        public:
            virtual ~EventPositionInterface()
            {}

            /**
             * Returns true when the position is met within tolerance
             */
            virtual bool conditionMet( int pos ) = 0;

            /**
             * Set the reference position
             * @param p the position where the event must fire
             */
            virtual void setPos( int p ) = 0;

            /**
             * set the tolerance on the position
             * @param t  the tolerance on the reference position
             */
            virtual void setTolerance( int t ) = 0;
    };
}

#endif
