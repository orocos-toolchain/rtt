/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:36 CEST 2002  TimeService.h 

                       TimeService.h -  description
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



#ifndef HEART_BEAT_GENERATOR_HPP
#define HEART_BEAT_GENERATOR_HPP

#include "Time.hpp"

namespace RTT
{

    /**
     * The TimeService is used for system-wide time keeping and conversions.
     * @ingroup CoreLibTime
     */
    class TimeService
    {
    public:
        /**
         * The type used to store SI unit time keeping
         */
        typedef double Seconds;

        /**
         * An integer representation of seconds
         */
        typedef long secs;

        /**
         * An integer representation of miliseconds
         */
        typedef long msecs;

        /**
         * An integer representation of microseconds
         */
        typedef long usecs;

        /**
         * An integer representation of nanoseconds
         */
        typedef long long nsecs;

        /**
         * The type for the systems clock tick.
         */
        typedef long long ticks;

    public:
        static TimeService* Instance();

        /**
         * Releases the TimeService
         * Reference counting might aid in making this call safe
         *
         * @return true on success, false on failure
         */
        static bool Release();

        /**
         * Destructor
         */
        virtual ~TimeService();

        /**
         * Get current tick of the System clock
         * 
         * @return current tick of the system clock
         * @deprecated by getTicks()
         */
        ticks ticksGet() const { return getTicks(); }

        /**
         * Get current tick of the System clock
         * 
         * @return current tick of the system clock
         */
        ticks getTicks() const;

        /**
         * Get clicks passed since a certain moment
         * 
         * @return ticks passed since \a relativeTime unless \a relativeTime
         *         is zero, then sets \a relativeTime = timeGet() and returns zero
         * @deprecated by getTicks()
         */
        ticks ticksGet( ticks &relativeTime ) const { return getTicks( relativeTime ); }

        /**
         * Get clicks passed since a certain moment
         * 
         * @return ticks passed since \a relativeTime unless \a relativeTime
         *         is zero, then sets \a relativeTime = timeGet() and returns zero
         */
        ticks getTicks( ticks &relativeTime ) const;

        /**
         * Get clicks passed since a certain moment.
         * If \a relativeTime is zero, the absolute system time is given.
         * 
         * @return ticks passed since \a relativeTime.
         */
        ticks ticksSince( ticks relativeTime ) const;

        /**
         * Get the time in seconds passed since a certain moment
         * 
         * @realtime
         * @runtime
         * @return seconds passed since \a relativeTime unless \a relativeTime
         *         is zero, then sets \a relativeTime = timeGet() and returns zero
         * @deprecated by getSeconds()
         */
        Seconds secondsGet( ticks &relativeTime ) const { return getSeconds(relativeTime); }

        /**
         * Get the time in seconds passed since a certain moment
         * 
         * @realtime
         * @runtime
         * @return seconds passed since \a relativeTime unless \a relativeTime
         *         is zero, then sets \a relativeTime = timeGet() and returns zero
         */
        Seconds getSeconds( ticks &relativeTime ) const;

        /**
         * Get Seconds passed since a certain moment.
         * If \a relativeTime is zero, the absolute syteme time is given.
         * 
         * @return Seconds passed since \a relativeTime.
         */
        Seconds secondsSince( ticks relativeTime ) const;

        /**
         * Change the time with \a delta seconds.
         * @return the new time.
         */
        Seconds secondsChange( Seconds delta );

        /**
         * Enables or disables reading the system clock.
         * If disabled, you'll have to change time using
         * \a secondsChange.
         */
        void enableSystemClock( bool yes_no );
	
        /**
         * Get current nsecs of the System clock
         * 
         * @return current nsecs of the system clock
         */
        nsecs getNSecs() const;

        /**
         * Get nsecs passed since a certain moment
         * 
         * @return nsecs passed since \a relativeTime unless \a relativeTime
         *         is zero, then sets \a relativeTime = timeGet() and returns zero
         */
        nsecs getNSecs( nsecs &relativeTime ) const;

        /**
         * Convert an amount of nano seconds to System ticks
         * @param m The amount of nano seconds
         * @return  The equivalent in ticks
         */
        static ticks nsecs2ticks( const nsecs m );

        /**
         * Convert an amount of ticks to nano seconds
         */
        static nsecs ticks2nsecs( const ticks t );

    protected:

        /**
         * Constructor
         */
        TimeService();

    private:

        /**
         * Our only instance of the TimeService
         */
        static TimeService* _instance;

        /**
         * System wide offset of time.
         */
        ticks offset;

        bool use_clock;
    };
} // namespace RTT

#endif
