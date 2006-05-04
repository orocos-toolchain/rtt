
#include "corba/ExecutionI.h"
#include "corba/CommandProxy.hpp"
#include "corelib/ConditionInterface.hpp"

#include <ace/String_Base.h>

using namespace std;

namespace ORO_Corba
{
    using namespace ORO_CoreLib;
    using namespace ORO_Execution;

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
                return mserver->valid();
            }
            
            ValidCondition* clone() const
            {
                return new ValidCondition( mserver.in() );
            }

            ValidCondition* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const {
                return this->clone();
            }
        };
    }

    ConditionInterface* CommandProxy::createValidCondition() const {
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

}

