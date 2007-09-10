/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  ExpressionProxy.cxx 

                        ExpressionProxy.cxx -  description
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
 
 

#include <Types.hpp>
#include "OperationsI.h"
#include "ExpressionProxy.hpp"
#include <iostream>

#include <ace/String_Base.h>

using namespace std;

namespace RTT
{namespace Corba
{

    ExpressionProxy::EMap ExpressionProxy::proxies;
    ExpressionProxy::DMap ExpressionProxy::dproxies;

    ExpressionProxy::ExpressionProxy( ::RTT::Corba::Expression_ptr e) 
        : mdata( ::RTT::Corba::Expression::_duplicate(e) )
    {
        try {
            CORBA::String_var nm = mdata->getType(); // force connect to object.
        }
        catch (CORBA::Exception &e) {
            Logger::log() <<Logger::Error << "CORBA exception raised when creating ExpressionProxy!" << Logger::nl;
            Logger::log() << e._info().c_str() << Logger::endl;
        }
        catch (...) {
            throw;
        }
    }

    ExpressionProxy::shared_ptr ExpressionProxy::Create(::RTT::Corba::Expression_ptr t) {
        if ( CORBA::is_nil( t ) )
            return 0;

        // proxy present for this object ?
        for (EMap::iterator it = proxies.begin(); it != proxies.end(); ++it)
            if ( (it->first)->_is_equivalent( t ) )
                return proxies[t];

        // create new:
        ExpressionProxy* ctp = new ExpressionProxy( t );
        proxies[t] = ctp;
        return ctp;
    }

    DataSourceBase::shared_ptr ExpressionProxy::CreateDataSource(::RTT::Corba::Expression_ptr t) {
        if ( CORBA::is_nil( t ) )
            return 0;

        // proxy present for this object ?
        for (DMap::iterator it = dproxies.begin(); it != dproxies.end(); ++it)
            if ( (it->first)->_is_equivalent( t ) )
                return dproxies[t];

        // First, try to create according to 'typename':
        CORBA::String_var tn = t->getTypeName();
        TypeInfo* builder = TypeInfoRepository::Instance()->type( tn.in() );
        DataSourceBase* dsb;
        if ( builder && builder->getProtocol(ORO_CORBA_PROTOCOL_ID) )
            dsb = builder->getProtocol(ORO_CORBA_PROTOCOL_ID)->proxy( t );
        else
            return Create( t ); // failed, create default proxy.
        
        dproxies[t] = dsb;
        return dsb;
    }


//     Corba::Expression_ptr ExpressionProxy::createExpression() const
//     {
//         return Corba::Expression::_duplicate( mdata.in() );
//     }

}}

