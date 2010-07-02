/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  ConditionDSDuration.hpp

                        ConditionDSDuration.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be

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


#ifndef CONDITION_DS_DURATION_HPP
#define CONDITION_DS_DURATION_HPP

#include "../scripting/rtt-scripting-config.h"
#include "../base/ConditionInterface.hpp"
#include "../os/TimeService.hpp"
#include "../internal/DataSource.hpp"

namespace RTT
{ namespace scripting {


    /**
     * A conditional that evaluates true after a certain time (in seconds) has passed,
     * given by a DataSource.
     * The behaviour can be inverted upon construction.
     */
    class RTT_SCRIPTING_API ConditionDSDuration
        : public base::ConditionInterface
    {
        typedef os::TimeService::nsecs nsecs;
        typedef os::TimeService::ticks ticks;

    public:
        /**
         * Create a condition that changes after \a ds->get() seconds.
         * @param ds A internal::DataSource containing the period of time starting from \a reset()
         * @param invert If \a false (default) then the condition will
         * raise to true after \a ds->get() seconds. If \a true then the condition will
         * fall to false after \a ds->get() seconds.
         */
        ConditionDSDuration( internal::DataSource<double>::shared_ptr ds, bool invert = false );

        virtual ~ConditionDSDuration();

        virtual bool evaluate();

        virtual void reset();

        base::ConditionInterface* clone() const;

        virtual base::ConditionInterface* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const;
    private:

        /**
         * The expiration time, expressed in ticks
         */
        internal::DataSource<double>::shared_ptr time;

        /**
         * A local reference to the HeartBeat Generator
         */
        os::TimeService *hb;
        /**
         * The time the evaluation is called the first time
         */
        ticks mark;

        /**
         * Invert the time comparison to 'smaller then'
         */
        bool _invert;
    };

}}

#endif
