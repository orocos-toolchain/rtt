/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionDuration.hpp

                        ConditionDuration.hpp -  description
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



#ifndef CONDITIONDURATION_HPP
#define CONDITIONDURATION_HPP

#include "ConditionInterface.hpp"
#include "HeartBeatGenerator.hpp"

#include <boost/lexical_cast.hpp>
#include <cassert>

namespace ORO_CoreLib
{

    /**
     * A conditional that evaluates true between being reset and until
     * a certain time has passed..
     */
    class ConditionDuration
                : public ConditionInterface
    {
            typedef HeartBeatGenerator::nsecs nsecs;
            typedef HeartBeatGenerator::ticks ticks;

        public:
            /**
             * Create a condition that expires after t nano seconds
             * @param t the period of time starting from the first evaluation
             *          for which this Condition evaluates true
             */
            ConditionDuration( nsecs t )
                    : time( t ), hb( HeartBeatGenerator::Instance() ),
                    mark( 0 )
            {}

            virtual ~ConditionDuration()
            {}

            /**
             * This method returns true if the time given at the
             * construction of the object has expired.
             * @return true if time has expired
             */
            virtual bool evaluate()
            {
                assert( mark );

                return ( time < HeartBeatGenerator::ticks2nsecs( hb->ticksGet( mark ) ) );
            };

            virtual void reset()
            {
                mark = hb->ticksGet();
            };

            ConditionInterface* clone() const
            {
                return new ConditionDuration( time );
            }

        private:

            /**
             * The expiration time, expressed in ticks
             */
            nsecs time;
            /**
             * A local reference to the HeartBeat Generator
             */
            HeartBeatGenerator *hb;
            /**
             * The time the evaluation is called the first time
             */
            ticks mark;
    };

}

#endif
