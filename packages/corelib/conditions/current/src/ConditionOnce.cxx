/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:24 CEST 2005  ConditionOnce.cxx 

                        ConditionOnce.cxx -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
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
 
 

#include "corelib/ConditionOnce.hpp"

namespace ORO_CoreLib
{

    ConditionOnce::ConditionOnce(bool what) : ran( false ), _what(what)
    {}

    ConditionOnce::~ConditionOnce()
    {}

    bool ConditionOnce::evaluate()
    {
        if ( ran == true )
            return !_what;
        else 
            ran = true;
        return _what;
    }

    void ConditionOnce::reset()
    {
        ran = false;
    }

    ConditionInterface* ConditionOnce::clone() const
    {
        return new ConditionOnce( _what );
    }
}

