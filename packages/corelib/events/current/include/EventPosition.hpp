/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 14:46:35 CEST 2002  EventPosition.h 

                       EventPosition.h -  description
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


#ifndef EVENT_POSITION_HPP
#define EVENT_POSITION_HPP

#include "EventPositionInterface.hpp"

namespace ORO_CoreLib
{

    class EventPosition : public EventPositionInterface
    {

        public:
            EventPosition( int pos, int i, int t = 0 )
                    : checkPos( pos ), tol( i ), count( 0 ), waitTime( t )
            {
            }

            bool conditionMet( int pos )
            {
                if ( pos <= checkPos + tol && pos >= checkPos - tol )
                {
                    if ( count == waitTime )
                    {
                        count = 0;
                        return true;
                    }

                    count++ ;
                }

                return false;
            }

            void setPos( int p )
            {
                checkPos = p;
            }

            void setTolerance( int t )
            {
                tol = t;
            }

            void setWaitTime( int t )
            {
                waitTime = t;
            }

        protected:
            EventPosition()
            {}

            int checkPos;

            int tol;

            int count;

            int waitTime;
    };
}

#endif
