/***************************************************************************
 tag: Peter Soetens  Wed Apr 17 16:01:36 CEST 2002  HeartBeatGenerator.h 

                       HeartBeatGenerator.h -  description
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

namespace ORO_CoreLib
{

    /**
     * The HeartBeatGenerator is used for time keeping and conversions.
     * In a later stage, it will also keep distributed clocks
     * synchronised.
     */
    class HeartBeatGenerator
    {
        public:

        // XXX: long long is not ANSI C++
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
            static HeartBeatGenerator* Instance();

            /**
             * Releases the HeartBeatGenerator
             * Reference counting might aid in making this call safe
             *
             * @return true on success, false on failure
             */
            static bool Release();

            /**
             * Destructor
             */
            virtual ~HeartBeatGenerator();

            /**
             * Get current tick of the HeartBeat clock
             * 
             * @return current tick of the heartbeat clock
             */
            ticks ticksGet() const;

            /**
             * Get clicks passed since a certain moment
             * 
             * @return ticks passed since <relativeTime> unless <relativeTime>
             *         is zero, then sets <relativeTime> = timeGet() and returns zero
             */
            ticks ticksGet( ticks &relativeTime ) const;

            /**
             * Get clicks passed since a certain moment.
             * If <relativeTime> is zero, the absolute system time is given.
             * 
             * @return ticks passed since <relativeTime>.
             */
            ticks ticksSince( ticks relativeTime ) const;
            /**
             * Get the time in seconds passed since a certain moment
             * 
             * @realtime
             * @runtime
             * @return seconds passed since <relativeTime> unless <relativeTime>
             *         is zero, then sets <relativeTime> = timeGet() and returns zero
             */
            Seconds secondsGet( ticks &relativeTime ) const;

            /**
             * Get Seconds passed since a certain moment.
             * If <relativeTime> is zero, the absolute syteme time is given.
             * 
             * @return Seconds passed since <relativeTime>.
             */
            Seconds secondsSince( ticks relativeTime ) const;

            /**
             * Convert an amount of nano seconds to HeartBeat ticks
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
            HeartBeatGenerator();

        private:

            /**
             * Our only instance of the HeartBeatGenerator
             */
            static HeartBeatGenerator* _instance;

    };
} // namespace ORO_CoreLib

#endif
