/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:31 CEST 2002  TimeService.cpp

                       TimeService.cpp -  description
                          -------------------
   begin                : Wed April 17 2002
   copyright            : (C) 2002 Peter Soetens
   email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#include "rtt/os/fosi.h"
#include "rtt/os/TimeService.hpp"

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

    bool TimeService::systemClockEnabled() const
    {
      return use_clock;
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

    TimeService::ticks
    TimeService::ticksChange( TimeService::ticks delta )
    {
        offset += delta;
        return ticksSince( 0 );
    }

    TimeService::nsecs
    TimeService::getNSecs() const
    {
      return rtos_get_time_ns();
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
