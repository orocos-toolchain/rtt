/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  CommandProxy.cxx 

                        CommandProxy.cxx -  description
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
 
 

#include "rtt/corba/ExecutionI.h"
#include "rtt/corba/CommandProxy.hpp"
#include "rtt/ConditionInterface.hpp"

#include <ace/String_Base.h>

using namespace std;

namespace RTT
{namespace Corba
{
    
    

    std::map<Orocos::Command_ptr, CommandProxy*> CommandProxy::proxies;

    CommandProxy::CommandProxy( ::Orocos::Command_ptr e) 
        : mdata( ::Orocos::Command::_duplicate(e) )
    {
        try {
            mdata->reset(); // force connect to object.
        }
        catch (CORBA::Exception &e) {
            Logger::log() <<Logger::Error << "CORBA exception raised when creating CommandProxy!" << Logger::nl;
            Logger::log() << e._info().c_str() << Logger::endl;
        }
        catch (...) {
            throw;
        }
    }
    
    namespace {
        struct ValidCondition
            : public ConditionInterface
        {
            ::Orocos::Command_var mserver;

            ValidCondition( ::Orocos::Command_ptr server )
                : mserver(::Orocos::Command::_duplicate(server) )
            {}
            bool evaluate()
            {
                return mserver->done();
            }
            
            ValidCondition* clone() const
            {
                return new ValidCondition( mserver.in() );
            }

            ValidCondition* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return this->clone();
            }
        };
    }

    ConditionInterface* CommandProxy::createCondition() const {
        return new ValidCondition( mdata.in() );
    }


    CommandProxy* CommandProxy::Create(::Orocos::Command_ptr t) {
        if ( CORBA::is_nil( t ) )
            return 0;

        // proxy present for this object ?
        if ( proxies.count( t ) )
            return proxies[t];

        // create new:
        CommandProxy* ctp = new CommandProxy( t );
        proxies[t] = ctp;
        return ctp;
    }

    Orocos::Command_ptr CommandProxy::server() const
    {
        return Orocos::Command::_duplicate( mdata.in() );
    }

}}

