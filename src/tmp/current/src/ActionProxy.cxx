/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  ActionProxy.cxx 

                        ActionProxy.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 

#include "rtt/corba/OperationsI.h"
#include "rtt/corba/ActionProxy.hpp"

#include <ace/String_Base.h>

using namespace std;

namespace RTT
{namespace Corba
{
    std::map<Corba::Action_ptr, ActionProxy*> ActionProxy::proxies;

    ActionProxy::ActionProxy( ::RTT::Corba::Action_ptr e) 
        : mdata( ::RTT::Corba::Action::_duplicate(e) )
    {
        try {
            mdata->reset(); // force connect to object.
        }
        catch (CORBA::Exception &e) {
            Logger::log() <<Logger::Error << "CORBA exception raised when creating ActionProxy!" << Logger::nl;
            Logger::log() << e._info().c_str() << Logger::endl;
        }
        catch (...) {
            throw;
        }
    }

    ActionProxy* ActionProxy::Create(::RTT::Corba::Action_ptr t) {
        if ( CORBA::is_nil( t ) )
            return 0;

        // proxy present for this object ?
        if ( proxies.count( t ) )
            return proxies[t];

        // create new:
        ActionProxy* ctp = new ActionProxy( t );
        proxies[t] = ctp;
        return ctp;
    }

    Corba::Action_ptr ActionProxy::server() const
    {
        return Corba::Action::_duplicate( mdata.in() );
    }

}}

