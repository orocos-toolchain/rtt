
#include "corba/ExecutionI.h"
#include "corba/ExpressionProxy.hpp"
#include <iostream>

#include <ace/String_Base.h>

using namespace std;

namespace ORO_Corba
{
    using namespace ORO_CoreLib;
    using namespace ORO_Execution;

    std::map<Orocos::Expression_ptr, ExpressionProxy*> ExpressionProxy::proxies;

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


//     Orocos::Expression_ptr ExpressionProxy::createExpression() const
//     {
//         return Orocos::Expression::_duplicate( mdata.in() );
//     }

}

