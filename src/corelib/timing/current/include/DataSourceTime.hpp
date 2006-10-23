/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  DataSourceTime.hpp 

                        DataSourceTime.hpp -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#include "TimeService.hpp"
#include "DataSource.hpp"

namespace RTT
{
    /**
     * A DataSource which returns the time elapsed since the last reset
     * in Seconds.
     */
    class DataSourceTime
        : public DataSource<TimeService::Seconds>
    {
        TimeService::ticks stamp;
        mutable TimeService::Seconds last;
        TimeService* ts;
    public:
        DataSourceTime()
            : stamp( 0 ), last(0), ts( TimeService::Instance() )
        {}
        
        TimeService::Seconds get() const {
            return last = ts->secondsSince(stamp);
        }

        TimeService::Seconds value() const {
            return last;
        }

        void reset() {
            // get new pointer to ts also ?
            stamp = ts->getTicks();
        }

        DataSourceTime* clone() const {
            return new DataSourceTime();
        }

        DataSourceTime* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new DataSourceTime();
        }
        
    };
}
