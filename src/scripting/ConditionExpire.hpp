/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionExpire.hpp

                        ConditionExpire.hpp -  description
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


#ifndef CONDITIONEXPIRE_HPP
#define CONDITIONEXPIRE_HPP

#include "../base/ConditionInterface.hpp"
#include "../os/TimeService.hpp"

namespace RTT
{
    /**
     * A conditional that evaluates \a true until
     * a certain time has elapsed since construction
     * or the last \a reset()
     */

    class RTT_API ConditionExpire
        : public ConditionInterface
    {
        typedef TimeService::ticks ticks;

    public:
        /**
         * Create a Condition that evaluates to false when t is reached
         * @param t the time in TimeService::ticks on and after which the condition
         *          will evaluate to false
         */
        ConditionExpire( ticks t )
            : time( t ), hb( TimeService::Instance() )
        {
            timestamp = hb->getTicks();
        }

        virtual ~ConditionExpire()
        {}

        virtual bool evaluate()
        {
            return ( time > hb->ticksSince(timestamp) );
        }

        virtual void reset()
        {
            timestamp = hb->getTicks();
        }

        virtual ConditionInterface* clone() const
        {
            return new ConditionExpire( time );
        }

    private:
        /**
         * The expiration time, expressed in ticks
         */
        ticks time;
        ticks timestamp;
        /**
         * A local reference to the HeartBeat Generator
         */
        TimeService *hb;
    };

}

#endif
