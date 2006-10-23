#include "corba/Services.hpp"
#include "corba/ApplicationServer.hpp"
#include "Logger.hpp"
#include "corba/ControlTaskProxy.hpp"

// See comment below.
#if 0
// The TAO implementation of a single event channel.
#include <orbsvcs/CosEvent/CEC_EventChannel.h>
#include <orbsvcs/CosEvent/CEC_Default_Factory.h>
#endif

namespace RTT 
{ namespace Corba {

    bool Services::MayShutdown = true;

    CosNaming::NamingContext_var Services::naming_context_;

    CosNaming::NamingContext_ptr Services::getNamingContext()
    {
        if ( CORBA::is_nil( naming_context_ ) ) {
            CORBA::Object_var naming_obj =
                ApplicationServer::orb->resolve_initial_references ("NameService" );

            // Need to check return value for errors.
            if (CORBA::is_nil (naming_obj.in ())) {
                log(Error) << "Could not locate Naming service." <<endlog();
                ACE_THROW (CORBA::UNKNOWN ());
            }

            naming_context_ =
                CosNaming::NamingContext::_narrow (naming_obj.in () );
            log(Info) << "Corba Naming Service found." <<endlog();
        }
        return CosNaming::NamingContext::_duplicate( naming_context_.in() );
    }

    /*
     * This code worked but since event channels are for now no longer used,
     * it is removed from the Services class.
     */
#if 0
    CosNotifyChannelAdmin::EventChannelFactory_ptr Services::getEventChannelFactory()
    {
        if ( CORBA::is_nil( notify_factory_ ) ) {
            // we need the naming service:
            getNamingContext();

            CosNaming::Name name (1);
            name.length (1);
            name[0].id = CORBA::string_dup ("NotifyEventChannelFactory");

            CORBA::Object_var obj;
            try {
                obj = naming_context_->resolve (name);
            } catch (...) {
                log(Error) << "Exception during resolving of Notification service." <<endlog();
            }
                
            if (CORBA::is_nil( obj ) ) {
                log(Error) << "Could not locate Notification service." <<endlog();
                return 0;
            }

            log(Info) << "Corba Notification Service found." <<endlog();
            notify_factory_ =
                CosNotifyChannelAdmin::EventChannelFactory::_narrow (obj.in () );
        }
        return CosNotifyChannelAdmin::EventChannelFactory::_duplicate( notify_factory_.in() );
    }

    CosEventChannelAdmin::EventChannel_ptr Services::createChannel()
    {
        log(Info) << "Creating Event Channel." << endlog();
#if 0
        // the Notification service is unusable since it does not implement the Pull interfaces !!!
        CosNotification::QoSProperties initial_qos;
        CosNotification::AdminProperties initial_admin;
        CosNotifyChannelAdmin::ChannelID id;

        getEventChannelFactory();
        try {
            if ( notify_factory_ )
                return notify_factory_->create_channel(initial_qos, initial_admin, id);
        } catch (...) {
            log(Error) << "Exception thrown while calling create_channel of Notification Service."<< endlog();
        }
        log(Error) << "No Event Channel created." << endlog();
        return 0;
#endif

        // create a default channel, use the Root POA.
        TAO_CEC_EventChannel_Attributes attributes(ControlTaskProxy::ProxyPOA(), ControlTaskProxy::ProxyPOA() );
        
        TAO_CEC_EventChannel* ec = new TAO_CEC_EventChannel( attributes, new TAO_CEC_Default_Factory(), 1 );

        log(Info) << "Created a TAO_CEC_EventChannel." << endlog();
        // implicitly activate it.
        return ec->_this();
    }
#endif    

}}
