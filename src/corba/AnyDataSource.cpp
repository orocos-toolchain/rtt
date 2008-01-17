#include "AnyDataSource.hpp"
#include "CorbaLib.hpp"

namespace RTT
{
    namespace Corba {
    
    AnyDataSource::~AnyDataSource() {}

    
    AnyDataSource::AnyDataSource( CORBA::Any_ptr value )
        : mdata( value )
    {
    }
    
    DataSource<CORBA::Any_var>::result_t AnyDataSource::get() const
    {
        return new CORBA::Any( mdata.in() );
    }

    
    DataSource<CORBA::Any_var>::result_t AnyDataSource::value() const
    {
        return new CORBA::Any(mdata.in());
    }

    
    AnyDataSource* AnyDataSource::clone() const
    {
        return new AnyDataSource( new CORBA::Any(mdata.in()));
    }

    
    AnyDataSource* AnyDataSource::copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
        // no copy needed, share this with all instances.
        return const_cast<AnyDataSource*>(this);
    }

    int AnyDataSource::serverProtocol() const {
        return ORO_CORBA_PROTOCOL_ID;
    }
    }

}


