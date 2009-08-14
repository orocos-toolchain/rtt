/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionDuration.hpp

                        ConditionDuration.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
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
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef CONDITIONDURATION_HPP
#define CONDITIONDURATION_HPP

#include "../base/ConditionInterface.hpp"
#include "../os/TimeService.hpp"

namespace RTT
{

    /**
     * A conditional that evaluates true after a certain time has passed.
     * The behaviour can be inverted upon construction.
     */
    class RTT_API ConditionDuration
        : public ConditionInterface
    {
        typedef TimeService::nsecs nsecs;
        typedef TimeService::ticks ticks;

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
        TimeService *hb;
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
