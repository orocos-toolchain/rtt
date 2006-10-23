#ifndef RTT_CORBA_POAUTILITY_H_
#define RTT_CORBA_POAUTILITY_H_

#include <tao/corba.h>
#include <tao/PortableServer/PortableServer.h>



class POAUtility {

public:
        
    static PortableServer::POA_ptr
    create_basic_POA(
        PortableServer::POA_ptr           parentPOAP,
        PortableServer::POAManager_ptr    POAManagerP,
        const char *                            POAName,
	CORBA::Boolean                    isMultiThread,
	CORBA::Boolean                    isPersistent
    );


    static PortableServer::POA_ptr
    create_service_POA(
        PortableServer::POA_ptr           parentPOAP,
        PortableServer::POAManager_ptr    POAManagerP,
        const char *                            POAName,
	CORBA::Boolean                    isMultiThread
    );


    static PortableServer::POA_ptr
    create_servant_activator_POA(
        PortableServer::POA_ptr           parentPOAP,
        PortableServer::POAManager_ptr    POAManagerP,
        const char *                            POAName,
	CORBA::Boolean                    isMultiThread,
	CORBA::Boolean                    isPersistent
    );


    static PortableServer::POA_ptr
    create_servant_locator_POA(
        PortableServer::POA_ptr           parentPOAP,
        PortableServer::POAManager_ptr    POAManagerP,
        const char *                            POAName,
	CORBA::Boolean                    isMultiThread,
	CORBA::Boolean                    isPersistent
    );


    static PortableServer::POA_ptr
    create_default_servant_POA(
        PortableServer::POA_ptr           parentPOAP,
        PortableServer::POAManager_ptr    POAManagerP,
        const char *                            POAName,
	CORBA::Boolean                    isMultiThread
    );

};

#endif
