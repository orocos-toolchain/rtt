/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:59 CEST 2006  ExpressionProxy.cxx 

                        ExpressionProxy.cxx -  description
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
 
 

#include <rtt/Types.hpp>
#include "rtt/corba/ExecutionI.h"
#include "rtt/corba/ExpressionProxy.hpp"
#include <iostream>

#include <ace/String_Base.h>

using namespace std;

namespace RTT
{namespace Corba
{
    
    

    std::map<Orocos::Expression_ptr, ExpressionProxy*> ExpressionProxy::proxies;
    std::map<Orocos::Expression_ptr, DataSourceBase*> ExpressionProxy::dproxies;

    ExpressionProxy::ExpressionProxy( ::Orocos::Expression_ptr e) 
        : mdata( ::Orocos::Expression::_duplicate(e) )
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

    ExpressionProxy* ExpressionProxy::Create(::Orocos::Expression_ptr t) {
        if ( CORBA::is_nil( t ) )
            return 0;

        // proxy present for this object ?
        if ( proxies.count( t ) )
            return proxies[t];

        // create new:
        ExpressionProxy* ctp = new ExpressionProxy( t );
        proxies[t] = ctp;
        return ctp;
    }

    DataSourceBase* ExpressionProxy::CreateDataSource(::Orocos::Expression_ptr t) {
        if ( CORBA::is_nil( t ) )
            return 0;

        // proxy present for this object ?
        if ( dproxies.count( t ) )
            return dproxies[t];

        // First, try to create according to 'typename':
        CORBA::String_var tn = t->getTypeName();
        TypeInfo* builder = TypeInfoRepository::Instance()->type( tn.in() );
        DataSourceBase* dsb;
        if ( builder )
            dsb = builder->buildCorbaProxy( t );
        else
            return Create( t ); // failed, create default proxy.
        
        dproxies[t] = dsb;
        return dsb;
    }


//     Orocos::Expression_ptr ExpressionProxy::createExpression() const
//     {
//         return Orocos::Expression::_duplicate( mdata.in() );
//     }

}}

