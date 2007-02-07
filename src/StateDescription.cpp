/***************************************************************************
  tag: Peter Soetens  Tue Jul 20 17:30:55 CEST 2004  StateDescription.cxx

                        StateDescription.cxx -  description
                           -------------------
    begin                : Tue July 20 2004
    copyright            : (C) 2004 Peter Soetens
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


#include "StateDescription.hpp"

namespace RTT {
    

  StateDescription* StateDescription::postponeState()
  {
    StateDescription* ret = new StateDescription( "__pp__" + getName(), pp, entrypoint );
    ret->setEntryProgram( mentry );
    ret->setRunProgram( mrun );
    ret->setHandleProgram( mhandle );
    ret->setExitProgram( mexit );
    ret->setDefined( isDefined() );
    setDefined( false );
    return ret;
  }

  StateDescription::~StateDescription() {
  }

  StateDescription* StateDescription::copy( std::map<const DataSourceBase*, DataSourceBase*>& replacementdss ) const
  {
    StateDescription* ret = new StateDescription( name, pp, entrypoint );
    ret->inited = isDefined();
    if ( mentry )
    {
      ret->setEntryProgram( ProgramInterfacePtr(mentry->copy( replacementdss )) );
    }
    if ( mexit )
    {
      ret->setExitProgram( ProgramInterfacePtr(mexit->copy( replacementdss )) );
    }
    if ( mhandle )
    {
      ret->setHandleProgram( ProgramInterfacePtr(mhandle->copy( replacementdss) ));
    }
    if ( mrun )
    {
      ret->setRunProgram( ProgramInterfacePtr(mrun->copy( replacementdss )));
    }
    return ret;
  }

}
