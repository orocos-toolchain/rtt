#include "CorbaMethodFactory.hpp"

using namespace RTT;
using namespace RTT::detail;

CorbaMethodFactory::CorbaMethodFactory( const std::string& method_name, corba::CServiceProvider_ptr fact, PortableServer::POA_ptr the_poa )
    : RTT::interface::OperationRepositoryPart(),
      mfact(corba::CServiceProvider::_duplicate(fact) ),
      mpoa(PortableServer::POA::_duplicate(the_poa)),
      method(method_name)
{}

CorbaMethodFactory::~CorbaMethodFactory() {}

unsigned int CorbaMethodFactory::arity()  const {
    return this->getArgumentList().size();
}

unsigned int CorbaMethodFactory::collectArity()  const {
    assert(false);
    return 0;
}

std::string CorbaMethodFactory::resultType() const {
    try {
        CORBA::String_var result = mfact->getResultType( method.c_str() );
        return std::string( result.in() );
    } catch ( corba::CNoSuchNameException& nsn ) {
        throw interface::name_not_found_exception( nsn.name.in() );
    }
    return std::string();
}

std::string CorbaMethodFactory::description() const {
    try {
        CORBA::String_var result = mfact->getDescription( method.c_str() );
        return std::string( result.in() );
    } catch ( corba::CNoSuchNameException& nsn ) {
        throw interface::name_not_found_exception( nsn.name.in() );
    }
    return std::string();
}

std::vector< interface::ArgumentDescription > CorbaMethodFactory::getArgumentList() const {
    CDescriptions ret;
    try {
        corba::CDescriptions_var result = mfact->getArguments( method.c_str() );
        ret.reserve( result->length() );
        for (size_t i=0; i!= result->length(); ++i)
            ret.push_back( interface::ArgumentDescription(std::string( result[i].name.in() ),
                                                          std::string( result[i].description.in() ),
                                                          std::string( result[i].type.in() ) ));
    } catch ( corba::CNoSuchNameException& nsn ) {
        throw interface:: name_not_found_exception( nsn.name.in() );
    }
    return ret;
}

base::DataSourceBase::shared_ptr CorbaMethodFactory::produce(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const {
    corba::CAnyArguments_var nargs = new corba::CAnyArguments();
    nargs->length( args.size() );
    for (size_t i=0; i < args.size(); ++i ) {
        const types::TypeInfo* ti = args[i]->getTypeInfo();
        CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
        assert( ctt );
        CORBA::Any_var any = ctt->createAny( args[i] );
        nargs[i] = *any;
    }
    try {
        assert(false);
        //corba::CExpression_var result = mfact->createMethod( method.c_str(), nargs.in() );
        //return ExpressionProxy::CreateDataSource( result._retn() ).get();
    } catch ( corba::CNoSuchNameException& nsn ) {
        throw interface:: name_not_found_exception( nsn.name.in() );
    } catch ( corba::CWrongNumbArgException& wa ) {
        throw interface:: wrong_number_of_args_exception( wa.wanted, wa.received );
    } catch ( corba::CWrongTypeArgException& wta ) {
        throw interface:: wrong_types_of_args_exception( wta.whicharg, wta.expected.in(), wta.received.in() );
    }
    return 0; // not reached.
}

base::DataSourceBase::shared_ptr CorbaMethodFactory::produceSend(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const {
    assert(false);
}

base::DataSourceBase::shared_ptr CorbaMethodFactory::produceHandle() const {
    assert(false);
}

base::DataSourceBase::shared_ptr CorbaMethodFactory::produceCollect(const std::vector<base::DataSourceBase::shared_ptr>& args, internal::DataSource<bool>::shared_ptr blocking) const {
    assert(false);
}
