/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:57 CEST 2002  ConditionExpire.hpp

                        ConditionExpire.hpp -  description
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


#ifndef CONDITIONEXPIRE_HPP
#define CONDITIONEXPIRE_HPP

#include <corelib/ConditionInterface.hpp>
#include <corelib/HeartBeatGenerator.hpp>

namespace ORO_Execution
{

	namespace ORO_CoreLib;

    /**
     * A conditional that evaluates true until
     * a certain time is reached
     */

    class ConditionExpire
                : public ConditionInterface
    {
            typedef HeartBeatGenerator::ticks ticks;

        public:
            /**
             * Create a Condition that evaluates to false when t is reached
             * @param t the time in HeartBeatGenerator::ticks on and after which the condition
             *          will evaluate to false
             */
            ConditionExpire( ticks t ) : time( t ), hb( HeartBeatGenerator::Instance() )
            {}

            virtual ~ConditionExpire()
            {}

            virtual bool evaluate()
            {
                return ( time < hb->timeGet() );
            }

            virtual ConditionInterface* copy() const
            {
                return new ConditionExpire( time );
            }

        private:
            /**
             * The expiration time, expressed in ticks
             */
            ticks time;
            /**
             * A local reference to the HeartBeat Generator
             */
            HeartBeatGenerator *hb;
    };

}

#endif
