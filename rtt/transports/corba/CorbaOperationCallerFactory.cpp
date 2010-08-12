#include "CorbaOperationCallerFactory.hpp"
#include "AnyDataSource.hpp"
#include "../../types/Types.hpp"
#include "../../internal/DataSources.hpp"
#include "../../internal/DataSourceCommand.hpp"
#include "../../SendStatus.hpp"
#include "../../Handle.hpp"

using namespace std;
using namespace RTT;
using namespace RTT::detail;

CorbaOperationCallerFactory::CorbaOperationCallerFactory( const std::string& method_name, corba::CService_ptr fact, PortableServer::POA_ptr the_poa )
    : RTT::interface::OperationRepositoryPart(),
      mfact(corba::CService::_duplicate(fact) ),
      mpoa(PortableServer::POA::_duplicate(the_poa)),
      method(method_name)
{}

CorbaOperationCallerFactory::~CorbaOperationCallerFactory() {}

unsigned int CorbaOperationCallerFactory::arity()  const {
    return mfact->getArity( method.c_str() );
}

unsigned int CorbaOperationCallerFactory::collectArity()  const {
    return mfact->getCollectArity( method.c_str() );
}

const TypeInfo* CorbaOperationCallerFactory::getArgumentType(unsigned int i) const {
    try {
        CORBA::String_var tname = mfact->getArgumentType( method.c_str(), i);
        if ( Types()->type( tname.in() ) != 0 )
            return Types()->type( tname.in() );
        // locally unknown type:
        if (i == 0)
            log(Warning) << "CorbaOperationCallerFactory: remote operation's "<< method <<" return type " << tname.in() << " is unknown in this process." << endlog();
        else
            log(Warning) << "CorbaOperationCallerFactory: remote operation's "<< method <<" argument "<< i <<" of type " << tname.in() << " is unknown in this process." << endlog();
    } catch ( CNoSuchNameException& nsne) {
        assert(false);
    }
    catch ( CWrongArgumentException& wae){
        log(Error) << "CorbaOperationCallerFactory::getArgumentType: Wrong arg nbr: " << wae.which_arg <<" max is " << wae.max_arg <<endlog();
    }
    return 0;
}

const TypeInfo* CorbaOperationCallerFactory::getCollectType(unsigned int i) const {
    try {
        CORBA::String_var tname = mfact->getCollectType( method.c_str(), i);
        return Types()->getTypeById( tname.in() );
    } catch (...){
        return 0;
    }
}


std::string CorbaOperationCallerFactory::resultType() const {
    try {
        CORBA::String_var result = mfact->getResultType( method.c_str() );
        return std::string( result.in() );
    } catch ( corba::CNoSuchNameException& nsn ) {
        throw interface::name_not_found_exception( nsn.name.in() );
    }
    return std::string();
}

std::string CorbaOperationCallerFactory::description() const {
    try {
        CORBA::String_var result = mfact->getDescription( method.c_str() );
        return std::string( result.in() );
    } catch ( corba::CNoSuchNameException& nsn ) {
        throw interface::name_not_found_exception( nsn.name.in() );
    }
    return std::string();
}

