/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:09:49 CET 2006  ControlTaskProxy.cxx 

                        ControlTaskProxy.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
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
 
 


#include "rtt/corba/ControlTaskProxy.hpp"
#include "rtt/corba/ControlTaskServer.hpp"
#include "rtt/corba/ControlTaskC.h"
#include "rtt/corba/CorbaMethodFactory.hpp"
#include "rtt/corba/CorbaCommandFactory.hpp"
#include "rtt/corba/CORBAExpression.hpp"
#include "rtt/corba/ScriptingAccessProxy.hpp"
#include "rtt/CommandInterface.hpp"
#include "rtt/Types.hpp"
#include "orbsvcs/CosNamingC.h"
#include <iostream>

#include <ace/String_Base.h>

using namespace std;

namespace RTT
{namespace Corba
{
    using namespace Orocos;

    IllegalServer::IllegalServer() : reason("This server does not exist or has the wrong type.") {}

    IllegalServer::~IllegalServer() throw() {}

    const char* IllegalServer::what() const throw() { return reason.c_str(); }


    std::map<Orocos::ControlTask_ptr, ControlTaskProxy*> ControlTaskProxy::proxies;


    ControlTaskProxy::~ControlTaskProxy()
    {
        if ( this->attributes()->properties() ) {
            flattenPropertyBag( *this->attributes()->properties() );
            deleteProperties( *this->attributes()->properties() );
        }
        this->attributes()->clear();
    }

    ControlTaskProxy::ControlTaskProxy(std::string name, bool is_ior) 
        : TaskContext("CORBAProxy") 
    {
        try {
            if (is_ior) {
                // Use the first argument to create the task object reference,
                // in real applications we use the naming service, but let's do
                // the easy part first!
                CORBA::Object_var task_object =
                    orb->string_to_object ( name.c_str() );

                // Now downcast the object reference to the appropriate type
                mtask = Orocos::ControlTask::_narrow (task_object.in ());
            } else {
                // NameService
                CORBA::Object_var rootObj = orb->resolve_initial_references("NameService");
                CosNaming::NamingContext_var rootContext = CosNaming::NamingContext::_narrow(rootObj.in());
                if (CORBA::is_nil(rootContext.in() )) {
                    Logger::log() <<Logger::Error << "ControlTaskProxy could not acquire NameService."<<Logger::endl;
                    throw IllegalServer();
                }
                Logger::log() <<Logger::Debug << "ControlTaskProxy found CORBA NameService."<<Logger::endl;
                CosNaming::Name serverName;
                serverName.length(2);
                serverName[0].id = CORBA::string_dup("ControlTasks");
                serverName[1].id = CORBA::string_dup( name.c_str() );

                // Get object reference
                CORBA::Object_var task_object = rootContext->resolve(serverName);
                mtask = Orocos::ControlTask::_narrow (task_object.in ());
            }
            if ( CORBA::is_nil( mtask.in() ) ) {
                Logger::log() << Logger::Error << "Failed to acquire ControlTaskServer '"+name+"'."<<Logger::endl;
                throw IllegalServer();
            }
            CORBA::String_var nm = mtask->getName(); // force connect to object.
            std::string newname( nm.in() );
            this->mtask_name = newname;
            Logger::log() << Logger::Info << "Successfully connected to ControlTaskServer '"+newname+"'."<<Logger::endl;
            proxies[mtask] = this;
        }
        catch (CORBA::Exception &e) {
            Logger::log() <<Logger::Error<< "CORBA exception raised when resolving Object !" << Logger::endl;
            Logger::log() << e._info().c_str() << Logger::endl;
        }
        catch (...) {
            Logger::log() <<Logger::Error <<"Unknown Exception in ControlTaskProxy construction!"<<Logger::endl;
            throw;
        }
        this->synchronize();
    }

    ControlTaskProxy::ControlTaskProxy( ::Orocos::ControlTask_ptr taskc) 
        : TaskContext("CORBAProxy"), mtask( taskc )
    {
        try {
            CORBA::String_var nm = mtask->getName(); // force connect to object.
            std::string name( nm.in() );
            this->mtask_name = name;
            proxies[mtask] = this;
        }
        catch (CORBA::Exception &e) {
            Logger::log() <<Logger::Error << "CORBA exception raised when creating ControlTaskProxy!" << Logger::nl;
            Logger::log() << e._info().c_str() << Logger::endl;
        }
        catch (...) {
            throw;
        }
        this->synchronize();
    }

    void ControlTaskProxy::synchronize()
    {
        if (!mtask)
            return;

        // load command and method factories.
        // methods:
        Logger::log() << Logger::Info << "Fetching Methods."<<Logger::endl;
        MethodInterface_var mfact = mtask->methods();
        if (mfact) {
            MethodList_var objs;
            objs = mfact->getMethods();
            for ( size_t i=0; i < objs->length(); ++i) {
                this->methods()->add( objs[i].in(), new CorbaMethodFactory( objs[i].in(), mfact.in() ) );
            }
        }
        // commands:
        Logger::log() << Logger::Info << "Fetching Commands."<<Logger::endl;
        CommandInterface_var cfact = mtask->commands();
        if (cfact) {
            CommandList_var objs;
            objs = cfact->getCommands();
            for ( size_t i=0; i < objs->length(); ++i) {
                this->commands()->add( objs[i].in(), new CorbaCommandFactory( objs[i].in(), cfact.in() ) );
            }
        }

        // first do properties:
      Logger::log() << Logger::Info << "Fetching Properties."<<Logger::endl;
        AttributeInterface::PropertyNames_var props = mtask->attributes()->getPropertyList();

        for (size_t i=0; i != props->length(); ++i) {
            if ( this->attributes()->hasProperty( string(props[i].name.in()) ) )
                continue; // previously added.
            Expression_var expr = mtask->attributes()->getProperty( props[i].name.in() );
            if ( CORBA::is_nil( expr ) ) {
                Logger::log() <<Logger::Error <<"Property "<< string(props[i].name.in()) << " present in getPropertyList() but not accessible."<<Logger::endl;
                continue; 
            }
#if 0 // This code may trigger endless recurse if server has recursive prop bags.
      // By using Property<PropertyBag>::narrow( ... ) this is no longer needed.
            // See if it is a PropertyBag:
            CORBA::Any_var any = expr->get();
            PropertyBag bag;
            if ( AnyConversion<PropertyBag>::update( *any, bag ) ) {
                Property<PropertyBag>* pbag = new Property<PropertyBag>( string(props[i].name.in()), string(props[i].description.in()), bag);
                this->attributes()->addProperty( pbag );
                continue;
            }
#endif
            AssignableExpression_var as_expr = AssignableExpression::_narrow( expr.in() );
            // addProperty also adds as attribute...
            if ( CORBA::is_nil( as_expr ) ) {
                Logger::log() <<Logger::Error <<"Property "<< string(props[i].name.in()) << " was not writable !"<<Logger::endl;
            } else {
                // If the type is known, immediately build the correct property and datasource,
                // otherwise, build a property of CORBA::Any.
                CORBA::String_var tn = as_expr->getTypeName();
                TypeInfo* ti = TypeInfoRepository::Instance()->type( tn.in() );
                Logger::log() <<Logger::Info << "Looking up Property " << tn.in();
                if ( ti ) {
                    this->attributes()->addProperty( ti->buildProperty( props[i].name.in(), props[i].description.in(), 
                                                                        ti->buildCorbaProxy( as_expr.in() ) ) );
                    Logger::log() <<Logger::Info <<" found!"<<Logger::endl;
                }
                else {
                    this->attributes()->addProperty( new Property<CORBA::Any_ptr>( string(props[i].name.in()), string(props[i].description.in()), new CORBAAssignableExpression<Property<CORBA::Any_ptr>::DataSourceType>( as_expr.in() ) ) );
                    Logger::log()  <<Logger::Info<<" not found :-("<<Logger::endl;
                }
            }
        }

      Logger::log() << Logger::Info << "Fetching Attributes."<<Logger::endl;
        // add attributes not yet added by properties:
        AttributeInterface::AttributeNames_var attrs = mtask->attributes()->getAttributeList();
        
        for (size_t i=0; i != attrs->length(); ++i) {
            if ( this->attributes()->hasAttribute( string(attrs[i].in()) ) )
                continue; // previously added.
            Expression_var expr = mtask->attributes()->getAttribute( attrs[i].in() );
            if ( CORBA::is_nil( expr ) ) {
                Logger::log() <<Logger::Error <<"Attribute "<< string(attrs[i].in()) << " present in getAttributeList() but not accessible."<<Logger::endl;
                continue; 
            }
            AssignableExpression_var as_expr = AssignableExpression::_narrow( expr.in()  );
            // If the type is known, immediately build the correct attribute and datasource,
            // otherwise, build a attribute of CORBA::Any.
            CORBA::String_var tn = expr->getTypeName();
            TypeInfo* ti = TypeInfoRepository::Instance()->type( tn.in() );
            Logger::log() <<Logger::Info << "Looking up Attribute " << tn.in();
            if ( ti ) {
                Logger::log() <<": found!"<<Logger::endl;
                if ( CORBA::is_nil( as_expr ) ) {
                    this->attributes()->setValue( ti->buildConstant( attrs[i].in(), ti->buildCorbaProxy( expr.in() ) ) );
                }
                else {
                    this->attributes()->setValue( ti->buildAttribute( attrs[i].in(), ti->buildCorbaProxy( as_expr.in() ) ) );
                }
            } else {
                Logger::log() <<": not found :-("<<Logger::endl;
                if ( CORBA::is_nil( as_expr ) )
                    this->attributes()->setValue( new Constant<CORBA::Any_ptr>( attrs[i].in(), new CORBAExpression<CORBA::Any_ptr>( expr.in() ) ) );
                else
                this->attributes()->setValue( new Attribute<CORBA::Any_ptr>( attrs[i].in(), new CORBAAssignableExpression<CORBA::Any_ptr>( as_expr.in() ) ) );
            }
        }

      Logger::log() << Logger::Info << "Fetching ScriptingAccess."<<Logger::endl;
      Orocos::ScriptingAccess_var saC = mtask->scripting();
      if ( saC ) {
          delete mscriptAcc;
          mscriptAcc = new ScriptingAccessProxy( saC.in() );
      }

      Logger::log() << Logger::Info << "All Done."<<Logger::endl;
    }

    bool ControlTaskProxy::InitOrb(int argc, char* argv[] ) {
        if ( orb.in() )
            return false;

        try {
            // First initialize the ORB, that will remove some arguments...
            orb =
                CORBA::ORB_init (argc, const_cast<char**>(argv),
                                 "" /* the ORB name, it can be anything! */);
            // Also activate the POA Manager, since we may get call-backs !
            CORBA::Object_var poa_object =
                orb->resolve_initial_references ("RootPOA");
            PortableServer::POA_var poa =
                PortableServer::POA::_narrow (poa_object.in ());
            PortableServer::POAManager_var poa_manager =
                poa->the_POAManager ();
            poa_manager->activate ();
            return true;
        }
        catch (CORBA::Exception &e) {
            Logger::log() <<Logger::Error << "Orb Init : CORBA exception raised!" << Logger::nl;
            Logger::log() << e._info().c_str() << Logger::endl;
        }
        return false;
    }

    void ControlTaskProxy::DestroyOrb()
    {
        try {
            // Destroy the POA, waiting until the destruction terminates
            //poa->destroy (1, 1);
            orb->destroy();
            std::cerr <<"Orb destroyed."<<std::endl;
        }
        catch (CORBA::Exception &e) {
            Logger::log() <<Logger::Error << "Orb Init : CORBA exception raised!" << Logger::nl;
            Logger::log() << e._info().c_str() << Logger::endl;
        }
    }


    ControlTaskProxy* ControlTaskProxy::Create(std::string name, bool is_ior /*=false*/) {
        if ( CORBA::is_nil(orb) || name.empty() )
            return 0;

        // create new:
        try {
            ControlTaskProxy* ctp = new ControlTaskProxy( name, is_ior );
            return ctp;
        }
        catch( IllegalServer& is ) {
            cerr << is.what() << endl;
        }
        return 0;
    }

    ControlTaskProxy* ControlTaskProxy::Create(::Orocos::ControlTask_ptr t) {
        if ( CORBA::is_nil(orb) || t == 0 )
            return 0;

        // proxy present for this object ?
        if ( proxies.count( t ) )
            return proxies[t];

        // create new:
        try {
            ControlTaskProxy* ctp = new ControlTaskProxy( t );
            return ctp;
        }
        catch( IllegalServer& is ) {
            cerr << is.what() << endl;
        }
        return 0;
    }

    bool ControlTaskProxy::executeCommand( CommandInterface* c)
    {
        return false;
    }

    int ControlTaskProxy::queueCommand( CommandInterface* c)
    {
        return 0;
    }

    void ControlTaskProxy::setName(const std::string& n)
    {
        //mtask->setName( n.c_str() );
    }

    bool ControlTaskProxy::addPeer( TaskContext* peer, std::string alias /*= ""*/ )
    {
        // if peer is a proxy, add the proxy, otherwise, create new server.
        ControlTaskProxy* ctp = dynamic_cast<ControlTaskProxy*>( peer );
        if (ctp) {
            return mtask->addPeer( ctp->server(), alias.c_str() );
        }
        // no server yet, create it.
        ControlTaskServer* newpeer = ControlTaskServer::Create(peer);
        return mtask->addPeer( newpeer->server(), alias.c_str() );
    }

    void ControlTaskProxy::removePeer( const std::string& name )
    {
        mtask->removePeer( name.c_str() );
    }

    bool ControlTaskProxy::connectPeers( TaskContext* peer )
    {
        ControlTaskServer* newpeer = ControlTaskServer::Create(peer);
        return mtask->connectPeers( newpeer->server() );
    }

    void ControlTaskProxy::disconnectPeers( const std::string& name )
    {
        mtask->disconnectPeers( name.c_str() );
    }

    TaskContext::PeerList ControlTaskProxy::getPeerList() const
    {
        Orocos::ControlTask::ControlTaskNames_var plist = mtask->getPeerList();
        TaskContext::PeerList vlist;
        for( size_t i =0; i != plist->length(); ++i)
            vlist.push_back( std::string( plist[i] ) );
        return vlist;
    }

    bool ControlTaskProxy::hasPeer( const std::string& peer_name ) const
    {
        return mtask->hasPeer( peer_name.c_str() );
    }

    TaskContext* ControlTaskProxy::getPeer(const std::string& peer_name ) const
    {
        Orocos::ControlTask_ptr ct = mtask->getPeer( peer_name.c_str() );
        if ( !ct )
            return 0;
        return ControlTaskProxy::Create( ct );
    }

    Orocos::ControlTask_ptr ControlTaskProxy::server() const {
        return Orocos::ControlTask::_duplicate(mtask);
    }

    CosPropertyService::PropertySet_ptr ControlTaskProxy::propertySet() {
        return mtask->propertySet();
    }
    
}}

