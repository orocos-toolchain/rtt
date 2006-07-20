/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:09:49 CET 2006  ControlTaskServer.cxx 

                        ControlTaskServer.cxx -  description
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
 
 

#include "rtt/corba/ControlTaskServer.hpp"
#include "rtt/corba/ControlTaskS.h"
#include "rtt/corba/ControlTaskC.h"
#include "rtt/corba/ControlTaskI.h"
#include <orbsvcs/CosNamingC.h>
#include <iostream>
#include <fstream>

// ACE Specific, for printing exceptions.
#include <ace/SString.h>

namespace Corba
{
    using namespace std;
    
    

    std::map<TaskContext*, ControlTaskServer*> ControlTaskServer::servers;

  ControlTaskServer::~ControlTaskServer()
  {

    servers.erase(mtaskcontext);

    if (muse_naming) {
        try {
            CORBA::Object_var rootObj = orb->resolve_initial_references("NameService");
            CosNaming::NamingContext_var rootNC = CosNaming::NamingContext::_narrow(rootObj.in());
            
            if (CORBA::is_nil( rootNC.in() ) ) {
                Logger::log() <<Logger::Warning << "ControlTask '"<< mtaskcontext->getName() << "' could not find CORBA Naming Service."<<Logger::nl;
            } else {
                // Nameserver found...
                CosNaming::Name name;
                name.length(2);
                name[0].id = CORBA::string_dup("ControlTasks");
                name[1].id = CORBA::string_dup( mtaskcontext->getName().c_str() );
                try {
                    rootNC->unbind(name);
                    Logger::log() <<Logger::Info << "Successfully removed ControlTask '"<<mtaskcontext->getName()<<"' from CORBA Naming Service."<<Logger::endl;
                }
                catch( ... ) {
                    Logger::log() <<Logger::Warning << "ControlTask '"<< mtaskcontext->getName() << "' unbinding failed."<<Logger::endl;
                }
            }
        } catch (...) {
            Logger::log() <<Logger::Warning << "ControlTask '"<< mtaskcontext->getName() << "' unbinding failed from CORBA Naming Service."<<Logger::endl;
        }
    }
  }


  

