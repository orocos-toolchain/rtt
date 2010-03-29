/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:09:49 CET 2006  TaskContextProxy.cxx

                        TaskContextProxy.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
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

#include "TaskContextProxy.hpp"
#include "TaskContextServer.hpp"
#include "TaskContextC.h"
#include "CorbaMethodFactory.hpp"

#include "../../base/CommandInterface.hpp"
#include "../../types/Types.hpp"
#include "../../extras/SequentialActivity.hpp"
#include "corba.h"
#ifdef CORBA_IS_TAO
#include "tao/TimeBaseC.h"
#include "tao/Messaging/Messaging.h"
#include "tao/Messaging/Messaging_RT_PolicyC.h"
#include "orbsvcs/CosNamingC.h"
#include <ace/String_Base.h>
#else
#include <omniORB4/Naming.hh>
#endif
#include <iostream>
#include <fstream>
#include <string>

#include "RemotePorts.hpp"

using namespace std;
using namespace RTT::detail;

namespace RTT
{namespace corba
{
    IllegalServer::IllegalServer() : reason("This server does not exist or has the wrong type.") {}

    IllegalServer::IllegalServer(const std::string& r) : reason(r) {}

    IllegalServer::~IllegalServer() throw() {}

    const char* IllegalServer::what() const throw() { return reason.c_str(); }


    std::map<TaskContextProxy*, corba::CTaskContext_ptr> TaskContextProxy::proxies;

    PortableServer::POA_var TaskContextProxy::proxy_poa;

    TaskContextProxy::~TaskContextProxy()
    {
        log(Info) << "Terminating TaskContextProxy for " <<  this->getName() <<endlog();
        if ( this->properties() ) {
            flattenPropertyBag( *this->properties() );
            deleteProperties( *this->properties() );
        }
        this->attributes()->clear();
        for (list<PortInterface*>::iterator it = port_proxies.begin(); it != port_proxies.end(); ++it)
            delete *it;
        proxies.erase(this);
    }

    TaskContextProxy::TaskContextProxy(std::string name, bool is_ior)
        : TaskContext("NotFound")
    {
        Logger::In in("TaskContextProxy");
        this->clear();
        this->setActivity( new SequentialActivity() );
        try {
            if (is_ior) {
                // Use the first argument to create the task object reference,
                // in real applications we use the naming service, but let's do
                // the easy part first!
                CORBA::Object_var task_object =
                    orb->string_to_object ( name.c_str() );

                // Now downcast the object reference to the appropriate type
                mtask = corba::CTaskContext::_narrow (task_object.in ());
            } else {
                // NameService
                CORBA::Object_var rootObj;
                CosNaming::NamingContext_var rootContext;
                try {
                    rootObj = orb->resolve_initial_references("NameService");
                    rootContext = CosNaming::NamingContext::_narrow(rootObj);
                } catch (...) {}

                if (CORBA::is_nil(rootContext)) {
                    std::string err("TaskContextProxy could not acquire NameService.");
                    log(Error) << err <<endlog();
                    throw IllegalServer(err);
                }
                Logger::log() <<Logger::Debug << "TaskContextProxy found CORBA NameService."<<endlog();
                CosNaming::Name serverName;
                serverName.length(2);
                serverName[0].id = CORBA::string_dup("TaskContexts");
                serverName[1].id = CORBA::string_dup( name.c_str() );

                // Get object reference
                CORBA::Object_var task_object = rootContext->resolve(serverName);
                mtask = corba::CTaskContext::_narrow (task_object.in ());
            }
            if ( CORBA::is_nil( mtask ) ) {
                std::string err("Failed to acquire TaskContextServer '"+name+"'.");
                Logger::log() << Logger::Error << err <<endlog();
                throw IllegalServer(err);
            }
            CORBA::String_var nm = mtask->getName(); // force connect to object.
            std::string newname( nm.in() );
            this->provides()->setName( newname );
            Logger::log() << Logger::Info << "Successfully connected to TaskContextServer '"+newname+"'."<<endlog();
            proxies[this] = mtask;
        }
        catch (CORBA::Exception &e) {
            log(Error)<< "CORBA exception raised when resolving Object !" << endlog();
            Logger::log() << CORBA_EXCEPTION_INFO(e) << endlog();
            throw;
        }
        catch (IllegalServer& e) {
            // rethrow
            throw e;
        }
        catch (...) {
            log(Error) <<"Unknown Exception in TaskContextProxy construction!"<<endlog();
            throw;
        }

        this->synchronize();
    }

    TaskContextProxy::TaskContextProxy( ::RTT::corba::CTaskContext_ptr taskc)
        : TaskContext("CORBAProxy"), mtask( corba::CTaskContext::_duplicate(taskc) )
    {
        Logger::In in("TaskContextProxy");
        this->clear();
        this->setActivity( new SequentialActivity() );
        try {
            CORBA::String_var nm = mtask->getName(); // force connect to object.
            std::string name( nm.in() );
            this->provides()->setName( name );
            proxies[this] = mtask;
        }
        catch (CORBA::Exception &e) {
            log(Error) << "CORBA exception raised when creating TaskContextProxy!" << Logger::nl;
            Logger::log() << CORBA_EXCEPTION_INFO(e) << endlog();
        }
        catch (...) {
            throw;
        }
        this->synchronize();
    }

    void TaskContextProxy::synchronize()
    {
        // Add here the interfaces that need to be synchronised every time a lookup is done.
        // Detect already added parts of an interface, does not yet detect removed parts...
        if (CORBA::is_nil(mtask))
            return;

        log(Debug) << "Fetching Ports."<<endlog();
        CDataFlowInterface_var dfact = mtask->ports();
        TypeInfoRepository::shared_ptr type_repo = TypeInfoRepository::Instance();
        if (dfact) {
            CDataFlowInterface::CPortDescriptions_var objs = dfact->getPortDescriptions();
            for ( size_t i=0; i < objs->length(); ++i) {
                CPortDescription port = objs[i];
                if (this->ports()->getPort( port.name.in() ))
                    continue; // already added.

                TypeInfo const* type_info = type_repo->type(port.type_name.in());
                if (!type_info)
                {
                    log(Warning) << "remote port " << port.name
                        << " has a type that cannot be marshalled over CORBA: " << port.type_name << ". "
                        << "It is ignored by TaskContextProxy" << endlog();
                }
                else
                {
                    PortInterface* new_port;
                    if (port.type == RTT::corba::CInput)
                        new_port = new RemoteInputPort( type_info, dfact.in(), port.name.in(), ProxyPOA() );
                    else
                        new_port = new RemoteOutputPort( type_info, dfact.in(), port.name.in(), ProxyPOA() );

                    this->ports()->addPort(*new_port);
                    port_proxies.push_back(new_port); // see comment in definition of port_proxies
                }
            }
        }

        CServiceProvider_var serv = mtask->providesService("this");
        this->fetchServices(this->provides(), serv.in() );

        log(Debug) << "All Done."<<endlog();
    }

    // Recursively fetch remote objects and create local proxies.
    void TaskContextProxy::fetchServices(ServiceProvider::shared_ptr parent, CServiceProvider_ptr mserv)
    {
        // load command and method factories.
        // methods:
        log(Debug) << "Fetching Methods."<<endlog();
        COperationRepository::COperationList_var objs;
        objs = mserv->getOperations();
        for ( size_t i=0; i < objs->length(); ++i) {
            if (this->provides()->hasMember( string(objs[i].in() )))
                continue; // already added.
            this->provides()->add( objs[i].in(), new CorbaMethodFactory( objs[i].in(), mserv, ProxyPOA() ) );
        }

        // first do properties:
        log(Debug) << "Fetching Properties."<<endlog();
        CServiceProvider_var serv = mserv->getService("this");
        CAttributeRepository::CPropertyNames_var props = serv->getPropertyList();

        for (size_t i=0; i != props->length(); ++i) {
            if ( this->provides()->hasProperty( string(props[i].name.in()) ) )
                continue; // previously added.
            if ( !serv->hasProperty( CORBA::string_dup(props[i].name.in() ) ) ) {
                log(Error) <<"Property "<< string(props[i].name.in()) << " present in getPropertyList() but not accessible."<<endlog();
                continue;
            }
#if 0 // This code may trigger endless recurse if server has recursive prop bags.
            // See if it is a PropertyBag:
            CORBA::Any_var any = expr->get();
            PropertyBag bag;
            if ( AnyConversion<PropertyBag>::update( *any, bag ) ) {
                Property<PropertyBag>* pbag = new Property<PropertyBag>( string(props[i].name.in()), string(props[i].description.in()), bag);
                this->provides()->properties()->ownProperty( pbag );
                continue;
            }
#endif
            // If the type is known, immediately build the correct property and datasource.
            CORBA::String_var tn = serv->getPropertyTypeName(props[i].name.in());
            TypeInfo* ti = TypeInfoRepository::Instance()->type( tn.in() );
            Logger::log() <<Logger::Info << "Looking up Property " << tn.in();
            if ( ti && ti->getProtocol(ORO_CORBA_PROTOCOL_ID)) {
                CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>(ti->getProtocol(ORO_CORBA_PROTOCOL_ID));
                assert(ctt);
                DataSourceBase::shared_ptr ds = ctt->createPropertyDataSource( serv.in(), props[i].name.in() );
                this->provides()->properties()->ownProperty( ti->buildProperty( props[i].name.in(), props[i].description.in(), ds));
                log(Info) <<" found!"<<endlog();
            }
            else {
                log(Info)<<" not found :-("<<endlog();
            }
        }

        log(Debug) << "Fetching Attributes."<<endlog();
        CAttributeRepository::CAttributeNames_var attrs = serv->getAttributeList();
        for (size_t i=0; i != attrs->length(); ++i) {
            if ( this->provides()->hasAttribute( string(attrs[i].in()) ) )
                continue; // previously added.
            if ( serv->hasAttribute( CORBA::string_dup( attrs[i].in() ) ) ) {
                log(Error) <<"Attribute "<< string(attrs[i].in()) << " present in getAttributeList() but not accessible."<<endlog();
                continue;
            }
            // If the type is known, immediately build the correct attribute and datasource,
            CORBA::String_var tn = serv->getAttributeTypeName( CORBA::string_dup(attrs[i].in()) );
            TypeInfo* ti = TypeInfoRepository::Instance()->type( tn.in() );
            log(Info) << "Looking up Attribute " << tn.in();
            if ( ti && ti->getProtocol(ORO_CORBA_PROTOCOL_ID) ) {
                Logger::log() <<": found!"<<endlog();
                CorbaTypeTransporter* ctt = dynamic_cast<CorbaTypeTransporter*>(ti->getProtocol(ORO_CORBA_PROTOCOL_ID));
                assert(ctt);
                DataSourceBase::shared_ptr ds = ctt->createAttributeDataSource( serv.in(), attrs[i].in() );
                this->provides()->setValue( ti->buildConstant( attrs[i].in(), ds));
            } else {
                Logger::log() <<": not found :-("<<endlog();
            }
        }

        log(Debug) << "Fetching Services of "<<parent->getName()<<":"<<endlog();
        CServiceProvider::CProviderNames_var plist = mserv->getProviderNames();

        for( size_t i =0; i != plist->length(); ++i) {
            if ( string( plist[i] ) == "this")
                continue;
            CServiceProvider_var cobj = mserv->getService(plist[i]);
            CORBA::String_var descr = cobj->getServiceDescription();

            ServiceProvider::shared_ptr tobj = this->provides(std::string(plist[i]));
            tobj->doc( descr.in() );

            // Recurse:
            this->fetchServices( tobj, cobj.in() );
        }
    }

    void TaskContextProxy::DestroyOrb()
    {
        try {
            // Destroy the POA, waiting until the destruction terminates
            //poa->destroy (1, 1);
            orb->destroy();
            std::cerr <<"Orb destroyed."<<std::endl;
        }
        catch (CORBA::Exception &e) {
            log(Error) << "Orb Init : CORBA exception raised!" << Logger::nl;
            Logger::log() << CORBA_EXCEPTION_INFO(e) << endlog();
        }
    }

    TaskContextProxy* TaskContextProxy::Create(std::string name, bool is_ior /*=false*/) {
        if ( CORBA::is_nil(orb) || name.empty() )
            return 0;

        // create new:
        try {
            TaskContextProxy* ctp = new TaskContextProxy( name, is_ior );
            return ctp;
        }
        catch( IllegalServer& is ) {
            cerr << is.what() << endl;
        }
        return 0;
    }

    TaskContextProxy* TaskContextProxy::CreateFromFile(std::string name) {
        if ( CORBA::is_nil(orb) || name.empty() )
            return 0;

        // create new:
        ifstream namestream( name.c_str() );
        string ior;
        namestream >> ior;
        return Create( ior, true);
    }

    TaskContext* TaskContextProxy::Create(::RTT::corba::CTaskContext_ptr t, bool force_remote) {
        Logger::In in("TaskContextProxy::Create");
        if ( CORBA::is_nil(orb) ) {
            log(Error) << "Can not create proxy when ORB is nill !"<<endlog();
            return 0;
        }
        if ( CORBA::is_nil(t) ) {
            log(Error) << "Can not create proxy for nill peer !" <<endlog();
            return 0;
        }

        // proxy present for this object ?
        // is_equivalent is actually our best try.
        for (PMap::iterator it = proxies.begin(); it != proxies.end(); ++it)
            if ( (it->second)->_is_equivalent( t ) ) {
                log(Debug) << "Existing proxy found !" <<endlog();
                return it->first;
            }
        
        // Check if the CTaskContext is actually a local TaskContext
        if (! force_remote)
        {
            for (TaskContextServer::ServerMap::iterator it = TaskContextServer::servers.begin(); it != TaskContextServer::servers.end(); ++it)
                if ( it->second->server()->_is_equivalent( t ) ) {
                    log(Debug) << "Local server found !" <<endlog();
                    return it->first;
                }
        }

        log(Debug) << "No local taskcontext found..." <<endlog();
        // create new:
        try {
            TaskContextProxy* ctp = new TaskContextProxy( t );
            return ctp;
        }
        catch( IllegalServer& is ) {
            cerr << is.what() << endl;
        }
        return 0;
    }

    bool TaskContextProxy::start() {
        try {
            if (! CORBA::is_nil(mtask) )
                return mtask->start();
        } catch(...) {
            mtask = CTaskContext::_nil();
            this->setName("NotFound");
            this->clear();
        }
        return false;
    }

    bool TaskContextProxy::stop() {
        try {
            if (! CORBA::is_nil(mtask) )
                return mtask->stop();
        } catch(...) {
            mtask = CTaskContext::_nil();
            this->setName("NotFound");
            this->clear();
        }
        return false;
    }

    bool TaskContextProxy::activate() {
        try {
            if (! CORBA::is_nil(mtask) )
                return mtask->activate();
        } catch(...) {
            mtask = CTaskContext::_nil();
            this->setName("NotFound");
            this->clear();
        }
        return false;
    }

    bool TaskContextProxy::isActive() const {
        try {
            if (! CORBA::is_nil(mtask) )
                return mtask->isActive();
        } catch(...) {
            mtask = CTaskContext::_nil();
        }
        return false;
    }

    bool TaskContextProxy::isRunning() const {
        try {
            if (! CORBA::is_nil(mtask) )
                return mtask->isRunning();
        } catch(...) {
            mtask = CTaskContext::_nil();
        }
        return false;
    }

    bool TaskContextProxy::configure() {
        try {
            if (! CORBA::is_nil(mtask) )
                return mtask->configure();
        } catch(...) {
            mtask = CTaskContext::_nil();
            this->setName("NotFound");
            this->clear();
        }
        return false;
    }

    bool TaskContextProxy::cleanup() {
        try {
            if (! CORBA::is_nil(mtask) )
                return mtask->cleanup();
        } catch(...) {
            mtask = CTaskContext::_nil();
            this->setName("NotFound");
            this->clear();
        }
        return false;
    }

    bool TaskContextProxy::isConfigured() const {
        try {
            if (! CORBA::is_nil(mtask) )
                return mtask->isConfigured();
        } catch(...) {
            mtask = CTaskContext::_nil();
        }
        return false;
    }

    bool TaskContextProxy::inFatalError() const {
        try {
            if (! CORBA::is_nil(mtask) )
                return mtask->inFatalError();
        } catch(...) {
            mtask = CTaskContext::_nil();
        }
        return false;
    }

    bool TaskContextProxy::inRunTimeError() const {
        try {
            if (! CORBA::is_nil(mtask) )
                return mtask->inRunTimeError();
        } catch(...) {
            mtask = CTaskContext::_nil();
        }
        return false;
    }

    TaskContext::TaskState TaskContextProxy::getTaskState() const {
        try {
            if (! CORBA::is_nil(mtask) )
                return TaskContext::TaskState( mtask->getTaskState() );
        } catch(...) {
            mtask = CTaskContext::_nil();
        }
        return TaskContext::Init;
    }

    void TaskContextProxy::setName(const std::string& n)
    {
        //mtask->setName( n.c_str() );
    }

    bool TaskContextProxy::addPeer( TaskContext* peer, std::string alias /*= ""*/ )
    {
        try {
            if (CORBA::is_nil(mtask))
                return false;

            // if peer is a proxy, add the proxy, otherwise, create new server.
            TaskContextProxy* ctp = dynamic_cast<TaskContextProxy*>( peer );
            if (ctp) {
                if ( mtask->addPeer( ctp->server(), alias.c_str() ) ) {
                    this->synchronize();
                    return true;
                }
                return false;
            }
            // no server yet, create it.
            TaskContextServer* newpeer = TaskContextServer::Create(peer);
            if ( mtask->addPeer( newpeer->server(), alias.c_str() ) ) {
                this->synchronize();
                return true;
            }
        } catch(...) {
            mtask = CTaskContext::_nil();
            this->setName("NotFound");
            this->clear();
        }
        return false;
    }

    void TaskContextProxy::removePeer( const std::string& name )
    {
        try {
            if (CORBA::is_nil(mtask))
                return;
            mtask->removePeer( name.c_str() );
        } catch(...) {
            mtask = CTaskContext::_nil();
            this->setName("NotFound");
            this->clear();
        }
    }

    void TaskContextProxy::removePeer( TaskContext* peer )
    {
        try {
            if (CORBA::is_nil(mtask))
                return;
            mtask->removePeer( peer->getName().c_str() );
        } catch(...) {
            mtask = CTaskContext::_nil();
            this->setName("NotFound");
            this->clear();
        }
    }

    bool TaskContextProxy::connectPeers( TaskContext* peer )
    {
        try {
            if (CORBA::is_nil(mtask))
                return false;
            TaskContextServer* newpeer = TaskContextServer::Create(peer);
            return mtask->connectPeers( newpeer->server() );
        } catch(...) {
            mtask = CTaskContext::_nil();
            this->setName("NotFound");
            this->clear();
        }
        return false;
    }

    void TaskContextProxy::disconnectPeers( const std::string& name )
    {
        try {
            if (! CORBA::is_nil(mtask) )
                mtask->disconnectPeers( name.c_str() );
        } catch(...) {
            mtask = CTaskContext::_nil();
            this->setName("NotFound");
            this->clear();
        }
    }

    TaskContext::PeerList TaskContextProxy::getPeerList() const
    {

        TaskContext::PeerList vlist;
        try {
            if (! CORBA::is_nil(mtask) ) {
                corba::CTaskContext::CTaskContextNames_var plist = mtask->getPeerList();
                for( size_t i =0; i != plist->length(); ++i)
                    vlist.push_back( std::string( plist[i] ) );
            }
        } catch(...) {
            mtask = CTaskContext::_nil();
        }
        return vlist;
    }

    bool TaskContextProxy::hasPeer( const std::string& peer_name ) const
    {
        try {
            if (! CORBA::is_nil(mtask))
                return mtask->hasPeer( peer_name.c_str() );
        } catch(...) {
            mtask = CTaskContext::_nil();
        }
        return false;
    }

    TaskContext* TaskContextProxy::getPeer(const std::string& peer_name ) const
    {
        try {
            if (CORBA::is_nil(mtask))
                return 0;
            corba::CTaskContext_ptr ct = mtask->getPeer( peer_name.c_str() );
            if ( CORBA::is_nil(ct) )
                return 0;
            return TaskContextProxy::Create( ct );
        } catch(...) {
            mtask = CTaskContext::_nil();
        }
        return 0;
    }

    bool TaskContextProxy::connectPorts( TaskContext* peer )
    {
        try {
            if (CORBA::is_nil(mtask))
                return false;
            TaskContextServer* newpeer = TaskContextServer::Create(peer);
            return mtask->connectPorts( newpeer->server() );
        } catch(...) {
            mtask = CTaskContext::_nil();
            this->setName("NotFound");
            this->clear();
        }
        return false;
    }

    bool TaskContextProxy::connectServices( TaskContext* peer )
    {
        try {
            if (CORBA::is_nil(mtask))
                return false;
            TaskContextServer* newpeer = TaskContextServer::Create(peer);
            return mtask->connectServices( newpeer->server() );
        } catch(...) {
            mtask = CTaskContext::_nil();
            this->setName("NotFound");
            this->clear();
        }
        return false;
    }

    bool TaskContextProxy::ready()
    {
        if (CORBA::is_nil(mtask)) {
            this->clear();
            return false;
        }
        try {
            mtask->getName(); // basic check
            return true;
        } catch(...) {
            // we could also try to re-establish the connection in case of naming...
            this->clear();
            mtask = CTaskContext::_nil();
        }
        return false;
    }

    corba::CTaskContext_ptr TaskContextProxy::server() const {
        if ( CORBA::is_nil(mtask) )
            return CTaskContext::_nil();
        return corba::CTaskContext::_duplicate(mtask);
    }

    PortableServer::POA_ptr TaskContextProxy::ProxyPOA() {
        if ( CORBA::is_nil(orb) )
            return PortableServer::POA::_nil();
        if ( CORBA::is_nil(proxy_poa) ) {
            CORBA::Object_var poa_object =
                orb->resolve_initial_references ("RootPOA");

            // new POA for the proxies:
            // Use default manager, is already activated !
            //PortableServer::POAManager_var proxy_manager = poa->the_POAManager ();
            //CORBA::PolicyList pol;
            //proxy_poa = poa->create_POA( "ProxyPOA", proxy_manager, pol );
            proxy_poa =
                PortableServer::POA::_narrow (poa_object.in ());
        }
        // note: do not use _retn(): user must duplicate in constructor.
        return proxy_poa.in();
    }
}}

