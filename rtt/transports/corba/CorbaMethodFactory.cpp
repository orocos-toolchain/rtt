#include "CorbaMethodFactory.hpp"
#include "AnyDataSource.hpp"
#include "../../types/Types.hpp"
#include "../../internal/DataSources.hpp"
#include "../../internal/DataSourceCommand.hpp"

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

const TypeInfo* CorbaMethodFactory::getArgumentType(unsigned int i) const {
    try {
        CORBA::String_var tname = mfact->getArgumentType( method.c_str(), i);
        if ( Types()->type( tname.in() ) != 0 )
            return Types()->type( tname.in() );
        // locally unknown type:
        if (i == 0)
            log(Warning) << "CorbaMethodFactory: remote operation's "<< method <<" return type " << tname.in() << " is unknown in this process." << endlog();
        else
            log(Warning) << "CorbaMethodFactory: remote operation's "<< method <<" argument "<< i <<" of type " << tname.in() << " is unknown in this process." << endlog();
    } catch ( CNoSuchNameException& nsne) {
        assert(false);
    }
    catch ( CWrongArgumentException& wae){
        log(Error) << "CorbaMethodFactory::getArgumentType: Wrong arg nbr: " << wae.which_arg <<" max is " << wae.max_arg <<endlog();
    }
    return 0;
}

const TypeInfo* CorbaMethodFactory::getCollectType(unsigned int i) const {
    try {
        CORBA::String_var tname = mfact->getCollectType( method.c_str(), i);
        return Types()->getTypeById( tname.in() );
    } catch (...){
        return 0;
    }
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

/**
 * Calls a CORBA method.
 */
class CorbaMethodCall: public ActionInterface
{
    CServiceProvider_var mfact;
    std::string mop;
    std::vector<base::DataSourceBase::shared_ptr> margs;
    ExecutionEngine* mcaller;
    base::DataSourceBase::shared_ptr mresult;
    corba::CAnyArguments_var nargs;
    // The type transporter for the return value
    CorbaTypeTransporter* mctt;
    bool mdocall;
public:
    CorbaMethodCall(CServiceProvider_ptr fact,
                    std::string op,
                    std::vector<base::DataSourceBase::shared_ptr> const& args,
                    ExecutionEngine* caller,
                    CorbaTypeTransporter* ctt,
                    base::DataSourceBase::shared_ptr result, bool docall)
    : mfact(CServiceProvider::_duplicate(fact)), mop(op), margs(args), mcaller(caller), mresult(result), mctt(ctt), mdocall(docall)
    {
    }

    void readArguments() {
        // We need to delay reading the arguments upto this point such that the args contain
        // the latest values.
        nargs = new corba::CAnyArguments();
        nargs->length( margs.size() );
        for (size_t i=0; i < margs.size(); ++i ) {
            const types::TypeInfo* ti = margs[i]->getTypeInfo();
            CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
            assert( ctt );
            CORBA::Any_var any = ctt->createAny( margs[i] );
            nargs[i] = *any;
        }
    }

    bool execute() {
        try {
            if (mdocall) {
                CORBA::Any_var any = mfact->callOperation( mop.c_str(), nargs.inout() );
                // todo: convert the arguments to local types:
                //
                // convert returned any to local type:
                if (mctt)
                    return mctt->updateFromAny(any.ptr(), mresult);
            } else {
                CSendHandle_var sh = mfact->sendOperation( mop.c_str(), nargs.inout() );
                AssignableDataSource<CSendHandle_var>::shared_ptr ads = AssignableDataSource<CSendHandle_var>::narrow( mresult.get() );
                if (ads) {
                    ads->set( sh );
                }
            }
            return true;
        } catch ( corba::CNoSuchNameException& nsn ) {
            return false;
        } catch ( corba::CWrongNumbArgException& wa ) {
            return false;
        } catch ( corba::CWrongTypeArgException& wta ) {
            return false;
        }

    }

    ActionInterface* clone() const { return new CorbaMethodCall(CServiceProvider::_duplicate( mfact.in() ), mop, margs, mcaller, mctt, mresult, mdocall); }
};

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
        // will throw if wrong args.
        mfact->checkOperation(method.c_str(), nargs.inout() );
        // convert returned any to local type:
        const types::TypeInfo* ti = this->getArgumentType(0);
        if ( ti ) {
            if ( ti != Types()->type("void") ) {
                CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
                DataSourceBase::shared_ptr result = ti->buildValue();
                return ti->buildActionAlias(new CorbaMethodCall(mfact.in(),method,args,caller, ctt, result, true), result );
            } else {
                return new DataSourceCommand( new CorbaMethodCall(mfact.in(),method,args,caller, 0, DataSourceBase::shared_ptr() , true) );
            }
        } else {
            // it's returning a type we don't know ! Return a DataSource<Any>
            DataSource<CORBA::Any_var>::shared_ptr result = new AnyDataSource( new CORBA::Any() );
            // todo Provide a ctt  implementation such that the result is updated !
            // The result is only for dummy reasons used now, since no ctt is set, no updating will be done.
            return new ActionAliasDataSource<CORBA::Any_var>(new CorbaMethodCall(mfact.in(),method,args,caller, 0, result, true), result.get() );
        }
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
        // will throw if wrong args.
        mfact->checkOperation(method.c_str(), nargs.inout() );
        // Will return a CSendHandle_var:
        DataSource<CSendHandle_var>::shared_ptr result = new ValueDataSource<CSendHandle_var>();
        return new ActionAliasDataSource<CSendHandle_var>(new CorbaMethodCall(mfact.in(),method,args,caller, 0, result, false), result.get() );
    } catch ( corba::CNoSuchNameException& nsn ) {
        throw interface:: name_not_found_exception( nsn.name.in() );
    } catch ( corba::CWrongNumbArgException& wa ) {
        throw interface:: wrong_number_of_args_exception( wa.wanted, wa.received );
    } catch ( corba::CWrongTypeArgException& wta ) {
        throw interface:: wrong_types_of_args_exception( wta.whicharg, wta.expected.in(), wta.received.in() );
    }
    return 0; // not reached.
}

base::DataSourceBase::shared_ptr CorbaMethodFactory::produceHandle() const {
    // collect expects a handle of this type. Also send returns a CSendHandle_var, which can be copied into this handle object.
    return new ValueDataSource<CSendHandle_var>();
}

base::DataSourceBase::shared_ptr CorbaMethodFactory::produceCollect(const std::vector<base::DataSourceBase::shared_ptr>& args, internal::DataSource<bool>::shared_ptr blocking) const {
    assert(false);
}
