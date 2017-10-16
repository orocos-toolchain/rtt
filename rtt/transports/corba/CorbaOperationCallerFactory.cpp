/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  CorbaOperationCallerFactory.cpp

                        CorbaOperationCallerFactory.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#include "CorbaOperationCallerFactory.hpp"
#include "AnyDataSource.hpp"
#include "CorbaLib.hpp"

#include "../../types/Types.hpp"
#include "../../internal/DataSources.hpp"
#include "../../internal/DataSourceCommand.hpp"
#include "../../SendStatus.hpp"
#include "../../Handle.hpp"

using namespace std;
using namespace RTT;
using namespace RTT::detail;

CorbaOperationCallerFactory::CorbaOperationCallerFactory(const std::string &method_name, corba::CService_ptr fact, PortableServer::POA_ptr the_poa )
    : RTT::OperationInterfacePart(),
      mfact(corba::CService::_duplicate(fact) ),
      mpoa(PortableServer::POA::_duplicate(the_poa)),
      method(method_name)
{}

CorbaOperationCallerFactory::CorbaOperationCallerFactory(const COperation &operation, corba::CService_ptr fact, PortableServer::POA_ptr the_poa )
    : RTT::OperationInterfacePart(),
      mfact(corba::CService::_duplicate(fact) ),
      mpoa(PortableServer::POA::_duplicate(the_poa)),
      method(operation.name.in()),
      mdescription(new COperation(operation))
{}

CorbaOperationCallerFactory::~CorbaOperationCallerFactory() {}

unsigned int CorbaOperationCallerFactory::arity()  const {
    if (mdescription)
        return mdescription->arguments.length();

    return mfact->getArity( method.c_str() );
}

unsigned int CorbaOperationCallerFactory::collectArity()  const {
    if (mdescription)
        return mdescription->collect_types.length();

    return mfact->getCollectArity( method.c_str() );
}

const TypeInfo* CorbaOperationCallerFactory::getArgumentType(unsigned int i) const {
    std::string tname;

    if (mdescription) {
        if (i == 0) {
            tname = mdescription->result_type.in();
        } else if (i <= mdescription->arguments.length()) {
            tname = mdescription->arguments[i-1].type.in();
        }

        // COperation mdescription contains fully-qualified argument types as returned
        // by OperationInterfacePart::getArgumentList() and DataSourceTypeInfo<T>::getType(),
        // while COperationInterface::getArgumentType() would return the unqualified type
        // name as DataSourceTypeInfo<T>::getType().
        // ==> Strip qualifiers from tname before lookup in the TypeInfoRepository.
        std::string::size_type separator = tname.find(' ');
        if ( separator != std::string::npos ) {
            tname = tname.substr(0, separator);
        }

    } else {
        try {
            tname = mfact->getArgumentType( method.c_str(), i);
        } catch ( CNoSuchNameException& ) {
            assert(false);
        }
        catch ( CWrongArgumentException& wae){
            log(Error) << "CorbaOperationCallerFactory::getArgumentType: Wrong arg nbr: " << wae.which_arg <<" max is " << wae.max_arg <<endlog();
        }
    }

    if (tname.empty()) return 0;

    if ( Types()->type( tname ) != 0 )
        return Types()->type( tname );
    // locally unknown type:
    if (i == 0)
        log(Warning) << "CorbaOperationCallerFactory: remote operation's "<< method <<" return type " << tname << " is unknown in this process." << endlog();
    else
        log(Warning) << "CorbaOperationCallerFactory: remote operation's "<< method <<" argument "<< i <<" of type " << tname << " is unknown in this process." << endlog();

    return 0;
}

const TypeInfo* CorbaOperationCallerFactory::getCollectType(unsigned int i) const {

    if (mdescription && i < mdescription->collect_types.length()) {
        return Types()->type( mdescription->collect_types[i].in() );

    } else {

        try {
            CORBA::String_var tname = mfact->getCollectType( method.c_str(), i);
            return Types()->type( tname.in() );
        } catch (...){
            return 0;
        }
    }

    return 0;
}


