/***************************************************************************
  tag: Peter Soetens  Tue Dec 21 22:43:07 CET 2004  ConditionDuration.cxx 

                        ConditionDuration.cxx -  description
                           -------------------
    begin                : Tue December 21 2004
    copyright            : (C) 2004 Peter Soetens
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
 
 

#include "corelib/ConditionDuration.hpp"

namespace ORO_CoreLib
{

    ConditionDuration::ConditionDuration( nsecs t, bool invert )
        : time( t ), hb( TimeService::Instance() ),
          mark( 0 ), _invert( invert )
    {}

    ConditionDuration::~ConditionDuration()
    {}

    bool ConditionDuration::evaluate()
    {
        return _invert != ( time < TimeService::ticks2nsecs( hb->ticksSince( mark ) ) );
    }

    void ConditionDuration::reset()
    {
        mark = hb->getTicks();
    }

    ConditionInterface* ConditionDuration::clone() const
    {
        return new ConditionDuration( time, _invert );
    }
}
