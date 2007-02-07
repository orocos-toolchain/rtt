/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  CommandProxy.cxx 

                        CommandProxy.cxx -  description
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
#include "CommandProxy.hpp"
#include "ConditionInterface.hpp"

#include <ace/String_Base.h>

using namespace std;

namespace RTT
{namespace Corba
{

    CommandProxy::CommandProxy( ::RTT::Corba::Command_ptr e) 
        : mdata( ::RTT::Corba::Command::_duplicate(e) )
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

    CommandProxy::~CommandProxy()
    {
    }
    
    namespace {
        struct ValidCondition
            : public ConditionInterface
        {
            ::RTT::Corba::Command_var mserver;

            ValidCondition( ::RTT::Corba::Command_ptr server )
                : mserver(::RTT::Corba::Command::_duplicate(server) )
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


    CommandProxy* CommandProxy::Create(::RTT::Corba::Command_ptr t) {
        if ( CORBA::is_nil( t ) )
            return 0;

        // create new:
        CommandProxy* ctp = new CommandProxy( t );
        return ctp;
    }

    Corba::Command_ptr CommandProxy::server() const
    {
        return Corba::Command::_duplicate( mdata.in() );
    }

}}

