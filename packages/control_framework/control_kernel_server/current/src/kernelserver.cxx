
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
