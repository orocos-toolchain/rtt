/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:31 CEST 2002  HeartBeatGenerator.cpp 

                       HeartBeatGenerator.cpp -  description
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

/**
 * TODO : use smart pointers for the heartbeat or a noncopyable pointer
 *        and count Instance()/Release() references
 *
 */

#include "os/fosi.h"
#include "corelib/HeartBeatGenerator.hpp"

namespace ORO_CoreLib
{
    HeartBeatGenerator* HeartBeatGenerator::_instance = 0;

    HeartBeatGenerator::ticks HeartBeatGenerator::nsecs2ticks( const nsecs m )
    {
        return nano2count( m );
    }

    HeartBeatGenerator::nsecs HeartBeatGenerator::ticks2nsecs( const ticks t )
    {
        return count2nano( t );
    }

    HeartBeatGenerator* HeartBeatGenerator::Instance()
    {
        if ( _instance == 0 )
        {
            _instance = new HeartBeatGenerator();
        }

        return _instance;
    }


    bool HeartBeatGenerator::Release()
    {
        if ( _instance != 0 )
        {
            delete _instance;
            _instance = 0;
            return true;
        }

        return false;
    }


    HeartBeatGenerator::HeartBeatGenerator()
        : offset(0), use_clock(true)
    {
        //rt_std::cout << "HeartBeat Created\n";
    }

    HeartBeatGenerator::~HeartBeatGenerator()
    {
        //rt_std::cout << "HB DESTRUCTOR\n";
    }

    void HeartBeatGenerator::enableSystemClock( bool yes_no )
    {
        use_clock=yes_no;
    }


    HeartBeatGenerator::ticks
    HeartBeatGenerator::ticksGet() const
    {
        return use_clock ? systemTimeGet() : 0 + offset;
    }

    HeartBeatGenerator::ticks
    HeartBeatGenerator::ticksGet( HeartBeatGenerator::ticks &relativeTime ) const
    {
        if ( relativeTime == 0 )
        {
            relativeTime = ticksGet();
            return 0;
        }
        return ( ticksGet() - relativeTime );
    }

    HeartBeatGenerator::ticks
    HeartBeatGenerator::ticksSince( HeartBeatGenerator::ticks relativeTime ) const
    {
        return ( ticksGet() - relativeTime );
    }


    HeartBeatGenerator::Seconds
    HeartBeatGenerator::secondsGet( HeartBeatGenerator::ticks &relativeTime ) const
    {
        return Seconds( ticks2nsecs( ticksGet( relativeTime ) ) / 1000000000.0 ) ;
    }

    HeartBeatGenerator::Seconds
    HeartBeatGenerator::secondsSince( HeartBeatGenerator::ticks relativeTime ) const
    {
        return Seconds( ticks2nsecs( ticksSince( relativeTime ) ) / 1000000000.0 ) ;
    }

    HeartBeatGenerator::Seconds
    HeartBeatGenerator::secondsChange( HeartBeatGenerator::Seconds delta )
    {
        offset += nsecs2ticks( nsecs(delta * 1000000000.0) );
        return Seconds( ticks2nsecs( ticksSince( 0 ) ) / 1000000000.0 ) ;
    }


}