std::vector< interface::ArgumentDescription > CorbaOperationCallerFactory::getArgumentList() const {
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
 * This is not a datasource because we separate the returning
 * of the data from the calling code in order to avoid that the calling code
 * depends on templates (ie a DataSource<T> base class).
 * This class needs to be combined with an ActionAliasDataSource that ties this
 * action to the value datasource referenced by mresult.
 */
class CorbaOperationCallerCall: public ActionInterface
{
    CService_var mfact;
    std::string mop;
    std::vector<base::DataSourceBase::shared_ptr> margs;
    ExecutionEngine* mcaller;
    base::DataSourceBase::shared_ptr mresult;
    corba::CAnyArguments_var nargs;
    // The type transporter for the return value
    CorbaTypeTransporter* mctt;
    bool mdocall;
public:
    CorbaOperationCallerCall(CService_ptr fact,
                    std::string op,
                    std::vector<base::DataSourceBase::shared_ptr> const& args,
                    ExecutionEngine* caller,
                    CorbaTypeTransporter* ctt,
                    base::DataSourceBase::shared_ptr result, bool docall)
    : mfact(CService::_duplicate(fact)), mop(op), margs(args), mcaller(caller), mresult(result), mctt(ctt), mdocall(docall)
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
            ctt->updateAny(margs[i], nargs[i]);
        }
    }

    bool execute() {
        try {
            if (mdocall) {
                CORBA::Any_var any = mfact->callOperation( mop.c_str(), nargs.inout() );
                for (size_t i=0; i < margs.size(); ++i ) {
                    const types::TypeInfo* ti = margs[i]->getTypeInfo();
                    CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
                    assert( ctt );
                    ctt->updateFromAny( &nargs[i], margs[i] );
                }
                // convert returned any to local type:
                if (mctt)
                    return mctt->updateFromAny(&any.in(), mresult);
            } else {
                CSendHandle_var sh = mfact->sendOperation( mop.c_str(), nargs.in() );
                AssignableDataSource<CSendHandle_var>::shared_ptr ads = AssignableDataSource<CSendHandle_var>::narrow( mresult.get() );
                if (ads) {
                    ads->set( sh ); // _var creates a copy of the obj reference.
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

    ActionInterface* clone() const { return new CorbaOperationCallerCall(CService::_duplicate( mfact.in() ), mop, margs, mcaller, mctt, mresult, mdocall); }

    virtual ActionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
        vector<DataSourceBase::shared_ptr> argcopy( margs.size() );
        unsigned int v=0;
        for (vector<DataSourceBase::shared_ptr>::iterator it = argcopy.begin(); it != argcopy.end(); ++it, ++v)
            argcopy[v] = (*it)->copy(alreadyCloned);
        return new CorbaOperationCallerCall(CService::_duplicate( mfact.in() ), mop, argcopy, mcaller, mctt, mresult->copy(alreadyCloned), mdocall);
    }
};

base::DataSourceBase::shared_ptr CorbaOperationCallerFactory::produce(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const {
    corba::CAnyArguments_var nargs = new corba::CAnyArguments();
    nargs->length( args.size() );
    for (size_t i=0; i < args.size(); ++i ) {
        const types::TypeInfo* ti = args[i]->getTypeInfo();
        CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
        assert( ctt );
        ctt->updateAny(args[i], nargs[i]);
    }
    try {
        // will throw if wrong args.
        mfact->checkOperation(method.c_str(), nargs.in() );
        // convert returned any to local type:
        const types::TypeInfo* ti = this->getArgumentType(0);
        if ( ti ) {
            if ( ti != Types()->type("void") ) {
                // create a method call object and a return value and let the former store results in the latter.
                CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
                DataSourceBase::shared_ptr result = ti->buildValue();
                // evaluate()/get() will cause the method to be called and remote return value will end up in result.
                return ti->buildActionAlias(new CorbaOperationCallerCall(mfact.in(),method,args,caller, ctt, result, true), result );
            } else {
                return new DataSourceCommand( new CorbaOperationCallerCall(mfact.in(),method,args,caller, 0, DataSourceBase::shared_ptr() , true) );
            }
        } else {
            // it's returning a type we don't know ! Return a DataSource<Any>
            DataSource<CORBA::Any_var>::shared_ptr result = new AnyDataSource( new CORBA::Any() );
            // todo Provide a ctt  implementation for 'CORBA::Any_var' such that the result is updated !
            // The result is only for dummy reasons used now, since no ctt is set, no updating will be done.
            return new ActionAliasDataSource<CORBA::Any_var>(new CorbaOperationCallerCall(mfact.in(),method,args,caller, 0, result, true), result.get() );
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

base::DataSourceBase::shared_ptr CorbaOperationCallerFactory::produceSend(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const {
    corba::CAnyArguments_var nargs = new corba::CAnyArguments();
    nargs->length( args.size() );
    for (size_t i=0; i < args.size(); ++i ) {
        const types::TypeInfo* ti = args[i]->getTypeInfo();
        CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
        assert( ctt );
        ctt->updateAny(args[i], nargs[i]);
    }
    try {
        // will throw if wrong args.
        mfact->checkOperation(method.c_str(), nargs.inout() );
        // Will return a CSendHandle_var:
        DataSource<CSendHandle_var>::shared_ptr result = new ValueDataSource<CSendHandle_var>();
        return new ActionAliasDataSource<CSendHandle_var>(new CorbaOperationCallerCall(mfact.in(),method,args,caller, 0, result, false), result.get() );
    } catch ( corba::CNoSuchNameException& nsn ) {
        throw interface:: name_not_found_exception( nsn.name.in() );
    } catch ( corba::CWrongNumbArgException& wa ) {
        throw interface:: wrong_number_of_args_exception( wa.wanted, wa.received );
    } catch ( corba::CWrongTypeArgException& wta ) {
        throw interface:: wrong_types_of_args_exception( wta.whicharg, wta.expected.in(), wta.received.in() );
    }
    return 0; // not reached.
}

base::DataSourceBase::shared_ptr CorbaOperationCallerFactory::produceHandle() const {
    // collect expects a handle of this type. Also send returns a CSendHandle_var, which can be copied into this handle object.
    return new ValueDataSource<CSendHandle_var>();
}

/**
 * Collects a CORBA method.
 * This is not a datasource because we separate the returning
 * of the data from the calling code in order to avoid that the calling code
 * depends on templates (ie a DataSource<T> base class).
 * This class needs to be combined with an ActionAliasDataSource that ties this
 * action to the value datasource referenced by mresult.
 */
class CorbaOperationCallerCollect: public DataSource<SendStatus>
{
    CSendHandle_var msh;
    std::vector<base::DataSourceBase::shared_ptr> margs;
    DataSource<bool>::shared_ptr misblocking;
    mutable SendStatus mss;
public:
    CorbaOperationCallerCollect(CSendHandle_ptr sh,
                       std::vector<base::DataSourceBase::shared_ptr> const& args,
                       DataSource<bool>::shared_ptr isblocking)
    : msh( CSendHandle::_duplicate(sh)), margs(args), misblocking(isblocking), mss(SendFailure)
    {
    }

    SendStatus value() const { return mss; }

    SendStatus const& rvalue() const { return mss; }

    SendStatus get() const {
        try {
            // only try to collect if we didn't do so before:
            if ( mss != SendSuccess ) {
                corba::CAnyArguments_var nargs;
                if ( misblocking.get() ) {
                    mss = SendStatus( msh->collect( nargs.out() ) );
                } else {
                    mss = SendStatus( msh->collectIfDone( nargs.out() ) );
                }
                // only convert results when we got a success:
                if (mss == SendSuccess) {
                    assert( nargs->length() ==  margs.size() );
                    for (size_t i=0; i < margs.size(); ++i ) {
                        const types::TypeInfo* ti = margs[i]->getTypeInfo();
                        CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
                        assert( ctt );
                        ctt->updateFromAny( &nargs[i], margs[i] );
                    }
                }
            }
            return mss;
        }  catch ( corba::CWrongNumbArgException& wa ) {
            return mss;
        } catch ( corba::CWrongTypeArgException& wta ) {
            return mss;
        }
    }

    DataSource<SendStatus>* clone() const { return new CorbaOperationCallerCollect(CSendHandle::_duplicate( msh.in() ), margs, misblocking); }

    virtual DataSource<SendStatus>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
        vector<DataSourceBase::shared_ptr> argcopy( margs.size() );
        unsigned int v=0;
        for (vector<DataSourceBase::shared_ptr>::iterator it = argcopy.begin(); it != argcopy.end(); ++it, ++v)
            argcopy[v] = (*it)->copy(alreadyCloned);
        return new CorbaOperationCallerCollect(CSendHandle::_duplicate( msh.in() ), argcopy, misblocking);
    }
};


base::DataSourceBase::shared_ptr CorbaOperationCallerFactory::produceCollect(const std::vector<base::DataSourceBase::shared_ptr>& args, internal::DataSource<bool>::shared_ptr blocking) const {
    unsigned int expected = mfact->getCollectArity(method.c_str());
    if (args.size() !=  expected + 1) {
        throw interface::wrong_number_of_args_exception( expected + 1, args.size() );
    }
    // isolate and check CSendHandle
    std::vector<base::DataSourceBase::shared_ptr> cargs( ++args.begin(), args.end() );
    DataSource<CSendHandle_var>::shared_ptr ds = DataSource<CSendHandle_var>::narrow( args.begin()->get() );
    if (!ds) {
        throw interface::wrong_types_of_args_exception(0,"CSendHandle_var",(*args.begin())->getTypeName() );
    }
    // check if args matches what CSendHandle expects.
    try {
        corba::CAnyArguments_var nargs = new corba::CAnyArguments();
        nargs->length( cargs.size() );
        for (size_t i=0; i < cargs.size(); ++i ) {
            const types::TypeInfo* ti = cargs[i]->getTypeInfo();
            CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
            assert( ctt );
            ctt->updateAny( cargs[i], nargs[i]);
        }
        ds->get()->checkArguments( nargs.in() );
    } catch ( CWrongNumbArgException& wna) {
        throw interface::wrong_number_of_args_exception(wna.wanted, wna.received);
    } catch ( CWrongTypeArgException& wta) {
        throw interface::wrong_types_of_args_exception(wta.whicharg,wta.expected.in(), wta.received.in());
    }
    // All went well, produce collect DataSource:
    return new CorbaOperationCallerCollect( ds->get().in(),cargs, blocking);
}

Handle CorbaOperationCallerFactory::produceSignal(base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args) const {
    log(Error) << "Can not attach Signal to remote Corba Operation '"<<method <<"'" <<endlog();
    return Handle();
}
