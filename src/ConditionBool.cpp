/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  ConditionBool.cxx

                        ConditionBool.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
#include "rtt/ConditionBool.hpp"

namespace RTT
{

    ConditionBool::ConditionBool() :
            state( false )
    {
    }

    ConditionBool::ConditionBool( bool value ) :
            state( value )
    {
    }

    ConditionBool::~ConditionBool()
    {
    }

    bool ConditionBool::evaluate()
    {
        return state;
    }

    void ConditionBool::setTrue()
    {
        state = true;
    }

    void ConditionBool::setFalse()
    {
        state = false;
    }

    ConditionBool* ConditionBool::clone() const
    {
        return new ConditionBool( state );
    }
}
