/***************************************************************************
  tag: Peter Soetens  Tue Apr 5 16:53:25 CEST 2005  kernelserver.cxx 

                        kernelserver.cxx -  description
                           -------------------
    begin                : Tue April 05 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
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
 
 

#include <iostream>
#include <fstream>
#include <tao/corba.h>
#include "KernelInterfaceI.h"
#include "kernelinterface.hpp"
#include <control_kernel/kernelserver.hpp>
#include <boost/bind.hpp>

CORBA::ORB_var orb;

void shutdownOrb() 
{
    orb->shutdown(0);
}

int startServer(int argc, char* argv[] )
{
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

        // The servant :
        ExecutionClient::KernelInterface* _k = ExecutionClient::KernelInterface::instance();
        Orocos_KernelInterface_i kernel_i( _k);
        Orocos::KernelInterface_var kernel = kernel_i._this();

        CORBA::String_var ior = orb->object_to_string( kernel.in() );
        std::cerr << ior.in() <<std::endl;
        {
            // write to a file as well.
            std::ofstream file_ior("file.ior");
            file_ior << ior.in() <<std::endl;
        }

        // Tell the servant what function to call on requestShutdown() :
        boost::function<void(void)> hook = shutdownOrb;
        _k->setShutdownHook( hook );

        orb->run();
        std::cerr <<"Breaking out of orb->run()"<<std::endl;
        // Destroy the POA, waiting until the destruction terminates
        //poa->destroy (1, 1);
        orb->destroy ();
    }
    catch (CORBA::Exception &ex) {
        std::cerr << "CORBA exception raised!" << std::endl;
    }
    return 0;
}
