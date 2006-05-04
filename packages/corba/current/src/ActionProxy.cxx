
#include "corba/ExecutionI.h"
#include "corba/ActionProxy.hpp"

#include <ace/String_Base.h>

using namespace std;

namespace ORO_Corba
{
    using namespace ORO_CoreLib;
    using namespace ORO_Execution;

    std::map<Orocos::Action_ptr, ActionProxy*> ActionProxy::proxies;

    ActionProxy::ActionProxy( ::Orocos::Action_ptr e) 
        : mdata( ::Orocos::Action::_duplicate(e) )
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

    ActionProxy* ActionProxy::Create(::Orocos::Action_ptr t) {
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

    Orocos::Action_ptr ActionProxy::server() const
    {
        return Orocos::Action::_duplicate( mdata.in() );
    }

}

