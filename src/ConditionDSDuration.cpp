/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  ConditionDSDuration.cxx 

                        ConditionDSDuration.cxx -  description
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
 
 

#include "ConditionDSDuration.hpp"

namespace RTT
{

    ConditionDSDuration::ConditionDSDuration( DataSource<double>::shared_ptr ds, bool invert )
        : time( ds ), hb( TimeService::Instance() ),
          mark( 0 ), _invert( invert )
    {}

    ConditionDSDuration::~ConditionDSDuration()
    {}

    bool ConditionDSDuration::evaluate()
    {
        return _invert != ( time->get() > hb->secondsSince( mark ) );
    }

    void ConditionDSDuration::reset()
    {
        mark = hb->getTicks();
    }

    ConditionInterface* ConditionDSDuration::clone() const
    {
        return new ConditionDSDuration( time, _invert );
    }

    ConditionInterface* ConditionDSDuration::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const
    {
        return new ConditionDSDuration( time->copy(alreadyCloned), _invert );
    }
}
