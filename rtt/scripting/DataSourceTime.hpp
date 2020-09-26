/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  DataSourceTime.hpp

                        DataSourceTime.hpp -  description
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


#include "rtt/os/TimeService.hpp"
#include "rtt/internal/DataSource.hpp"
#include "rtt/scripting/rtt-scripting-config.h"

namespace RTT
{ namespace scripting {

    /**
     * A internal::DataSource which returns the time elapsed since the last reset
     * in Seconds.
     */
    class RTT_SCRIPTING_API DataSourceTime
        : public internal::DataSource<os::TimeService::Seconds>
    {
        os::TimeService::ticks stamp;
        mutable os::TimeService::Seconds last;
        os::TimeService* ts;
    public:
        DataSourceTime()
            : stamp( 0 ), last(0), ts( os::TimeService::Instance() )
        {}

        os::TimeService::Seconds get() const {
            return last = ts->secondsSince(stamp);
        }

        os::TimeService::Seconds value() const {
            return last;
        }

        os::TimeService::Seconds const& rvalue() const {
            return last;
        }

        void reset() {
            // get new pointer to ts also ?
            stamp = ts->getTicks();
        }

        DataSourceTime* clone() const {
            return new DataSourceTime();
        }

        DataSourceTime* copy( std::map<const base::DataSourceBase*, base::DataSourceBase*>& alreadyCloned ) const {
            return new DataSourceTime();
        }

    };
}}
