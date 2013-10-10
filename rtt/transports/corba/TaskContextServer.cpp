/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:09:49 CET 2006  TaskContextServer.cxx

                        TaskContextServer.cxx -  description
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



#include "TaskContextServer.hpp"
#include "TaskContextProxy.hpp"
#include "corba.h"
#ifdef CORBA_IS_TAO
#include "TaskContextS.h"
#include <orbsvcs/CosNamingC.h>
// ACE Specific, for printing exceptions.
#include <ace/SString.h>
#include "tao/TimeBaseC.h"
#include "tao/Messaging/Messaging.h"
#include "tao/Messaging/Messaging_RT_PolicyC.h"
#else
#include <omniORB4/Naming.hh>
#endif
#include "TaskContextC.h"
#include "TaskContextI.h"
#include "DataFlowI.h"
#include "POAUtility.h"
#include <iostream>
#include <fstream>

#include "../../os/threads.hpp"
#include "../../Activity.hpp"

namespace RTT
{namespace corba
{
    using namespace std;

    std::map<TaskContext*, TaskContextServer*> TaskContextServer::servers;

    base::ActivityInterface* TaskContextServer::orbrunner = 0;

    bool TaskContextServer::is_shutdown = false;

    std::map<TaskContext*, std::string> TaskContextServer::iors;

  TaskContextServer::~TaskContextServer()
  {
    Logger::In in("~TaskContextServer()");
    servers.erase(mtaskcontext);

    // Remove taskcontext ior reference
    iors.erase(mtaskcontext);

    PortableServer::ObjectId_var oid = mpoa->servant_to_id(mtask_i.in());
    mpoa->deactivate_object(oid);

    if (muse_naming) {
        try {
            CORBA::Object_var rootObj = orb->resolve_initial_references("NameService");
            CosNaming::NamingContext_var rootNC = CosNaming::NamingContext::_narrow(rootObj.in());

            if (CORBA::is_nil( rootNC.in() ) ) {
                log(Warning) << "CTaskContext '"<< mtaskcontext->getName() << "' could not find CORBA Naming Service."<<endlog();
            } else {
                // Nameserver found...
                CosNaming::Name name;
                name.length(2);
                name[0].id = CORBA::string_dup("TaskContexts");
                name[1].id = CORBA::string_dup( mtaskcontext->getName().c_str() );
                try {
                    rootNC->unbind(name);
                    log(Info) << "Successfully removed CTaskContext '"<<mtaskcontext->getName()<<"' from CORBA Naming Service."<<endlog();
                }
                catch( CosNaming::NamingContext::NotFound ) {
                    log(Info) << "CTaskContext '"<< mtaskcontext->getName() << "' task was already unbound."<<endlog();
                }
                catch( ... ) {
                    log(Warning) << "CTaskContext '"<< mtaskcontext->getName() << "' unbinding failed."<<endlog();
                }
            }
        } catch (...) {
            log(Warning) << "CTaskContext '"<< mtaskcontext->getName() << "' unbinding failed from CORBA Naming Service."<<endlog();
        }
    }
  }




    TaskContextServer::TaskContextServer(TaskContext* taskc, bool use_naming, bool require_name_service)
      : mtaskcontext(taskc), muse_naming(use_naming)
    {
        Logger::In in("TaskContextServer()");
        servers[taskc] = this;
        try {
            // Each server has its own POA.
            // The server's objects have their own poa as well.
            CORBA::Object_var poa_object =
                orb->resolve_initial_references ("RootPOA");
            mpoa = PortableServer::POA::_narrow(poa_object);
            PortableServer::POAManager_var poa_manager =
                mpoa->the_POAManager ();

            //poa = POAUtility::create_basic_POA( poa, poa_manager, taskc->getName().c_str(), 0, 1);
            //            poa_manager->activate ();

            // TODO : Use a better suited POA than create_basic_POA, use the 'session' or so type
            // But watch out: we need implicit activation, our you will get exceptions upon ->_this()
            // The POA for the Server's objects:
//             PortableServer::POA_var objpoa = POAUtility::create_basic_POA(poa,
//                                                               poa_manager,
//                                                               std::string(taskc->getName() + "OBJPOA").c_str(),
//                                                               0, 0); // Not persistent, allow implicit.

            // The servant : TODO : cleanup servant in destructor !
            RTT_corba_CTaskContext_i* serv;
            mtask_i = serv = new RTT_corba_CTaskContext_i( taskc, mpoa );
            mtask   = serv->activate_this();

            // Store reference to iors
            CORBA::String_var ior = orb->object_to_string( mtask.in() );
            iors[taskc] = std::string( ior.in() );

            if ( use_naming ) {
                CORBA::Object_var rootObj;
                CosNaming::NamingContext_var rootNC;
                try {
                    rootObj = orb->resolve_initial_references("NameService");
                    rootNC = CosNaming::NamingContext::_narrow(rootObj);
                } catch (...) {}

                if (CORBA::is_nil( rootNC ) ) {
                    std::string  err("CTaskContext '" + taskc->getName() + "' could not find CORBA Naming Service.");
                    if (require_name_service) {
                        servers.erase(taskc);
                        log(Error) << err << endlog();
                        servers.erase(taskc);
                        throw IllegalServer(err);
                    }
                    else
                    {
                        log(Warning) << err << endlog();
                        log() <<"Writing IOR to 'std::cerr' and file '" << taskc->getName() <<".ior'"<<endlog();

                        // this part only publishes the IOR to a file.
                        CORBA::String_var ior = orb->object_to_string( mtask.in() );
                        std::cerr << ior.in() <<std::endl;
                        {
                            // write to a file as well.
                            std::string iorname( taskc->getName());
                            iorname += ".ior";
                            std::ofstream file_ior( iorname.c_str() );
                            file_ior << ior.in() <<std::endl;
                        }
                        return;
                    }
                }
                log(Info) << "CTaskContext '"<< taskc->getName() << "' found CORBA Naming Service."<<endlog();
                // Nameserver found...
                CosNaming::Name name;
                name.length(1);
                name[0].id = CORBA::string_dup("TaskContexts");
                CosNaming::NamingContext_var controlNC;
                try {
                    controlNC = rootNC->bind_new_context(name);
                }
                catch( CosNaming::NamingContext::AlreadyBound&) {
                    log(Debug) << "NamingContext 'TaskContexts' already bound to CORBA Naming Service."<<endlog();
                    // NOP.
                }

                name.length(2);
                name[1].id = CORBA::string_dup( taskc->getName().c_str() );
                try {
                    rootNC->bind(name, mtask );
                    log(Info) << "Successfully added CTaskContext '"<<taskc->getName()<<"' to CORBA Naming Service."<<endlog();
                }
                catch( CosNaming::NamingContext::AlreadyBound&) {
                    log(Warning) << "CTaskContext '"<< taskc->getName() << "' already bound to CORBA Naming Service."<<endlog();
                    log() <<"Trying to rebind...";
                    try {
                        rootNC->rebind(name, mtask);
                    } catch( ... ) {
                        log() << " failed!"<<endlog();
                        return;
                    }
                    log() << " done. New CTaskContext bound to Naming Service."<<endlog();
                }
            } // use_naming
            else {
                log(Info) <<"CTaskContext '"<< taskc->getName() << "' is not using the CORBA Naming Service."<<endlog();
                log() <<"Writing IOR to 'std::cerr' and file '" << taskc->getName() <<".ior'"<<endlog();

                // this part only publishes the IOR to a file.
                CORBA::String_var ior = orb->object_to_string( mtask.in() );
                std::cerr << ior.in() <<std::endl;
                {
                    // write to a file as well.
                    std::string iorname( taskc->getName());
                    iorname += ".ior";
                    std::ofstream file_ior( iorname.c_str() );
                    file_ior << ior.in() <<std::endl;
                }
                return;
            }
        }
        catch (CORBA::Exception &e) {
            log(Error) << "CORBA exception raised!" << endlog();
            log() << CORBA_EXCEPTION_INFO(e) << endlog();
        }

    }

    void TaskContextServer::CleanupServers() {
        if ( !CORBA::is_nil(orb) && !is_shutdown) {
            log(Info) << "Cleaning up TaskContextServers..."<<endlog();
            while ( !servers.empty() ){
                delete servers.begin()->second;
                // note: destructor will self-erase from map !
            }
            CDataFlowInterface_i::clearServants();
            log() << "Cleanup done."<<endlog();
        }
    }

    void TaskContextServer::CleanupServer(TaskContext* c) {
        if ( !CORBA::is_nil(orb) ) {
            ServerMap::iterator it = servers.find(c);
            if ( it != servers.end() ){
                log(Info) << "Cleaning up TaskContextServer for "<< c->getName()<<endlog();
                CDataFlowInterface_i::deregisterServant(c->provides().get());
                delete it->second; // destructor will do the rest.
                // note: destructor will self-erase from map !
            }
        }
    }

    void TaskContextServer::ShutdownOrb(bool wait_for_completion)
    {
        Logger::In in("ShutdownOrb");
        DoShutdownOrb(wait_for_completion);
    }

    void TaskContextServer::DoShutdownOrb(bool wait_for_completion)
    {
        if (is_shutdown) {
            log(Info) << "Orb already down..."<<endlog();
            return;
        }
        if ( CORBA::is_nil(orb) ) {
            log(Error) << "Orb Shutdown...failed! Orb is nil." << endlog();
            return;
        }

        try {
            CleanupServers(); // can't do this after an orb->shutdown().
            log(Info) << "Orb Shutdown...";
            is_shutdown = true;
            if (wait_for_completion)
                log(Info)<<"waiting..."<<endlog();
            orb->shutdown( wait_for_completion );
            log(Info) << "done." << endlog();
        }
        catch (CORBA::Exception &e) {
            log(Error) << "Orb Shutdown...failed! CORBA exception raised." << endlog();
            log() << CORBA_EXCEPTION_INFO(e) << endlog();
            return;
        }
    }


    void TaskContextServer::RunOrb()
    {
        if ( CORBA::is_nil(orb) ) {
            log(Error) << "RunOrb...failed! Orb is nil." << endlog();
            return;
        }
        try {
            log(Info) <<"Entering orb->run()."<<endlog();
            orb->run();
            log(Info) <<"Breaking out of orb->run()."<<endlog();
        }
        catch (CORBA::Exception &e) {
            log(Error) << "Orb Run : CORBA exception raised!" << endlog();
            log() << CORBA_EXCEPTION_INFO(e) << endlog();
        }
    }

    /**
     * Class which runs an orb in an Orocos thread.
     */
    class OrbRunner
        : public Activity
    {
    public:
        OrbRunner()
            : Activity(RTT::os::LowestPriority)
        {}
        void loop()
        {
            Logger::In in("OrbRunner");
            TaskContextServer::RunOrb();
        }

        bool breakLoop()
        {
            return true;
        }

        void finalize()
        {
            Logger::In in("OrbRunner");
            log(Info) <<"Safely stopped."<<endlog();
        }
    };

    void TaskContextServer::ThreadOrb()
    {
        Logger::In in("ThreadOrb");
        if ( CORBA::is_nil(orb) ) {
            log(Error) << "ThreadOrb...failed! Orb is nil." << endlog();
            return;
        }
        if (orbrunner != 0) {
            log(Error) <<"Orb already running in a thread."<<endlog();
        } else {
            log(Info) <<"Starting Orb in a thread."<<endlog();
            orbrunner = new OrbRunner();

            orbrunner->start();
        }
    }

    void TaskContextServer::DestroyOrb()
    {
        Logger::In in("DestroyOrb");
        if ( CORBA::is_nil(orb) ) {
            log(Error) << "DestroyOrb...failed! Orb is nil." << endlog();
            return;
        }

        if (orbrunner) {
            orbrunner->stop();
            delete orbrunner;
            orbrunner = 0;
        }

        try {
            // Destroy the POA, waiting until the destruction terminates
            //poa->destroy (1, 1);
            CleanupServers();
            orb->destroy();
            rootPOA = 0;
            orb = 0;
            log(Info) <<"Orb destroyed."<<endlog();
        }
        catch (CORBA::Exception &e) {
            log(Error) << "Orb Destroy : CORBA exception raised!" << endlog();
            log() << CORBA_EXCEPTION_INFO(e) << endlog();
        }

    }

    TaskContextServer* TaskContextServer::Create(TaskContext* tc, bool use_naming, bool require_name_service) {
        if ( CORBA::is_nil(orb) )
            return 0;

        if ( servers.count(tc) ) {
            log(Debug) << "Returning existing TaskContextServer for "<<tc->getName()<<endlog();
            return servers.find(tc)->second;
        }

        // create new:
        log(Info) << "Creating new TaskContextServer for "<<tc->getName()<<endlog();
        try {
            TaskContextServer* cts = new TaskContextServer(tc, use_naming, require_name_service);
            return cts;
        }
        catch( IllegalServer& is ) {
            cerr << is.what() << endl;
        }
        return 0;
    }

    CTaskContext_ptr TaskContextServer::CreateServer(TaskContext* tc, bool use_naming, bool require_name_service) {
        if ( CORBA::is_nil(orb) )
            return CTaskContext::_nil();

        if ( servers.count(tc) ) {
            log(Debug) << "Returning existing TaskContextServer for "<<tc->getName()<<endlog();
            return CTaskContext::_duplicate( servers.find(tc)->second->server() );
        }

        for (TaskContextProxy::PMap::iterator it = TaskContextProxy::proxies.begin(); it != TaskContextProxy::proxies.end(); ++it)
            if ( (it->first) == tc ) {
                log(Debug) << "Returning server of Proxy for "<<tc->getName()<<endlog();
                return CTaskContext::_duplicate(it->second);
            }

        // create new:
        log(Info) << "Creating new TaskContextServer for "<<tc->getName()<<endlog();
        try {
            TaskContextServer* cts = new TaskContextServer(tc, use_naming, require_name_service);
            return CTaskContext::_duplicate( cts->server() );
        }
        catch( IllegalServer& is ) {
            cerr << is.what() << endl;
        }
        return CTaskContext::_nil();
    }


    corba::CTaskContext_ptr TaskContextServer::server() const
    {
        // we're not a factory function, so we don't _duplicate.
        return mtask.in();
    }

    std::string TaskContextServer::getIOR(TaskContext* tc)
    {
	IorMap::const_iterator it = iors.find(tc);
	if (it != iors.end())
	    return it->second;

	return std::string("");
    }

}}
