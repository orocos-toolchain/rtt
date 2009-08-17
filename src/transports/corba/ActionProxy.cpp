/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  ActionProxy.cxx

                        ActionProxy.cxx -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be

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



#include "OperationsI.h"
#include "ActionProxy.hpp"

#include "corba.h"
#ifdef CORBA_IS_TAO
#include <ace/String_Base.h>
#endif

using namespace std;

namespace RTT
{namespace corba
{
    std::map<corba::Action_ptr, ActionProxy*> ActionProxy::proxies;

    ActionProxy::ActionProxy( ::RTT::corba::Action_ptr e)
        : mdata( ::RTT::corba::Action::_duplicate(e) )
    {
        try {
            mdata->reset(); // force connect to object.
        }
        catch (CORBA::Exception &e) {
            Logger::log() <<Logger::Error << "CORBA exception raised when creating ActionProxy!" << Logger::nl;
            Logger::log() << CORBA_EXCEPTION_INFO(e) << Logger::endl;
        }
        catch (...) {
            throw;
        }
    }

    ActionProxy* ActionProxy::Create(::RTT::corba::Action_ptr t) {
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

    corba::Action_ptr ActionProxy::server() const
    {
        return corba::Action::_duplicate( mdata.in() );
    }

}}