std::string CorbaOperationCallerFactory::resultType() const {
    if (mdescription)
        return mdescription->result_type.in();

    try {
        CORBA::String_var result = mfact->getResultType( method.c_str() );
        return std::string( result.in() );
    } catch ( corba::CNoSuchNameException& nsn ) {
        throw name_not_found_exception( nsn.name.in() );
    }
    return std::string();
}

std::string CorbaOperationCallerFactory::getName() const {
    return method;
}

std::string CorbaOperationCallerFactory::description() const {
    if (mdescription)
        return mdescription->description.in();

    try {
        CORBA::String_var result = mfact->getDescription( method.c_str() );
        return std::string( result.in() );
    } catch ( corba::CNoSuchNameException& nsn ) {
        throw name_not_found_exception( nsn.name.in() );
    }
    return std::string();
}

std::vector< ArgumentDescription > CorbaOperationCallerFactory::getArgumentList() const {
    corba::CArgumentDescriptions_var result;
    const corba::CArgumentDescriptions *result_ptr = 0;

    if (mdescription) {
        result_ptr = &(mdescription->arguments);
    } else {
        try {
            result = mfact->getArguments( method.c_str() );
            result_ptr = &(result.in());
        } catch ( corba::CNoSuchNameException& nsn ) {
            throw  name_not_found_exception( nsn.name.in() );
        }
    }

    CArgumentDescriptions ret;
    ret.reserve( result_ptr->length() );
    for (size_t i=0; i!= result_ptr->length(); ++i)
        ret.push_back( ArgumentDescription(std::string( (*result_ptr)[i].name.in() ),
                                           std::string( (*result_ptr)[i].description.in() ),
                                           std::string( (*result_ptr)[i].type.in() ) ));
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
    bool moneway;
public:
    CorbaOperationCallerCall(CService_ptr fact,
                    std::string op,
                    std::vector<base::DataSourceBase::shared_ptr> const& args,
                    ExecutionEngine* caller,
                    CorbaTypeTransporter* ctt,
                    base::DataSourceBase::shared_ptr result, bool docall, bool oneway)
    : mfact(CService::_duplicate(fact)), mop(op), margs(args), mcaller(caller), mresult(result), mctt(ctt), mdocall(docall), moneway(oneway)
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
                if (!moneway) {
                    CSendHandle_var sh = mfact->sendOperation( mop.c_str(), nargs.in() );
                    AssignableDataSource<CSendHandle_var>::shared_ptr ads = AssignableDataSource<CSendHandle_var>::narrow( mresult.get() );
                    if (ads) {
                        ads->set( sh ); // _var creates a copy of the obj reference.
                    }
                } else {
                    mfact->sendOperationOneway( mop.c_str(), nargs.in() );
                }

            }
            return true;
        } catch ( corba::CNoSuchNameException& ) {
            return false;
        } catch ( corba::CWrongNumbArgException& ) {
            return false;
        } catch ( corba::CWrongTypeArgException& ) {
            return false;
        } catch ( corba::CCallError& e) {
            throw std::runtime_error(std::string(e.what.in()));
        }
    }

    ActionInterface* clone() const { return new CorbaOperationCallerCall(CService::_duplicate( mfact.in() ), mop, margs, mcaller, mctt, mresult, mdocall, moneway); }

    virtual ActionInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
        vector<DataSourceBase::shared_ptr> argcopy( margs.size() );
        unsigned int v=0;
        for (vector<DataSourceBase::shared_ptr>::iterator it = argcopy.begin(); it != argcopy.end(); ++it, ++v)
            argcopy[v] = (*it)->copy(alreadyCloned);
        DataSourceBase::shared_ptr result = mresult ? mresult->copy(alreadyCloned) : 0;
        return new CorbaOperationCallerCall(CService::_duplicate( mfact.in() ), mop, argcopy, mcaller, mctt, result, mdocall, moneway);
    }
};

