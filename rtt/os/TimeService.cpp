/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:31 CEST 2002  TimeService.cpp

                       TimeService.cpp -  description
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

#include "os/fosi.h"
#include "TimeService.hpp"

namespace RTT {
    using namespace os;

    TimeService* TimeService::_instance = 0;

    const TimeService::ticks TimeService::InfiniteTicks = ::InfiniteTicks;
    const TimeService::nsecs TimeService::InfiniteNSecs = ::InfiniteNSecs;
    const TimeService::Seconds TimeService::InfiniteSeconds  = ::InfiniteSeconds;

    TimeService::ticks TimeService::nsecs2ticks( const nsecs m )
    {
        return nano2ticks( m );
    }

    TimeService::nsecs TimeService::ticks2nsecs( const ticks t )
    {
        return ticks2nano( t );
    }

    TimeService* TimeService::Instance()
    {
        if ( _instance == 0 )
        {
            _instance = new TimeService();
        }

        return _instance;
    }


    bool TimeService::Release()
    {
        if ( _instance != 0 )
        {
            delete _instance;
            _instance = 0;
            return true;
        }

        return false;
    }


    TimeService::TimeService()
        : offset(0), use_clock(true)
    {
        //os::cout << "HeartBeat Created\n";
    }

    TimeService::~TimeService()
    {
        //os::cout << "HB DESTRUCTOR\n";
    }

    void TimeService::enableSystemClock( bool yes_no )
    {
        // guarantee monotonous time increase :
        if ( yes_no == use_clock )
            return;
        // we switched from one time domain to the other
        use_clock=yes_no;
        if ( use_clock == true )
            {
                // if offset is X, then start counting from X.
                offset = offset - rtos_get_time_ticks();
            }
        else
            {
                // start counting from _now_ + old offset
                offset = offset + rtos_get_time_ticks();
            }
    }


    TimeService::ticks
    TimeService::getTicks() const
    {
        return use_clock ? rtos_get_time_ticks() + offset : 0 + offset;
    }

    TimeService::ticks
    TimeService::getTicks( TimeService::ticks &relativeTime ) const
    {
        if ( relativeTime == 0 )
        {
            relativeTime = getTicks();
            return 0;
        }
        return ( getTicks() - relativeTime );
    }

    TimeService::ticks
    TimeService::ticksSince( TimeService::ticks relativeTime ) const
    {
        return ( getTicks() - relativeTime );
    }


    TimeService::Seconds
    TimeService::getSeconds( TimeService::ticks &relativeTime ) const
    {
        return nsecs_to_Seconds( ticks2nsecs( getTicks( relativeTime ) ) ) ;
    }

    TimeService::Seconds
    TimeService::secondsSince( TimeService::ticks relativeTime ) const
    {
        return nsecs_to_Seconds( ticks2nsecs( ticksSince( relativeTime ) ) ) ;
    }

    TimeService::Seconds
    TimeService::secondsChange( TimeService::Seconds delta )
    {
        offset += nsecs2ticks( Seconds_to_nsecs( delta ) );
        return nsecs_to_Seconds( ticks2nsecs( ticksSince( 0 ) ) ) ;
    }

    TimeService::nsecs
    TimeService::getNSecs() const
    {
        return ticks2nsecs(offset) + (use_clock) ? (rtos_get_time_ns()) : (0); 
    }

    TimeService::nsecs
    TimeService::getNSecs( TimeService::nsecs &relativeTime ) const
    {
        if ( relativeTime == 0 )
        {
            relativeTime = getNSecs();
            return 0;
        }
        return ( getNSecs() - relativeTime );
    }
}
