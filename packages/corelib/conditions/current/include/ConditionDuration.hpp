/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionDuration.hpp

                        ConditionDuration.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef CONDITIONDURATION_HPP
#define CONDITIONDURATION_HPP

#include "ConditionInterface.hpp"
#include "HeartBeatGenerator.hpp"

namespace ORO_CoreLib
{

    /**
     * A conditional that evaluates true after a certain time has passed.
     * The behaviour can be inverted upon construction.
     */
    class ConditionDuration
        : public ConditionInterface
    {
        typedef HeartBeatGenerator::nsecs nsecs;
        typedef HeartBeatGenerator::ticks ticks;
        
    public:
        /**
         * Create a condition that changes after t nano seconds
         * @param t the period of time starting from \a reset()
         * @param invert If \a false (default) then the condition will
         * raise to true after \a t. If \a true then the condition will
         * fall to false after \a t.
         */
        ConditionDuration( nsecs t, bool invert = false );

        virtual ~ConditionDuration();

        virtual bool evaluate();

        virtual void reset();

        ConditionInterface* clone() const;

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

        /**
         * Invert the time comparison to 'smaller then'
         */
        bool _invert;
    };

}

#endif