base::DataSourceBase::shared_ptr CorbaOperationCallerFactory::produce(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const {
#ifndef RTT_CORBA_NO_CHECK_OPERATIONS
    corba::CAnyArguments_var nargs = new corba::CAnyArguments();
    nargs->length( args.size() );

    // this part is only done to feed to checkOperation() with some defaults.
    // We don't want to evaluate() the *real* data sources yet !
    for (size_t i=0; i < args.size(); ++i ) {
        const types::TypeInfo* ti = args[i]->getTypeInfo();
        CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
        if (!ctt)
		throw wrong_types_of_args_exception(i+1,"type known to CORBA transport", ti->getTypeName());
        DataSourceBase::shared_ptr tryout = ti->buildValue();
        ctt->updateAny(tryout, nargs[i]);
    }
#endif

    // check argument types and produce:
    try {
#ifndef RTT_CORBA_NO_CHECK_OPERATIONS
        // will throw if wrong args.
        mfact->checkOperation(method.c_str(), nargs.in() );
#endif
        // convert returned any to local type:
        const types::TypeInfo* ti = this->getArgumentType(0);
        if ( ti ) {
            if ( ti != Types()->type("void") ) {
                // create a method call object and a return value and let the former store results in the latter.
                CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
                DataSourceBase::shared_ptr result = ti->buildValue();
                // evaluate()/get() will cause the method to be called and remote return value will end up in result.
                return ti->buildActionAlias(new CorbaOperationCallerCall(mfact.in(),method,args,caller, ctt, result, true, false), result );
            } else {
                return new DataSourceCommand( new CorbaOperationCallerCall(mfact.in(),method,args,caller, 0, DataSourceBase::shared_ptr() , true, false) );
            }
        } else {
            // it's returning a type we don't know ! Return a DataSource<Any>
            DataSource<CORBA::Any_var>::shared_ptr result = new AnyDataSource( new CORBA::Any() );
            // todo Provide a ctt  implementation for 'CORBA::Any_var' such that the result is updated !
            // The result is only for dummy reasons used now, since no ctt is set, no updating will be done.
            return new ActionAliasDataSource<CORBA::Any_var>(new CorbaOperationCallerCall(mfact.in(),method,args,caller, 0, result, true, false), result.get() );
        }
    } catch ( corba::CNoSuchNameException& nsn ) {
        throw  name_not_found_exception( nsn.name.in() );
    } catch ( corba::CWrongNumbArgException& wa ) {
        throw  wrong_number_of_args_exception( wa.wanted, wa.received );
    } catch ( corba::CWrongTypeArgException& wta ) {
        throw  wrong_types_of_args_exception( wta.whicharg, wta.expected.in(), wta.received.in() );
    }
    return 0; // not reached.
}

base::DataSourceBase::shared_ptr CorbaOperationCallerFactory::produceSend(const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* caller) const {
#ifndef RTT_CORBA_NO_CHECK_OPERATIONS
    corba::CAnyArguments_var nargs = new corba::CAnyArguments();
    nargs->length( args.size() );
    for (size_t i=0; i < args.size(); ++i ) {
        const types::TypeInfo* ti = args[i]->getTypeInfo();
        CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
        if (!ctt)
		throw wrong_types_of_args_exception(i+1,"type known to CORBA transport", ti->getTypeName());
        DataSourceBase::shared_ptr tryout = ti->buildValue();
        ctt->updateAny(tryout, nargs[i]);
    }
#endif

    try {
#ifndef RTT_CORBA_NO_CHECK_OPERATIONS
        // will throw if wrong args.
        mfact->checkOperation(method.c_str(), nargs.inout() );
#endif
        // Will return a CSendHandle_var:
        DataSource<CSendHandle_var>::shared_ptr result = new ValueDataSource<CSendHandle_var>();
#ifdef RTT_CORBA_SEND_ONEWAY_OPERATIONS
        bool oneway = (mdescription && mdescription->send_oneway);
#else
        bool oneway = false;
#endif
        return new ActionAliasDataSource<CSendHandle_var>(new CorbaOperationCallerCall(mfact.in(),method,args,caller, 0, result, false, oneway), result.get() );
    } catch ( corba::CNoSuchNameException& nsn ) {
        throw  name_not_found_exception( nsn.name.in() );
    } catch ( corba::CWrongNumbArgException& wa ) {
        throw  wrong_number_of_args_exception( wa.wanted, wa.received );
    } catch ( corba::CWrongTypeArgException& wta ) {
        throw  wrong_types_of_args_exception( wta.whicharg, wta.expected.in(), wta.received.in() );
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

    ~CorbaOperationCallerCollect() {
    	try {
    		msh->dispose();
    	} catch(...) {}
    }

    SendStatus value() const { return mss; }

    SendStatus const& rvalue() const { return mss; }

    SendStatus get() const {
        try {
            // only try to collect if we didn't do so before:
            if ( mss != SendSuccess ) {
                corba::CAnyArguments_var nargs;
                if ( misblocking->get() ) {
                    mss = SendStatus( static_cast<int>(msh->collect( nargs.out() ) ) - 2 );
                } else {
                    mss = SendStatus( static_cast<int>(msh->collectIfDone( nargs.out() ) ) - 2 );
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
        }  catch ( corba::CWrongNumbArgException& ) {
            return mss;
        } catch ( corba::CWrongTypeArgException& ) {
            return mss;
        } catch ( corba::CCallError& e) {
            throw std::runtime_error(std::string(e.what.in()));
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
    unsigned int expected = 0;
    if (mdescription)
        expected = mdescription->collect_types.length();
    else
        expected = mfact->getCollectArity(method.c_str());

    if (args.size() !=  expected + 1) {
        throw wrong_number_of_args_exception( expected + 1, args.size() );
    }
    // isolate and check CSendHandle
    std::vector<base::DataSourceBase::shared_ptr> cargs( ++args.begin(), args.end() );
    DataSource<CSendHandle_var>::shared_ptr ds = DataSource<CSendHandle_var>::narrow( args.begin()->get() );
    if (!ds) {
        throw wrong_types_of_args_exception(0,"CSendHandle_var",(*args.begin())->getTypeName() );
    }
    if ( CORBA::is_nil(ds->get()) ) return new ValueDataSource<SendStatus>(SendSuccess);
    // check if args matches what CSendHandle expects.
    try {
        corba::CAnyArguments_var nargs = new corba::CAnyArguments();
        nargs->length( cargs.size() );
        for (size_t i=0; i < cargs.size(); ++i ) {
            const types::TypeInfo* ti = cargs[i]->getTypeInfo();
            CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>( ti->getProtocol(ORO_CORBA_PROTOCOL_ID) );
            assert( ctt );
            DataSourceBase::shared_ptr tryout = ti->buildValue();
            ctt->updateAny(tryout, nargs[i]);
        }
        ds->get()->checkArguments( nargs.in() );
    } catch ( CWrongNumbArgException& wna) {
        throw wrong_number_of_args_exception(wna.wanted, wna.received);
    } catch ( CWrongTypeArgException& wta) {
        throw wrong_types_of_args_exception(wta.whicharg,wta.expected.in(), wta.received.in());
    }
    // All went well, produce collect DataSource:
    return new CorbaOperationCallerCollect( ds->get().in(),cargs, blocking);
}

#ifdef ORO_SIGNALLING_OPERATIONS
Handle CorbaOperationCallerFactory::produceSignal(base::ActionInterface* func, const std::vector<base::DataSourceBase::shared_ptr>& args, ExecutionEngine* subscriber) const {
    log(Error) << "Can not attach Signal to remote Corba Operation '"<<method <<"'" <<endlog();
    return Handle();
}
#endif