    ControlTaskServer::ControlTaskServer(TaskContext* taskc, bool use_naming)
      :mtaskcontext(taskc), muse_naming(use_naming)
    {
        servers[taskc] = this;

        try {
            // The servant : TODO : cleanup servant in destructor !
            Orocos_ControlTask_i* the_task = new Orocos_ControlTask_i( taskc );
            // move into var ptr.
            PortableServer::ServantBase_var  servant = the_task;
            // get default poa.
            PortableServer::POA_var poa = servant.in()->_default_POA();
            // explicit activation:
            PortableServer::ObjectId_var objId = poa->activate_object( servant.in() );
            // get object reference:
            CORBA::Object_var obj = poa->id_to_reference( objId.in() );
            mtask = Orocos::ControlTask::_narrow( obj.in() );

            if ( use_naming ) {
                CORBA::Object_var rootObj = orb->resolve_initial_references("NameService");
                CosNaming::NamingContext_var rootNC = CosNaming::NamingContext::_narrow(rootObj.in());
            
                if (CORBA::is_nil( rootNC.in() ) ) {
                    Logger::log() <<Logger::Warning << "ControlTask '"<< taskc->getName() << "' could not find CORBA Naming Service."<<Logger::nl;
                    Logger::log() <<"Writing IOR to 'std::cerr' and file '" << taskc->getName() <<".ior'"<<Logger::endl;

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
                Logger::log() <<Logger::Info << "ControlTask '"<< taskc->getName() << "' found CORBA Naming Service."<<Logger::endl;
                // Nameserver found...
                CosNaming::Name name;
                name.length(1);
                name[0].id = CORBA::string_dup("ControlTasks");
                CosNaming::NamingContext_var controlNC;
                try {
                    controlNC = rootNC->bind_new_context(name);
                }
                catch( CosNaming::NamingContext::AlreadyBound&) {
                    Logger::log() <<Logger::Debug << "NamingContext 'ControlTasks' already bound to CORBA Naming Service."<<Logger::endl;
                    // NOP.
                }

                name.length(2);
                name[1].id = CORBA::string_dup( taskc->getName().c_str() );
                try {
                    rootNC->bind(name, obj.in() );
                    Logger::log() <<Logger::Info << "Successfully added ControlTask '"<<taskc->getName()<<"' to CORBA Naming Service."<<Logger::endl;
                }
                catch( CosNaming::NamingContext::AlreadyBound&) {
                    Logger::log() <<Logger::Warning << "ControlTask '"<< taskc->getName() << "' already bound to CORBA Naming Service."<<Logger::endl;
                    Logger::log() <<"Trying to rebind...";
                    try {
                        rootNC->rebind(name, obj.in() );
                    } catch( ... ) {
                        Logger::log() << " failed!"<<Logger::endl;
                        return;
                    }
                    Logger::log() << " done. New ControlTask bound to Naming Service."<<Logger::endl;
                }
            } // use_naming
            else {
                Logger::log() <<Logger::Info <<"ControlTask '"<< taskc->getName() << "' is not using the CORBA Naming Service."<<Logger::endl;
                Logger::log() <<"Writing IOR to 'std::cerr' and file '" << taskc->getName() <<".ior'"<<Logger::endl;
                
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
            Logger::log() <<Logger::Error << "CORBA exception raised!" << Logger::endl;
            Logger::log() << e._info().c_str() << Logger::endl;
        }

    }

    bool ControlTaskServer::InitOrb(int argc, char* argv[] ) {
        if ( orb.in() )
            return false;
        try {
            // First initialize the ORB, that will remove some arguments...
            orb =
                CORBA::ORB_init (argc, argv,
                                 "" /* the ORB name, it can be anything! */);
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

    void ControlTaskServer::CleanupServers() {
        if ( !CORBA::is_nil(orb) ) {
            Logger::log() <<Logger::Info << "Cleaning up ControlTaskServers..."<<Logger::nl;
            ServerMap::iterator it = servers.begin();
            while ( !servers.empty() ){
                delete servers.begin()->second;
            }
            Logger::log() << "Cleanup done."<<Logger::endl;
        }
    }

    void ControlTaskServer::ShutdownOrb(bool wait_for_completion) 
    {
      if ( CORBA::is_nil(orb) ) {
	Logger::log() <<Logger::Error << "Orb Shutdown...failed! Orb is nil." << Logger::endl;
	return;
      }
        try {
            orb->shutdown( wait_for_completion );
	    Logger::log() <<Logger::Info << "Orb Shutdown...done." << Logger::endl;
        }
        catch (CORBA::Exception &e) {
            Logger::log() <<Logger::Error << "Orb Shutdown...failed! CORBA exception raised." << Logger::nl;
            Logger::log() << e._info().c_str() << Logger::endl;
        }
    }

    void ControlTaskServer::RunOrb()
    {
        try {
            orb->run();
            Logger::log() << Logger::Info <<"Breaking out of orb->run()."<<Logger::endl;
        }
        catch (CORBA::Exception &e) {
            Logger::log() <<Logger::Error << "Orb Run : CORBA exception raised!" << Logger::nl;
            Logger::log() << e._info().c_str() << Logger::endl;
        }
    }

    void ControlTaskServer::DestroyOrb()
    {
        try {
            // Destroy the POA, waiting until the destruction terminates
            //poa->destroy (1, 1);
	    CleanupServers();
            orb->destroy();
            Logger::log() << Logger::Info <<"Orb destroyed."<<Logger::endl;
        }
        catch (CORBA::Exception &e) {
            Logger::log() <<Logger::Error << "Orb Destroy : CORBA exception raised!" << Logger::nl;
            Logger::log() << e._info().c_str() << Logger::endl;
        }
    }

    ControlTaskServer* ControlTaskServer::Create(TaskContext* tc, bool use_naming) {
        if ( !orb )
            return 0;

        if ( servers.count(tc) ) {
            Logger::log() << Logger::Debug << "Returning existing ControlTaskServer for "<<tc->getName()<<Logger::nl;
            return servers.find(tc)->second;
        }
        // create new:
        Logger::log() << Logger::Info << "Creating new ControlTaskServer for "<<tc->getName()<<Logger::nl;
        ControlTaskServer* cts = new ControlTaskServer(tc, use_naming);
        return cts;
    }

    Orocos::ControlTask_ptr ControlTaskServer::server() const
    {
        return Orocos::ControlTask::_duplicate(mtask.in());
    }

}
